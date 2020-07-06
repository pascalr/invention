var Stream = require('stream');
var express = require('express');
var app = express();
var bodyParser = require('body-parser')
const fs = require('fs');
const path = require('path');

var sys = require('util')
const { exec, spawn } = require('child_process');

const SerialPort = require('serialport');

var nodeCleanup = require('node-cleanup')
var sweep
var fake

var isPortOpen = false

function log(msg, err) {
  if (err) {
    console.log(`${new Date().toUTCString()} - ${err}`);
  } else {
    console.log(`${new Date().toUTCString()} - ${msg}`);
  }
}

app.use(bodyParser.urlencoded({ extended: true }))

app.use(function(req, res, next) {
  res.header("Access-Control-Allow-Origin", "http://localhost:3000");
  res.header("Access-Control-Allow-Headers", "Origin, X-Requested-With, Content-Type, Accept");
  res.header('Access-Control-Allow-Methods', 'PUT, POST, GET, DELETE, OPTIONS');
  next();
});

// TODO: read this
// https://blog.soshace.com/en/programming-en/node-lessons-safe-way-to-file/

app.get('/capture.jpg',function (req, res) {
  res.sendFile(path.join(__dirname, req.path));
})

//app.use(express.static("public"));
app.get('/public/*',function (req, res) {
  res.sendFile(path.join(__dirname, req.path));
})

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
  log('Running command arduino: ' + req.query.cmd)
  arduino_or_fake.write(req.query.cmd+"\n")
  res.end()
})

function pipe(source, dest) {
  source.on('data', function(data) {
    dest.write(data)
  })
}

app.get('/sweep',function (req, res) {

  log('Spawning sweep')
  sweep = spawn('../bin/sweep', {stdio: 'inherit'})

  //pipe(arduino_or_fake, sweep.stdin)
  //pipe(sweep.stdout, arduino_or_fake)

  //sweep.stdin.on('error', function( err ) {
  //  if (err.code == "EPIPE") {
  //    log("Unable to send data to sweep, already closed..")
  //  }
  //});

  //sweep.stderr.on('data', (data) => {
  //  log("Sweep err " + data);
  //});
  
  //sweep.on('close', (code) => {
  //  log("sweep closed. FIXME: PUT BACK WHAT IT USED TO BE IN ON CODE");
  //  arduino_or_fake.on('data', defaultParserDataVal)
  //  res.end()
  //});

  //sweep.on('error', (err) => {
  //  console.error('Failed to start sweep.');
  //});

})

app.post('/run/recette',function (req, res) {
  log('HERE! About to run command: ' + req.params.command)
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
      log('Unkown command: ' + cmd)
      res.end('Unkown command: ' + cmd)
      return false;
    }
  }
  gcode += CODE_TERMINATE
  log(req)
})

// ************ ARDUINO ********************
var arduinoLog = {}
var arduinoInfo = {}
var arduinoInfoChanged = false

const Readline = require('@serialport/parser-readline');
const port = new SerialPort('/dev/ttyACM0', { baudRate: 115200, autoOpen: false });
const parser = port.pipe(new Readline({ delimiter: '\n' }));
port.on("open", () => {
  isPortOpen = true
  arduinoLog = {}
  log('serial port opened');
});
port.on("close", () => {
  isPortOpen = false
  arduinoLog = {}
  log('serial port closed');
});
var defaultParserDataVal = (data) => {
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
  log('got word from arduino:', str);
}
parser.on('data', defaultParserDataVal);

function nocache(req, res, next) {
  res.header('Cache-Control', 'private, no-cache, no-store, must-revalidate');
  res.header('Expires', '-1');
  res.header('Pragma', 'no-cache');
  next();
}

app.get('/close', function(req, res) {
  arduino_or_fake.close(function (err) {
    log("Port closed", err);
  })
  res.end()
})

app.get('/takePicture', function(req, res) {
	
  exec('fswebcam --no-banner -r 640x480 -i 0 -F 2 --flip v capture.jpg', function(err, stdout, stderr) {
    log(err)
    log(stderr)
    log(stdout);
    res.set({ 'content-type': 'text/plain; charset=utf-8' });
    res.send(stdout)
  });
})

function openCallback(err) {
  log('Port opened', err)
}

app.get('/connect', function(req, res) {
  log('GET path=' + req.path);
  if (arduino_or_fake.isOpen()) {
    log('Reloading connection')
    arduino_or_fake.close(function (err) {
      log('Port closed')
      arduino_or_fake.open(openCallback)
    })
  } else {
    arduino_or_fake.open(openCallback)
  }
  
  res.end()
})

app.get('/poll/arduino', nocache, function(req, res) {
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
  res.sendFile(path.join(__dirname, 'index.html'));
})

app.get('*',function (req, res) {
  log('GET * path=' + req.path);
  res.writeHead(404, 'Not Found');
  res.end();
});

var server_address = process.argv[2] || 'localhost'
var portnb = process.argv[3] || '3001'

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


// Could simply return directly the port when isPortOpen, but this way I have more control
const arduino_or_fake = {
  write: (data) => {
    if (isPortOpen) {
      port.write(data);
    } else if (!fake) {
      log("Error: fake is not created yet...")
    } else {
      fake.stdin.write(data)
    }
  },

  on: (str, func) => {
    if (isPortOpen) {
      //port.on(str,func);
      parser.on(str,func);
    } else if (!fake) {
      log("Error: fake is not created yet...")
    } else {
      fake.stdout.on(str,func);
    }
  },

  isOpen: () => {
    return isPortOpen || (fake && !fake.exitCode)
  },

  open: (callback) => {
    port.open(function(err) {
      if (err) {
        log("Error opening arduino port. Using fake arduino instead.");
        fake = spawn('../bin/fake')
        callback(err)
      }
      callback(err)
    })
  },

  close: (callback) => {
    if (isPortOpen) {
      port.close(callback)
    } else {
      if (fake && !fake.exitCode) {
        fake.kill('SIGINT');
        callback()
      }
    }
  }


}

nodeCleanup(function (exitCode, signal) {
  if (sweep) {
    sweep.kill('SIGINT');
  }
  if (fake) {
    fake.kill('SIGINT');
  }
})

app.listen(portnb, server_address);

log('Listening on ' + server_address + ' port ' + portnb)
