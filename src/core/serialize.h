#ifndef MY_SERIALIZE_H
#define MY_SERIALIZE_H

// Implementing a custom json serializer for arduino.

#include "Motor.h"
#include "program.h"
#include "writer/writer.h"
#include "../utils/constants.h"

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
#ifndef ARDUINO
#include <string>
template <typename T>
void writeJson(T& writer, const char* key, std::string val) {
  writer << "\"" << key << "\": \"" << val << "\", ";
}
template <typename T, typename P>
void writeJson(T& writer, const char* key, P val) {
  writer << "\"" << key << "\": \"" << val << "\", ";
}
#endif

template <typename T>
void serialize(Program& p, T& out) {
  out << "{";
  for (int i = 0; p.motors[i] != 0; i++) {
    out << "\"axis_" << p.motors[i]->getName() << "\": ";
    serialize(p.motors[i], out);
    if (p.motors[i+1] != 0) {
      out << ", ";
    }
  }
  out << "}";
}

template <typename T>
void serialize(StepperMotor* axis, T& out) {
  out << "{";
  writeJson(out, PROPERTY_NAME, axis->getName());
  writeJson(out, PROPERTY_POSITION, axis->getPosition());
  writeJson(out, "m_max_speed_reached", axis->m_max_speed_reached);
  writeJson(out, "m_next_step_time", axis->m_next_step_time);
  writeJson(out, "m_start_time", axis->m_start_time);
  writeJson(out, "m_time_to_reach_middle_us", axis->m_time_to_reach_middle_us);
  writeJson(out, "m_time_ot_start_decelerating_us", axis->m_time_to_start_decelerating_us);
  writeJson(out, "m_acceleration", axis->m_acceleration);
  writeJson(out, "m_max_speed", axis->m_max_speed);
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

template <typename T>
void serialize(Motor* axis, T& out) {
  out << "{";
  writeJson(out, PROPERTY_NAME, axis->getName());
  writeJson(out, PROPERTY_POSITION, axis->getPosition());
  writeJson(out, PROPERTY_FORWARD, axis->isForward);
  writeJson(out, PROPERTY_REFERENCED, axis->isReferenced);
  writeJson(out, PROPERTY_REFERENCING, axis->isReferencing);
  out << "\"" << PROPERTY_DESTINATION << "\": " << axis->getDestination(); 
  // CAREFULL: JSON MUST NOT END WITH COMMA
  out << "}";
}


#endif
