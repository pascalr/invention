const SerialPort = require('serialport');

var express = require('express');
var app = express();

const Readline = require('@serialport/parser-readline');
const port = new SerialPort('/dev/ttyACM0', { baudRate: 9600 });
const parser = port.pipe(new Readline({ delimiter: '\n' }));
port.on("open", () => {
  console.log('serial port open');
});
parser.on('data', data =>{
  console.log('got word from arduino:', data);
});

var portnb = 3003

app.listen(portnb);

console.log('Listening on http://localhost:' + portnb)
