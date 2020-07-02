var express = require('express');
var app = express();
var bodyParser = require('body-parser')
const fs = require('fs');
const path = require('path');
const multer = require('multer')
var _ = require('./common/lodash.min.js')
var he = require('he') // unused I think
var elasticlunr = require('./common/elasticlunr.js')
const { Transform } = require('stream');

var sys = require('util')
var exec = require('child_process').exec;

const SerialPort = require('serialport');

// CONSTANTS

const DATA_PATH = path.join(__dirname, 'data');

// FUNCTIONS

//app.use(bodyParser.urlencoded({ extended: false }))
app.use(bodyParser.urlencoded({ extended: true }))

app.use(function(req, res, next) {
  res.header("Access-Control-Allow-Origin", "http://localhost:3000");
  res.header("Access-Control-Allow-Headers", "Origin, X-Requested-With, Content-Type, Accept");
  res.header('Access-Control-Allow-Methods', 'PUT, POST, GET, DELETE, OPTIONS');
  next();
});

//app.use(express.static("private"));

// TODO: read this
// https://blog.soshace.com/en/programming-en/node-lessons-safe-way-to-file/

app.get('/capture.jpg',function (req, res) {
  res.sendFile(path.join(__dirname, req.path));
})

app.get('/common/*',function (req, res) {
  res.sendFile(path.join(__dirname, req.path));
})

app.get('/icon/*',function (req, res) {
  res.sendFile(path.join(__dirname, req.path));
})

//app.get('/private/*',function (req, res) {
//  res.sendFile(path.join(__dirname, req.path));
//})

app.get('/images/*',function (req, res) {
  res.sendFile(path.join(__dirname, req.path));
})

const CODE_MOVE = "M" // () changes mode to move (maybe useless)
const CODE_GRAB = "G" // () close the claw until pressure switch is closed
const CODE_RELEASE = "R" // () open the claw to position 0
const CODE_X = "X" // (mm) absolute position to move to
const CODE_Y = "Y" // (mm) absolute position to move to
const CODE_Z = "Z" // (mm) absolute position to move to
const CODE_ADD = "A" // (qty) tilts until weight lost is equal to qty
const CODE_END = "\n" // () do the command, separates all the small commands
const CODE_TERMINATE = ";" // () send the command

var pince = {
  x: 0,
  y: 0,
  z: 0,
}

var CUISEUR = 1;

var INGREDIENTS_POSITION = {
  riz: {x: 50, y: 100}
}

function grabIngredient(ingredient) {
  if (!INGREDIENTS_POSITION[ingredient]) {throw "Unkown ingredient " + ingredient}
  return retract() + CODE_MOVE + CODE_X + INGREDIENTS_POSITION[ingredient].x + CODE_Y + INGREDIENTS_POSITION[ingredient].y + CODE_END + grab()
}

function grab() {
  return CODE_MOVE + CODE_Z + "500" + CODE_END
}

function removeCap() {
}

function moveToContainer() {
}

function add() {

}

function retract() {
  return pince.z <= 0 ? "" : "G00Z0\n"
}

app.get('/run/arduino',function (req, res) {
  // TODO: write to serial directly
  let fn = (query) => (`echo ${query.cmd} > /dev/ttyACM0`)
  console.log('Running command arduino: ' + req.query.cmd)
  const toExe = fn(req.query)
  exec(toExe, function(err, stdout, stderr) {
    console.log(err)
    console.log(stderr)
    console.log(stdout);
    res.set({ 'content-type': 'text/plain; charset=utf-8' });
    res.send(stdout)
  });
})

app.post('/run/recette',function (req, res) {
  console.log('HERE! About to run command: ' + req.params.command)
  var gcode = ""
  var selectedContainerType = 0;
  var selectedContainer = 0;
  for (var i = 0; i < req.body.in.length; i++) {
    const cmd = req.body.in[i]
    if (cmd === "Cuiseur") {
      selectedContainerType = CUISEUR;
      selectedContainer = req.body.in[i+1];
      i++;
    } else if (cmd === "Ajouter") {
      const qty = req.body.in[i+1];
      const ingredient = req.body.in[i+2];
      gcode += grabIngredient(ingredient)
      gcode += retract()
      // Grab the ingredient
      // Move to the selected container
      // Drop the required amount into the container
      // Retract
    } else if (cmd === "Verser") {
    } else if (cmd === "Melanger") {
    } else if (cmd === "Cuire") {
    } else {
      console.log('Unkown command: ' + cmd)
      res.end('Unkown command: ' + cmd)
      return false;
    }
  }
  gcode += CODE_TERMINATE
  console.log(req)
})

// ************ ARDUINO ********************
var arduinoLog = {}
var arduinoInfo = {}
var arduinoInfoChanged = false

const Readline = require('@serialport/parser-readline');
const port = new SerialPort('/dev/ttyACM0', { baudRate: 9600, autoOpen: false });
const parser = port.pipe(new Readline({ delimiter: '\n' }));
port.on("open", () => {
  arduinoLog = {}
  console.log('serial port open');
});
parser.on('data', data =>{
  const timestamp = Date.now()
  const str = data.toString()
  if (str[0] === '-') {
    const splited = str.split(':')
    const key = splited[0].trim().substr(1).trim()
    const value = splited[1].trim()
    arduinoInfo[key] = value
    arduinoInfoChanged = true
  } else {
    arduinoLog[timestamp.toString()] = str
  }
  console.log('got word from arduino:', str);
});

function nocache(req, res, next) {
  res.header('Cache-Control', 'private, no-cache, no-store, must-revalidate');
  res.header('Expires', '-1');
  res.header('Pragma', 'no-cache');
  next();
}

app.get('/closeArduino', function(req, res) {
  port.close(function (err) {
    console.log(`${new Date().toUTCString()} - Port closed.`, err);
  })
  res.end()
})

app.get('/takePicture', function(req, res) {
	
  exec('fswebcam --no-banner -r 640x480 -i 0 -F 2 --flip v capture.jpg', function(err, stdout, stderr) {
    console.log(err)
    console.log(stderr)
    console.log(stdout);
    res.set({ 'content-type': 'text/plain; charset=utf-8' });
    res.send(stdout)
  });
})

app.get('/reloadArduino', function(req, res) {
  console.log('GET path=' + req.path);
  console.log('*** reloadingArduino ***')
  port.close(function (err) {
    console.log(`${new Date().toUTCString()} - Port closed.`, err);
    port.open(function (err) {
      console.log(`${new Date().toUTCString()} - Port opened.`, err);
      if (err) {
        return console.log('Error opening port: ', err.message)
      }
  
      // Because there's no callback to write, write errors will be emitted on the port:
      //port.write('main screen turn on')
    })
  })
  res.end()
})

app.get('/poll/arduino', nocache, function(req, res) {
  console.log('poll/arduino')
  //res.set('Content-Type', 'application/json');
  //res.writeHead(200, { 'Content-Type': 'application/json' });
  const keys = Object.keys(arduinoLog).slice()
  let log = ""
  keys.forEach((key) => {
    log = log + arduinoLog[key] + "\n"
    delete arduinoLog[key]
  })
  //res.status(200).send({log, info: (arduinoInfoChanged ? arduinoInfo : null)})
  res.status(200).json({log, info: (arduinoInfoChanged ? arduinoInfo : null)})

  arduinoInfoChanged = false
  //res.end();
})
// *****************************************

app.get('/', function(req, res) {
  res.sendFile(path.join(__dirname, 'data/Ma machine.html'));
})

app.get('*',function (req, res) {
  console.log('GET * path=' + req.path);
  res.writeHead(404, 'Not Found');
  res.end();
});

var server_address = process.argv[2] || 'localhost'
var portnb = process.argv[3] || '3000'

if (server_address === 'local' || server_address === 'lan') {
  var ip_address = null
  Object.values(require('os').networkInterfaces()).forEach(function (ifaces) {
    const iface = ifaces[0]
    // skip over internal (i.e. 127.0.0.1) and non-ipv4 addresses
    if (!('IPv4' !== iface.family || iface.internal !== false)) {
      ip_address = iface.address
    }
  })
  server_address = ip_address
}

app.listen(portnb, server_address);

console.log('Listening on ' + server_address + ' port ' + portnb)
