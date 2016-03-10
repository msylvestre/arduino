var util = require('util');
var XBee = require('../../index.js');

// This parser buffers data, emits chucks
// seperated by space chars (" ")
var Parser = require('./parser.js');

var xbee = new XBee.XBee({
  port: 'COM3',   // replace with yours
  baudrate: 9600 // 9600 is default
})

// Open COM port, read some parameters from the XBee at once.
xbee.init();

// Add Node by hand...
var myNode = xbee.addNode([0x00,0x13,0xa2,0x00,0x40,0x61,0x2f,0xe4], Parser);

// Triggered whenever this node
//   - responds to discovery request
//   - sends identification frame on network association
//      (not necessarily on power up, as node might
//       already be associated)
myNode.on("discovered", function(node) {
  console.log("Discovered myNode");

  // We configure pin D4 to sample digital data.
  myNode.AT("D4", [ 0x03 ], function(err, res, x) {
      if (err) return console.log("AT Error D4:", err);

    // This configures pin 2 to sample analog data.
    myNode.AT("D2", [ 0x02 ], function(err, res, x) {
      if (err) return console.log("AT Error D2:", err);

      // We can put requests in the callback, but generally
      // but generally parallel requests are fine!
      myNode.AT("IS", function(err, res) { // Manually query IO Sample
        if (err) return console.log("AT Error IS:", err);
        console.log("Queried IO Sample: ", util.inspect(xbee.tools.parseIOSample(res)));
      });
    });

  });
});

// Whenever myNode sends us an IO data sample...
myNode.on("io", function(io) {
  console.log("I/O: ", util.inspect(io));
});

// Whenever myNode sends us text data - since we are using
// a parser, data will first be processed there before landing
// here:
myNode.on("data", function(data) {
  console.log("Data: ", util.inspect(data));
  if (data == "ping") myNode.send("pong");
});


// Emitted when .init() is done (COM port open, parameters read)
xbee.on("initialized", function(params) {
  console.log("XBee Parameters: %s", util.inspect(params));
  // Start Node discovery to find currently connected nodes.
  xbee.discover(); 
  console.log("Node discovery starded...");
  
  // Local Request:
  xbee.AT("VR", function(err, res) {
    console.log("Firmware Version:", xbee.tools.bArr2HexStr(res));
  });
  

  // Remote Requests ... 
  // Enable auto reporting of IO Samples with a 2000ms interval set
  // Note that this request will most typically fail. 
  // Try uncommenting the line "xbee.discover()", and this should
  // work fine (if your network is configured properly).
  // The reason is that remote nodes seem to become unresponsive for
  // a little while during node discovery.
  myNode.AT("IR", xbee.tools.dec2bArr(2000), function(err, res) { 
    if (err) return console.log("AT Error:", util.inspect(err));
    console.log("Auto Reporting Enabled!");
  });
});

xbee.on("discoveryEnd", function() {
  // Discovery is over. If the XBee is an End Device/Router, 
  // you may want to re-issue xbee.discover() later. 
  // For Coordinators this should not be necessary, as
  // nodes will notify coordinators once they join the PAN,
  // triggering the "node" event.
  console.log("...node discovery over");
});

// Triggered whenever a node is discovered that is not already
// added. / myNode will not show up here!
xbee.on("newNodeDiscovered", function(node) {
  console.log("Node %s discovered", node.remote64.hex);
  console.log(util.inspect(node));

  node.on("data", function(data) {
    console.log("%s> %s", node.remote64.hex, util.inspect(data)); 
    node.send("pong", function(err, status) {
      // Transmission successful if err is null
    });
  });
  
  node.on("io", function(sample) {
    console.log("%s> %s", node.remote64.hex, util.inspect(data)); 
  });

  // Here some functions you might find helpful:
  node.setPinMode("DIO2", "DIGITAL_INPUT");
  node.setPinMode("DIO3", "DIGITAL_INPUT");
  node.setPinMode("DIO0", "DIGITAL_OUTPUT_HIGH");

  node.getPinMode("DIO0", function(err, res) {
    console.log("Pin DIO0 has mode: ", res);
  });

  // XBee will send a sample whenever one of these pins measure a change
  node.setChangeDetection([ "DIO2", "DIO3" ]);

  // Manually retrieve a sample
  node.getSample(function(err, res) {
    console.log("Res2:", util.inspect(res));
  });
  
  // ...or instruct xbee to sample data every 5 seconds:
  node.setSampleInterval(2000);
});
