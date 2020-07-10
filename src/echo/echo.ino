int incomingByte;
void setup() {
    Serial.begin(115200);
}
void loop() {
  // send data only when you receive data:
  if (Serial.available() > 0) {
    
    int incomingByte = Serial.read();
  
    // say what you got:
    Serial.print((char) incomingByte);
  }
}
