#include "hx711.h"
#include "arduino_lib.h"

#define CALIBRATION_WEIGHT 654

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

void calibrateWithWeight() {
  float ratio = (scale.averageValue() - offset) / CALIBRATION_WEIGHT;
  //writer << ratio;
  Serial.println(ratio);
  scale.setScale(ratio);
}

void loop() {

  if (reader.inputAvailable()) {

    char cmd = reader.getByte();

    // ignore newline characters
    if (cmd == '\r' || cmd == '\n') return;

    Serial.print("Received: ");
    Serial.println(cmd);

    // get weight
    if (cmd == 'w') {
      Serial.print(scale.getGram(), 1); // Print the gram value with one decimal precision
      Serial.println(" g");
      
    } else if (cmd == 'c') {
      calibrateWithWeight();

    } else if (cmd == '#') { // Print the version
      Serial.println("fixed");

    } else if (cmd == 'e') {
      calibrateEmpty();
    
    } else {
      Serial.println("Unkown command");
      return;
      // TODO: Discard the rest of the input to be sure.
    }
    Serial.println("done");
  }

  delay(100);
}
