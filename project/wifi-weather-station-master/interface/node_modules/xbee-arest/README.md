## NOTE!
Check out [xbee-api](http://github.com/jouz/xbee-api). It separates out the API component and is aimed at being a more solid and tested module for working with XBees. New higher level modules based on [xbee-api](http://github.com/jouz/xbee-api) are in development here: [xbee-stream](http://github.com/jouz/xbee-stream) and here [xbee-nodes](http://github.com/jouz/xbee-nodes).

# SVD-XBEE

[Digi's xbee modules](http://www.digi.com/xbee) are good for quickly building low power wireless networks. They can be used to send/receive text data from serial ports of different devices. XBees can also be used alone for their on board digital and analog I/O capabilities.

**svd-xbee** is a high level, actively maintained fork of Richard Morrison's [node-xbee](http://github.com/mozz100/node-xbee). It talks the ZigBee API with XBee radio modules over serial connections and provides high level abstractions of the XBee's functionality.

### Nutshell
```javascript
var XBee = require('svd-xbee');

var xbee = new XBee({
  port: 'COM3',   // replace with yours
  baudrate: 9600 // 9600 is default
}).init();

var robot = xbee.addNode([0x00,0x13,0xa2,0x00,0x40,0x61,0xaa,0xe2]);

var robot.on("data", function(data) {
    console.log("robot>", data);
    if (data == "ping") robot.send("pong");
});
```
### Features

- Asynchronous architecture
- Event-based Node Discovery
- Local and remote AT commands
- High-level abstraction of common tasks
- Parse API frames into meaningful objects
    - Regular text data
    - Analog & Digital I/O Samples
    - Modem Status
    - Transmission Reports

### Installation

    npm install svd-xbee

### Documentation

For documentation, see the [Documentation](https://github.com/jouz/svd-xbee/wiki/Documentation).

### EXAMPLES

See the [examples folder](https://github.com/jouz/svd-xbee/tree/master/examples) in the repository for more examples.

## SUPPORTED XBEE MODELS

Both ZNet 2.5 and ZIGBEE modules should be supported. Since ZIGBEE offers more features and is more robust, you might be interested in upgrading your modules from ZNet 2.5 to ZIGBEE: [upgradingfromznettozb.pdf](ftp://ftp1.digi.com/support/documentation/upgradingfromznettozb.pdf).  
Development is done using Series 2 XBee modules with XB24-ZB (ZIGBEE) firmware. In specific, this document is used as reference: [90000976_M.pdf](http://ftp1.digi.com/support/documentation/90000976_M.pdf "http://ftp1.digi.com/support/documentation/90000976_M.pdf").


## MODULE CONFIGURATION

The module communicating with svd-xbee must be set to use an API function set with escape characters enabled (ATAP = 2). Other nodes in the network can be configured however you find it convenient. See [Module Configuration](https://github.com/jouz/svd-xbee/wiki/Module-Configurationi) for more details.


## ACKNOWLEDGMENTS

* voodootikigod's [serialport module](https://github.com/voodootikigod/node-serialport) (in fact you're going to need this to use this package)
* "[Building Wireless Sensor Networks](http://shop.oreilly.com/product/9780596807740.do)" by Rob Faludi


## LICENSE


> This work by <span xmlns:cc="http://creativecommons.org/ns#" property="cc:attributionName">Jan Kolkmeier</span> is licensed under a <a rel="license" href="http://creativecommons.org/licenses/by-sa/2.0/uk/">Creative Commons Attribution-ShareAlike 2.0 UK: England &amp; Wales License</a>.<br /><a rel="license" href="http://creativecommons.org/licenses/by-sa/2.0/uk/"><img alt="Creative Commons License" style="border-width:0" src="http://i.creativecommons.org/l/by-sa/2.0/uk/88x31.png" /></a><br />Based on a work at <a xmlns:dct="http://purl.org/dc/terms/" href="https://github.com/mozz100/node-xbee" rel="dct:source">github.com</a>.
