var util = require('util');
var EventEmitter = require('events').EventEmitter;
var api = require("./lib/xbee-api.js");
var serialport = require("serialport");
var async = require('async');
var os = require('os');

var C = exports.C = api.Constants;
var Tools = exports.Tools = api.tools;

function XBee(options) { 
  EventEmitter.call(this);

  // Option Parsing
  if (typeof options === 'string') {
    this.options = { port: options };
  } else {
    this.options = options;
  }

  this.tools = Tools; // Remove this, use XBee.Tools instead

  this.data_parser = options.data_parser || undefined;

  this.use_heartbeat = options.use_heartbeat || false;
  this.heartbeat_packet = options.heartbeat_packet || '```';
  this.heartbeat_timeout = options.heartbeat_timeout || 8000;

  // How long (in ms) shall we wait before deciding that a transmit hasn't been successful?
  this.transmit_status_timeout = options.transmit_status_timeout || 1000;

  if (options.api_mode) api.api_mode = options.api_mode;

  // Current nodes
  this.nodes = {};
}

util.inherits(XBee, EventEmitter);

XBee.prototype._makeTask = function(packet) {
  var self = this;
  return function Writer(cb) {
    //console.log("<<< "+util.inspect(packet.data));
    //console.log("<<< "+packet.data);

    var timeout = setTimeout(function() {
      cb({ msg: "Never got Transmit status from XBee" });
    }, self.transmit_status_timeout );
    self.serial.write(packet.data, function(err, results) {
      if (err) {
        cb(err);
      } else {
        //console.log(util.inspect(packet.data));
        if (results != packet.data.length) return cb(new Error("Not all bytes written"));
        self.serial.once(packet.cbid, function(packet) {
          //console.log("Got Respones: "+packet.cbid);
          clearTimeout(timeout);
          var error = null;
          if (packet.commandStatus !== undefined) {
            if (packet.commandStatus != C.COMMAND_STATUS.OK) {
              error = C.COMMAND_STATUS[packet.commandStatus];
            }
            packet = packet.commandData;
          } else if (packet.deliveryStatus !== undefined) {
            if (packet.deliveryStatus != C.DELIVERY_STATUS.SUCCESS) {
              error = C.DELIVERY_STATUS[packet.deliveryStatus];
            }
          }
          cb(error, packet);
        });
      }
    });
  };
}

XBee.prototype._AT = function(cmd, val, _cb) {
  // val parameter is optional
  if (typeof val === 'function') {
    _cb = val;
    val = undefined;
  }

  var frame = new api.ATCommand();
  frame.setCommand(cmd);
  frame.commandParameter = val;
  var cbid = C.FRAME_TYPE.AT_COMMAND_RESPONSE + C.EVT_SEP + frame.frameId;
  var packet = [this._makeTask({
    data: frame.getBytes(),
    cbid: cbid
  })];
  this._queue.push({ packets:packet, cb:_cb });
  return cbid;
}

// TODO: Merge this up with _AT to save some space
XBee.prototype._remoteAT = function(cmd, remote64, remote16, val, _cb) {
  // val parameter is optional
  if (typeof val === 'function') {
    _cb = val;
    val = undefined;
  }

  var frame = new api.RemoteATCommand();
  frame.setCommand(cmd);
  frame.commandParameter = val;
  frame.destination64 = remote64.dec;
  frame.destination16 = remote16.dec;
  var cbid = C.FRAME_TYPE.REMOTE_COMMAND_RESPONSE + C.EVT_SEP + frame.frameId;
  var packet = [this._makeTask({
    data: frame.getBytes(),
    cbid: cbid
  })];
  this._queue.push({ packets:packet, cb:_cb });
  return cbid;
}

XBee.prototype._handleNodeIdentification = function(node) {
  if (!this.nodes[node.remote64.hex]) {
    this.nodes[node.remote64.hex] = new Node(this, node, this.data_parser);
    this.emit("newNodeDiscovered", this.nodes[node.remote64.hex]);
  } else {
    // update 16-bit address, as it may change during reconnects.
    this.nodes[node.remote64.hex].remote16 = node.remote16;
    this.nodes[node.remote64.hex].id = node.id;
    this.nodes[node.remote64.hex].emit("discovered", this.nodes[node.remote64.hex]);
  }
  this.nodes[node.remote64.hex].connected = true;
}

XBee.prototype.init = function(cb) {
  var self = this;
  // Serial connection to the XBee
  self.serial = new serialport.SerialPort(self.options.port, {
    baudrate: self.options.baudrate || 9600,
    databits: 8,
    stopbits: 1,
    parity: 'none',
    parser: api.packetBuilder()
  });

  self.serial.on("open", function() {
    self.readParameters.bind(self)(cb);
  });

  var exit = function() { 
    self.serial.close(function(err) {
      if (err) console.log("Error closing port: "+util.inspect(err));
      process.exit();
    });
  }
  
  if (os.platform() !== 'win32') {
    process.on('SIGINT', exit);
  }


  /* Frame-specific Handlers */

  // Whenever a node reports with an identification frame.
  self._onNodeIdentification = function(packet) {
    var node = parseNodeIdentificationPayload(packet.nodeIdentificationPayload);
    self._handleNodeIdentification(node);
  }
  
  // Modem Status
  self._onModemStatus = function(packet) {
    if (res.status == C.MODEM_STATUS.JOINED_NETWORK) {
      self.emit("joinedNetwork", packet);  
    } else if (res.status == C.MODEM_STATUS.HARDWARE_RESET) {
      self.emit("hardwareReset", packet);
    } else if (res.status == C.MODEM_STATUS.WATCHDOG_RESET) {
      self.emit("watchdogReset", packet);
    } else if (res.status == C.MODEM_STATUS.DISASSOCIATED) {
      self.emit("disassociated", packet);
    } else if (res.status == C.MODEM_STATUS.COORDINATOR_STARTED) {
      self.emit("coordinatorStarted", packet);
    } else {
      console.warn("Modem status: ", C.MODEM_STATUS[res.status]);
    }
  }

  // Messages
  self._onReceivePacket = function(data) {
    if (!self.nodes[data.remote64.hex]) {
      var node = self.addNode(data.remote64.dec, data.remote16.dec, self.data_parser);
      self.emit("newNodeDiscovered", node); // TODO: Should this be a different event?
    }
    self.nodes[data.remote64.hex]._onReceivePacket(data);
  }

  // Data samples (from XBee's I/O)
  self._onDataSampleRx = function(data) {
    // Todo: data from local xbee?
    if (!self.nodes[data.remote64.hex]) {
      var node = self.addNode(data.remote64.dec, data.remote16.dec, self.data_parser);
      self.emit("newNodeDiscovered", node); // TODO: Should this be a different event?
    }
    self.nodes[data.remote64.hex]._onDataSampleRx(data);
  }

  self.serial.on(C.FRAME_TYPE.MODEM_STATUS,             self._onModemStatus);
  self.serial.on(C.FRAME_TYPE.NODE_IDENTIFICATION,      self._onNodeIdentification);
  self.serial.on(C.FRAME_TYPE.ZIGBEE_RECEIVE_PACKET,    self._onReceivePacket);
  self.serial.on(C.FRAME_TYPE.ZIGBEE_IO_DATA_SAMPLE_RX, self._onDataSampleRx);
  
  self._queue = async.queue(function(task, callback) {
    async.series(task.packets, function(err, data) {
      if (typeof task.cb === 'function') task.cb(err, data[data.length-1]);
      callback();
    });
  }, 1);

  return self;
}

XBee.prototype.readParameters = function(_done_cb) {
  var self = this;

  // Returns a function that initiates an AT command to
  // query a configuration parameter's value. 
  // To be passed to an async.parallel.
  var QF = function(command, val, f) { // Format the result using f
    f = typeof f !== 'undefined' ? f : function(a){return a};
    return function(cb) {
      self._AT(command, val, function(err, res) {
        if (!err) {
          cb(err, f(res));
        } else {
          console.error('Error: XBee.readParameters.QF; ', err.msg);
        }
      });
    }
  }

  var parameters = {
    panid:             QF('ID', undefined, Tools.bArr2HexStr),
    id:                QF('NI', undefined, Tools.bArr2Str),
    sourceHigh:        QF('SH', undefined, Tools.bArr2HexStr),
    sourceLow:         QF('SL', undefined, Tools.bArr2HexStr),
    nodeDiscoveryTime: QF('NT', undefined, function(a) { return 100 * Tools.bArr2Dec(a); })
  };
  
  var done = function(err, results) {
    if (err) {
      self.emit("error", new Error("Failure to read XBee params: "+util.inspect(err)));
      if (typeof _done_cb === 'function') _done_cb(err);
    }
    self.parameters = results;
    self.emit("initialized", self.parameters);
    if (typeof _done_cb === 'function') _done_cb(null, self.parameters);
  }

  // Using async to read parameters
  var res_stop = Object.keys(parameters).length;
  var results = {};
  for (k in parameters) {
    parameters[k]((function(key) {
      return function(err, data) {
        if (err) return done(err, null);
        results[key] = data; 
        // TODO: Timeout?
        if (--res_stop === 0) {
          done(null, results);
        }
      }
    })(k));
  }
}

// Add a node by hand.
XBee.prototype.addNode = function(remote64, remote16, parser) {
  var self = this;
  if (!remote16 instanceof Array) {
    parser = remote16;
  } else if (!remote16) {
    remote16 = [0xff,0xfe]; // Unknown
  }
  
  var node_data = {
    remote16: { dec: remote16, hex: Tools.bArr2HexStr(remote16) },
    remote64: { dec: remote64, hex: Tools.bArr2HexStr(remote64) }
  };
  
  var node = self.nodes[node_data.remote64.hex];

  if (!node) {
    node = self.nodes[node_data.remote64.hex] = new Node(self, node_data, self.data_parser);
    node.connected = false;
  }

  if (typeof parser === "function")
    node.parser = parser(node);

  return node;
}

// Run network discovery. Associated nodes can report in
// for config.nodeDiscoveryTime ms.
XBee.prototype.discover = function(cb) {
  var self = this;
  var cbid = self._AT('ND');
  self.serial.on(cbid, function(packet) {
    var node = parseNodeIdentificationPayload(packet.commandData);
    self._handleNodeIdentification(node);
  })
  setTimeout(function() {
    if (typeof cb === 'function') cb(); 
    self.removeAllListeners(cbid);
    self.emit("discoveryEnd");
  }, self.parameters.nodeDiscoveryTime || 6000);
}

XBee.prototype.broadcast = function(data, cb) {
  var remote64 = {};
  var remote16 = {};
  remote64.dec = [0x00,0x00,0x00,0x00,0x00,0x00,0xff,0xff];
  remote16.dec = [0xff,0xfe]; 
  this.send(data, remote64, remote16, cb);
}

XBee.prototype.send = function(data, remote64, remote16, _cb) {
  var packets = [];
  while (data.length > 0) {
    var frame = new api.TransmitRFData();
    frame.destination64 = remote64.dec;
    frame.destination16 = remote16.dec;
    var length = (C.MAX_PAYLOAD_SIZE < data.length) ? C.MAX_PAYLOAD_SIZE : data.length;
    frame.RFData = data.slice(0,  length);
    data = data.slice(length);
    packets.push(this._makeTask({
      data: frame.getBytes(),
      cbid: C.FRAME_TYPE.ZIGBEE_TRANSMIT_STATUS + C.EVT_SEP + frame.frameId
    }));
  }

  this._queue.push({ packets:packets, cb:_cb });
}

XBee.prototype.AT = function(cmd, val, _cb) {
  this._AT(cmd, val, _cb);
}

XBee.prototype.setChangeDetection = function(pins, cb, remote) {
  // TODO: this is lazy...
  var mask = "000000000000".split('');
  for (pin in pins) {
    var _pin = pins[pin];
    if (typeof _pin == "number") {
      mask[C.CHANGE_DETECTION.PIN[_pin]] = '1';
    } else {
      mask[C.CHANGE_DETECTION[_pin]] = '1';
    }
  }

  var val = parseInt(mask.reverse().join(''), 2);
      val = Tools.dec2bArr(val, 2);

  if (remote) 
    this._remoteAT("IC", remote.remote64, remote.remote16, val, cb);
  else
    this._AT("IC", val, cb);
}

XBee.prototype.setSampleInterval = function(interval, cb, remote) {
  var _interval = Tools.dec2bArr(interval, 2);
  if (remote) 
    this._remoteAT("IR", remote.remote64, remote.remote16, _interval, cb);
  else
    this._AT("IR", _interval, cb);
}

XBee.prototype.getSample = function(cb, remote) {
  var _cb = function(err, res) {
    if (err) cb(err);
    else cb(err, parseIOSample(res));
  }

  if (remote) 
    this._remoteAT("IS", remote.remote64, remote.remote16, _cb);
  else
    this._AT("IS", _cb);
}


// Change the mode of a pin.
// If "pin" is a string,
//   we assume the descriptive name (AO3, etc.)
// If "pin" is a number
//   we assume the phsical pin is meant (~1-20)
XBee.prototype.getAnalogPin = function(pin, cb, remote) {
  var _pin;
  if (typeof pin == "number") 
    _pin = C.ANALOG_CHANNEL.MASK[C.ANALOG_CHANNEL.PIN[pin]][0]
  else _pin = pin;

  this.getSample(function(err, res) {
    if (err) cb(err); 
    else if (res.analogSamples[_pin] == undefined)
      cb(new Error("XBee not configured to take analog samples on pin "+_pin));
    else
      cb(err, res.analogSamples[_pin]);
  }, remote);
}

// Change the mode of a pin.
// If "pin" is a string,
//   we assume the descriptive name (DIO3, etc.)
// If "pin" is a number
//   we assume the phsical pin is meant (~1-20)
XBee.prototype.getDigitalPin = function(pin, cb, remote) {
  var _pin;
  if (typeof pin == "number") 
    _pin = C.DIGITAL_CHANNEL.MASK[C.DIGITAL_CHANNEL.PIN[pin]][0]
  else _pin = pin;

  this.getSample(function(err, res) {
    if (err) cb(err); 
    else if (res.digitalSamples[_pin] == undefined)
      cb(new Error("XBee not configured to take digital samples on pin "+_pin));
    else
      cb(err, res.digitalSamples[_pin]);
  }, remote);
}

XBee.prototype.getPinMode = function(pin, mode, cb, remote) {
  var _pin;
  if (typeof pin == "number") _pin = C.PIN_COMMAND.PIN[pin];
  else if (typeof pin == "string" && pin.length != 2) _pin = C.PIN_COMMAND[pin];
  else _pin = pin;
  if (_pin == undefined)
    return cb(new Error("Unknown pin: "+pin));

  var _cb = function(err, res) {
    if (err) cb(err);
    else cb(err, res[0]); // Or should we return the name of the mode?
  };

  if (remote) 
    this._remoteAT(_pin, remote.remote64, remote.remote16, _cb);
  else
    this._AT(_pin, _cb);
}

// Change the mode of a pin.
// If "pin" is not the two-letter AT command associated with the pin,
//   we assume the descriptive name (DIO3, etc.)
// If "pin" is a number
//   we assume the phsical pin is meant (~1-20)
// If "mode" is a number
//   we assume it is the byte parameter
// If "mode" is a string
//   we assume descriptive mode (DISABLED, DIGITAL_INPUT, ...) see constants.js 
XBee.prototype.setPinMode = function(pin, mode, cb, remote) {
  var _pin, _mode;
  if (typeof pin == "number") _pin = C.PIN_COMMAND.PIN[pin];
  else if (typeof pin == "string" && pin.length != 2) _pin = C.PIN_COMMAND[pin];
  else _pin = pin;
  if (_pin == undefined || C.PIN_MODE[_pin] == undefined)
    return cb(new Error("Unknown pin: "+pin));
  if (typeof mode == "string") {
    _mode = C.PIN_MODE[_pin][mode];
  } else _mode = mode;
  if (_mode == undefined || C.PIN_MODE[_pin][mode] == undefined)
    return cb(new Error("Unknown mode "+mode+" for pin "+pin));
  _mode = parseInt(_mode); // Make sure mode is dec

  if (remote) 
    this._remoteAT(_pin, remote.remote64, remote.remote16, [_mode], cb);
  else
    this._AT(_pin, [_mode], cb);
}

XBee.prototype.setDestinationNode = function(dest, cb, remote) {
  cb("Not Implemented Yet");
}

exports.XBee = XBee;



function Node(xbee, params, data_parser) {
  EventEmitter.call(this);
  this.xbee = xbee;
  this.remote16 = params.remote16;
  this.remote64 = params.remote64;
  this.id = params.id || "";
  this.deviceType = params.deviceType || -1;
  this.buffer = "";
  if (typeof data_parser === 'function')
    this.parser = data_parser(this);
  this.timeout = {};
  this.connected = true;
  this.refreshTimeout();
}

util.inherits(Node, EventEmitter);

Node.prototype._onReceivePacket = function(packet) {
  // TODO: should be buffer all along!
  var data = new Buffer(packet.rawData).toString('ascii');
  if (this.xbee.use_heartbeat) {
    this.refreshTimeout();
    if (data === this.xbee.heartbeat_packet) return;
  }

  if  (this.parser !== undefined) this.parser.parse(data);
  else this.emit('data', data, packet);
}

Node.prototype._onDataSampleRx = function(packet) {
  var sample = parseIOSample(packet.ioSample);
  this.emit('io', sample, packet);  
  if (this.xbee.use_heartbeat) {
    this.refreshTimeout();
  }
}

Node.prototype.timeoutOccured = function() {
  this.connected = false;
  this.emit('disconnect');
}

Node.prototype.refreshTimeout = function() {
  clearTimeout(this.timeout);
  this.timeout = setTimeout(this.timeoutOccured.bind(this), this.xbee.heartbeat_timeout);
  if (!this.connected) {
    this.connected = true;
    // todo other stuff
  }
}

Node.prototype.send = function(data, cb) {
  this.xbee.send(data, this.remote64, this.remote16, cb);
}

Node.prototype.AT = function(cmd, val, cb) {
  // val parameter is optional
  if (typeof val === "function") {
    // use val as the callback in this case
    this.xbee._remoteAT(cmd, this.remote64, this.remote16, val);
  } else {
    this.xbee._remoteAT(cmd, this.remote64, this.remote16, val, cb);
  }
}

Node.prototype.isCoordinator = function() {
  return this.deviceType === C.DEVICE_TYPES.COORDINATOR;
}

Node.prototype.isRouter = function() {
  return this.deviceType === C.DEVICE_TYPES.ROUTER;
}

Node.prototype.isEndDevice = function() {
  return this.deviceType === C.DEVICE_TYPES.END_DEVICE
}

Node.prototype.setChangeDetection = function(pins, cb) {
  this.xbee.setChangeDetection(pins, cb, this);
}

Node.prototype.setSampleInterval = function(interval, cb) {
  this.xbee.setSampleInterval(interval, cb, this);
}

Node.prototype.getSample = function(cb) {
  this.xbee.getSample(cb, this);
};

Node.prototype.setPinMode = function(pin, mode, cb) {
  this.xbee.setPinMode(pin, mode, cb, this);
}

Node.prototype.getAnalogPin = function(pin, cb) {
  this.xbee.getAnalogPin(pin, cb, this);
}

Node.prototype.getDigitalPin = function(pin, cb) {
  this.xbee.getDigitalPin(pin, cb, this);
}

Node.prototype.setDestinationNode = function(dest, cb) {
  this.xbee.setDestinationNode(dest, cb);
}

exports.Node = Node;

var parseIOSample = exports.parseIOSample = function(sample) {
  var res = {
    digitalSamples: {},
    analogSamples: {}
  }

  var numSamples = sample.splice(0,1)[0];
  var mskD = sample.splice(0,2);
      mskD = (mskD[0] << 8) | mskD[1];
  var mskA = sample.splice(0,1)[0];

  if (mskD > 0) {
    var digitalSamples = sample.splice(0,2);
    var valD = (digitalSamples[0] << 8) | digitalSamples[1];
    for (var bit in C.DIGITAL_CHANNELS.MASK) {
      if ((mskD & (1 << bit)) >> bit) {
        res.digitalSamples[C.DIGITAL_CHANNELS.MASK[bit][0]] = (valD & (1 << bit)) >> bit;
      }
    }
  }

  if (mskA > 0) {
    var analogSamples = sample.splice(0);
    var sampleNr = 0;
    for (var bit in C.ANALOG_CHANNELS.MASK) {
      if ((mskA & (1 << bit)) >> bit) {
        var valA = (analogSamples[sampleNr*2] << 8) | analogSamples[sampleNr*2+1];
        // Convert to mV
        res.analogSamples[C.ANALOG_CHANNELS.MASK[bit][0]] = (valA * 1200) / 1023;
        sampleNr++;
      }
    }
  }

  return res;
}

var parseAddress = exports.parseAddress = function(dec) {
  return {
    dec: dec,
    hex: Tools.bArr2HexStr(dec)
  }
}

var parseNodeIdentificationPayload = exports.parseNodeIdentificationPayload = function(payload) {
  var res = {}
  res.id = "";

  res.remote16 = parseAddress(payload.splice(0, 2));
  res.remote64 = parseAddress(payload.splice(0, 8)); 
  while(payload[0] != 0x00) res.id += String.fromCharCode(payload.splice(0,1)[0]);
  payload.splice(0,1); // Read 0x00 away 
  res.remoteParent16 = parseAddress(payload.splice(0, 2)); 
  res.deviceType = payload.splice(0, 1)[0];
  res.sourceEvent = payload.splice(0, 1)[0];
  res.nodeIdentificationPayload = payload.splice(0);
  // res.status = payload.splice(0, 1)[0];
  // ...

  return res;
}
