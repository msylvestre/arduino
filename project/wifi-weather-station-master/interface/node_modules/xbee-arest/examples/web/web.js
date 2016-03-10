var util = require('util');
var XBee = require('../../index.js').XBee;
var app = require('http').createServer(handler)
  , io = require('socket.io').listen(app)
  , fs = require('fs')

app.listen(80);

function handler(req, res) {
  fs.readFile(__dirname + '/index.html',
  function (err, data) {
    if (err) {
      res.writeHead(500);
      return res.end('Error loading index.html');
    }

    res.writeHead(200);
    res.end(data);
  });
}

io.sockets.on('connection', function (socket) {
});

var xbee = new XBee({
  port: 'COM3',   // replace with yours
  baudrate: 9600 // 9600 is default
});

// Add Node by hand...
var myNode = xbee.addNode([0x00,0x13,0xa2,0x00,0x40,0x61,0x2f,0xe4]);

xbee.on("initialized", function(params) {
  console.log("XBee Parameters: %s", util.inspect(params));
  xbee.discover(); 
  console.log("Node discovery starded...");
});

xbee.on("discoveryEnd", function() {
  console.log("...node discovery over");
});

xbee.init();

myNode.on("io", function(sample) {
  console.log("I/O:", io);
  io.sockets.emit("io", sample);
});

myNode.on("data", function(data) {
  console.log("Data:", data);
  io.sockets.emit("data", data);
});

myNode.on("discovered", function(node) {
  console.log("myNode Discovered.");


  // Enable Auto Reporting every 2 seconds
  myNode.AT("IR", xbee.tools.dec2bArr(2000), function(err, res) { 
    if (err) return console.log("AT Error:", util.inspect(err));
    console.log("Auto Reporting Enabled.");
  });

  // Configure pin D4 to sample digital data.
  myNode.AT("D4", [ 0x03 ], function(err, res) {
    if (err) return console.log("AT Error:", err);
    console.log("D4 configured.");
  });

  // Configures pin D2 to sample analog data.
  myNode.AT("D2", [ 0x02 ], function(err, res) {
    if (err) return console.log("AT Error:", err);
    console.log("D2 configured.");
  });
});

io.sockets.on('connection', function(socket) {
  socket.on("msg", function(data) {
    myNode.send(data);
  });
});

// Triggered whenever a node is discovered that is not already
xbee.on("newNodeDiscovered", function(node) {
  console.log("Node %s discovered.", node.remote64.hex);
});
