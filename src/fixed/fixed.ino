#include "hx711.h"

#define CALIBRATION_WEIGHT 642

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
  float ratio = (w - offset) / CALIBRATION_WEIGHT;
  scale.setScale(ratio);
}

void loop() {

  if (reader.inputAvailable()) {
    char cmd = reader.getByte();

    // get weight
    if (cmd == 'w') {
      //writer.doPrint();
      Serial.print(scale.getGram(), 1); // Print the gram value with one decimal precision
      Serial.println(" g");

    } else if (cmd = 'c') {
      calibrateWithWeight();

    } else if (cmd = 'e') {
      calibrateEmpty();
    
    } else {
      writer << "Unkown command " << cmd << "\n";
      // TODO: Discard the rest of the input to be sure.
    }
  }

  delay(100);
}
