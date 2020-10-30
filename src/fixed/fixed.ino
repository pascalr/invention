#include "hx711.h"
#include "arduino_lib.h"

// https://github.com/aguegu/ardulibs/tree/master/hx711 

// Hx711.DOUT - pin #A1
// Hx711.SCK - pin #A0
Hx711 scale(A1, A0);

void setup() {

  Serial.begin(115200);
  Serial.println("Setup...");
  Serial.println("fixed");

  //pinMode(LED_BUILTIN, OUTPUT);

  calibrateEmpty();

  Serial.println("ready");

}
  
ArduinoReader reader;
ArduinoWriter writer;

long offset;

void calibrateEmpty() {
  offset = scale.averageValue();
  scale.setOffset(offset);
}

void calibrateWithWeight(double weight) {
  float ratio = (scale.averageValue() - offset) / weight;
  Serial.println(ratio);
  scale.setScale(ratio);
}

#define BUF_SIZE 52
char buf[BUF_SIZE];

void loop() {

  if (reader.inputAvailable()) {

    getInputLine(reader, buf, BUF_SIZE);
    char cmd = buf[0];

    // ignore newline characters
    if (cmd == '\r' || cmd == '\n') return;

    Serial.print("Received: ");
    Serial.println(cmd);

    // get weight
    if (cmd == 'w') {
      Serial.println(scale.getGram(), 1); // Print the gram value with one decimal precision
      
    } else if (cmd == 'c') {
      double nb;
      char* input = buf; input++;
      if (parseNumber(&input, nb) < 0) {
        Serial.println("error");
        Serial.println("Invalid number given.");
        return;
      }
      Serial.println(nb);
      calibrateWithWeight(nb);
    
    } else if (cmd == '#') { // Print the version
      Serial.println("fixed");

    } else if (cmd == 'e') {
      calibrateEmpty();
    
    } else {
      Serial.println("error");
      Serial.println("Unkown command");
      return;
    }
    Serial.println("done");
  }

  delay(100); // OPTIMIZE: Probably useless since it is busy wait I believe

}
