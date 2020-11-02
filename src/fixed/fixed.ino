#include "hx711.h"
#include "arduino_lib.h"

// https://github.com/aguegu/ardulibs/tree/master/hx711 

// Hx711.DOUT - pin #A1
// Hx711.SCK - pin #A0
Hx711 scale(A1, A0);

void setup() {

  Serial.begin(115200);

  //pinMode(LED_BUILTIN, OUTPUT);

  calibrateEmpty();
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
  Serial.print("ratio: ");
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

    Serial.print("echo: ");
    Serial.println(cmd);
    
    double nb;
    char* input = buf; input++;

    // get weight
    if (cmd == 'w') {
      Serial.print("weight: ");
      Serial.println(scale.getGram(), 1); // Print the gram value with one decimal precision
      
    } else if (cmd == 'c') {
      if (parseNumber(&input, nb) < 0) {
        Serial.println("error: ");
        Serial.println("Invalid number given.");
        return;
      }
      calibrateWithWeight(nb);
    
    } else if (cmd == '#') { // Print the version
      Serial.println("#: fixed");
    
    } else if (cmd == 't') { // Set the ratio
      if (parseNumber(&input, nb) < 0) {
        Serial.println("error: ");
        Serial.println("Invalid number given.");
        return;
      }
      scale.setScale(nb);

    } else if (cmd == 'e') {
      calibrateEmpty();
    
    } else {
      Serial.println("error: ");
      Serial.println("Unkown command");
      return;
    }
    Serial.println("done");
  }

  delay(100); // OPTIMIZE: Probably useless since it is busy wait I believe

}
