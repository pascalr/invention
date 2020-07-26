#ifndef ARDUINO_MOCK_H
#define ARDUINO_MOCK_H
#ifndef ARDUINO

#include <stdint.h>

#define HIGH 0x1
#define LOW  0x0

#define INPUT 0x0
#define OUTPUT 0x1
#define INPUT_PULLUP 0x2

#ifndef PI
#define PI 3.1415926535897932384626433832795
#endif

#define HALF_PI 1.5707963267948966192313216916398
#define TWO_PI 6.283185307179586476925286766559
#define DEG_TO_RAD 0.017453292519943295769236907684886
#define RAD_TO_DEG 57.295779513082320876798154814105
#define EULER 2.718281828459045235360287471352

void pinMode(uint8_t pin, uint8_t mode);
void digitalWrite(uint8_t pin, uint8_t val);
//int digitalRead(uint8_t pin);
//int analogRead(uint8_t pin);
void analogReference(uint8_t mode);
void analogWrite(uint8_t pin, int val);

//unsigned long millis(void) {}
//unsigned long micros(void) {}
//void delay(unsigned long ms) {}
//void delayMicroseconds(unsigned int us) {}

#endif
#endif
