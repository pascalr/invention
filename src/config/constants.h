#ifndef CONSTANTS_H
#define CONSTANTS_H

#include "camera_constants.h"

#define sind(x) (sin(fmod((x),360) * M_PI / 180))
#define cosd(x) (cos(fmod((x),360) * M_PI / 180))

#define RAYON 340.0

#ifndef LOW
#define LOW 0
#define HIGH 1
#define INPUT 0
#define OUTPUT 1
#define PI 3.1415926535897932384626433832795
#endif

#define FORWARD true
#define REVERSE false

#define TOOL_X_MIN 0.0 // mm
#define TOOL_X_MAX 640.0 // mm
#define TOOL_Z_MIN -10.0 // mm
#define TOOL_Z_MAX RAYON

#define BASE_X_MIN 0.0 // mm
#define BASE_X_MAX 640.0 // mm
#define BASE_X_MIDDLE ((BASE_X_MAX + BASE_X_MIN)/2)

#define AXIS_X_MAX_POS BASE_X_MAX
#define AXIS_Z_MAX_POS RAYON

#define ARM_WIDTH 111.0 // mm
#define ARM_LENGTH 370.0 // mm

#define ARMOIRE_WIDTH 1016.0 // mm
#define ARMOIRE_DEPTH 609.6 // mm
#define ARMOIRE_HEIGHT 1905.0 // mm

#define DISTANCE_CAMERA_SHELF 1234

#define CAMERA_TOOL_DISTANCE 62.0 // mm
#define BASE_TOOL_DISTANCE RAYON

#define BASE_CAMERA_DISTANCE (BASE_TOOL_DISTANCE - CAMERA_TOOL_DISTANCE)

#define OFFSET_X 164.0 // mm, the distance between the sidewall and x = 0
#define OFFSET_Z 160.0 // mm, the distance between the back and z = 0

#ifndef PI
#define PI 3.1415926535897932384626433832795
#endif

#define MESSAGE_RECEIVED "ok"
#define MESSAGE_DONE "done"
// invalid_pending means that it should only process one command at once
// it is still listening if told to stop or query position
// anything else is an invalid_pending.
#define MESSAGE_INVALID_INPUT "invalid_input"
#define MESSAGE_INVALID_PENDING "invalid_pending"
#define MESSAGE_READY "ready"
#define MESSAGE_JSON "json"

#define PROPERTY_NAME "name"
#define PROPERTY_POSITION "pos"
#define PROPERTY_DESTINATION "dest"
#define PROPERTY_FORWARD "forward"
#define PROPERTY_REFERENCED "referenced"
#define PROPERTY_REFERENCING "referencing"
#define PROPERTY_SPEED "speed"
#define PROPERTY_MOTOR_ENABLED "mot_en"
#define PROPERTY_FORCE_ROTATION "force"
#define PROPERTY_DESTINATION_ANGLE "dest_angle"
#define PROPERTY_ANGLE "angle"
#define PROPERTY_DELTA_POSITION "d_pos"

#define SERIAL_KEY_POLL 1111
#define SERIAL_KEY_SWEEP 1112
#define SERIAL_DELIMITER '\n'

#define ERROR_EXPECTED_NUMBER -1
#define ERROR_EXPECTED_AXIS -2
#define ERROR_DESTINATION_TOO_HIGH -3
#define ERROR_DESTINATION_TOO_LOW -4
#define ERROR_OUT_OF_BOUNDS_MIN_X -5
#define ERROR_OUT_OF_BOUNDS_MAX_X -6
#define ERROR_AXIS_NOT_REFERENCED -7

// Maximum number of characters to represent a number in an input.
#define MAX_NUMBER_CHAR 12

#define HR_CODE_WIDTH 31 // mm

#endif
