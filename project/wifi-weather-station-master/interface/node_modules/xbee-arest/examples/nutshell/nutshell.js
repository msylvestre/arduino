var util = require('util');
var XBee = require('../../index.js').XBee;

var xbee = new XBee({
  port: 'COM5',   // replace with yours
  baudrate: 9600 // 9600 is default
}).init();

xbee.on("initialized", function(params) {
  console.log("XBee Parameters: %s", util.inspect(params));

  xbee.discover(); 
  console.log("Node discovery starded...");
});

xbee.on("discoveryEnd", function() {
  console.log("...node discovery over");
});


// Add Set to your remote node
var robot = xbee.addNode([0x00,0x13,0xa2,0x00,0x40,0x61,0x2f,0xe4]);

robot.on("data", function(data) {
  console.log("robot>", data);
  if (data == "ping") robot.send("pong");
});
