#ifndef MY_SERIALIZE_H
#define MY_SERIALIZE_H

// Implementing a custom json serializer for arduino.


#include "program.h"
#include "axis.h"
#include "writer.h"
#include "../config/constants.h"

template <typename T>
void writeJson(T& writer, const char* key, const char* val) {
  writer << "\"" << key << "\": \"" << val << "\", ";
}
template <typename T>
void writeJson(T& writer, const char* key, bool val) {
  writer << "\"" << key << "\": " << (val ? "true" : "false") << ", ";
}
template <typename T>
void writeJson(T& writer, const char* key, char val) {
  writer << "\"" << key << "\": \"" << val << "\", ";
}
template <typename T>
void writeJson(T& writer, const char* key, double val) {
  writer << "\"" << key << "\": " << val << ", ";
}
template <typename T>
void writeJson(T& writer, const char* key, int val) {
  writer << "\"" << key << "\": " << val << ", ";
}
template <typename T>
void writeJson(T& writer, const char* key, long val) {
  writer << "\"" << key << "\": " << val << ", ";
}
template <typename T>
void writeJson(T& writer, const char* key, unsigned long val) {
  writer << "\"" << key << "\": " << val << ", ";
}

template <typename T>
void serialize(Program& p, T& out) {
  out << "{";
  for (int i = 0; p.movingAxes[i] != 0; i++) {
    out << "\"axis_" << p.movingAxes[i]->getName() << "\": ";
    serialize(p.movingAxes[i], out);
    if (p.movingAxes[i+1] != 0) {
      out << ", ";
    }
  }
  out << "}";
}

template <typename T>
void serialize(Axis* axis, T& out) {
  out << "{";
  writeJson(out, PROPERTY_NAME, axis->getName());
  writeJson(out, PROPERTY_POSITION, axis->getPosition());
  /*writeJson(out, PROPERTY_FORWARD, axis->isForward);
  writeJson(out, PROPERTY_REFERENCED, axis->isReferenced);
  writeJson(out, PROPERTY_REFERENCING, axis->isReferencing);
  writeJson(out, PROPERTY_MOTOR_ENABLED, axis->isMotorEnabled);
  writeJson(out, PROPERTY_FORCE_ROTATION, axis->forceRotation);
  if (axis->name == 'x') {
    writeJson(out, PROPERTY_DELTA_POSITION, ((HorizontalAxis*)axis)->getDeltaPosition());
  } else if (axis->name == 'z') {
    writeJson(out, PROPERTY_DESTINATION_ANGLE, ((ZAxis*)axis)->getDestinationAngle());
    writeJson(out, PROPERTY_ANGLE, ((ZAxis*)axis)->getPositionAngle());
  }
  out << "\"" << PROPERTY_SPEED << "\": " << axis->speed; 
  */
  out << "\"" << PROPERTY_DESTINATION << "\": " << axis->getDestination(); 
  // CAREFULL: JSON MUST NOT END WITH COMMA
  out << "}";
}


#endif
