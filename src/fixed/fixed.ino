#include "hx711.h"

// Hx711.DOUT - pin #A1
// Hx711.SCK - pin #A0
Hx711 scale(A1, A0);

void setup() {

  Serial.begin(115200);

}

void loop() {

  Serial.print(scale.getGram(), 1);
  Serial.println(" g");

  delay(200);
}
