#ifndef CONSTANTS_H
#define CONSTANTS_H

#define REFERENCE_SPEED_RPM 30.0 // tr/min

#define REFERENCE_POSITION_H 0.0 // mm
#define REFERENCE_POSITION_V 0.0 // mm
#define REFERENCE_POSITION_T 90.0 // degrees
#define REFERENCE_POSITION_R 0.0

#define DETECTED_CODES_BASE_PATH "client/storage/detected_codes/"

#ifndef PI
#define PI 3.1415926535897932384626433832795
#endif

#define asind(x) (asin(x * M_PI / 180))
#define sind(x) (sin(fmod((x),360) * M_PI / 180))
#define cosd(x) (cos(fmod((x),360) * M_PI / 180))

#define FORWARD true
#define REVERSE false

#define CHANGE_LEVEL_ANGLE_LOW 0.0
#define CHANGE_LEVEL_ANGLE_HIGH 180.0

#define ARM_WIDTH 111.0 // mm
#define ARM_LENGTH 370.0 // mm

#define ARMOIRE_WIDTH 1016.0 // mm
#define ARMOIRE_DEPTH 609.6 // mm
#define ARMOIRE_HEIGHT 1905.0 // mm

//#define CAMERA_TOOL_DISTANCE 62.0 // mm
//#define BASE_TOOL_DISTANCE RAYON

//#define BASE_CAMERA_DISTANCE (BASE_TOOL_DISTANCE - CAMERA_TOOL_DISTANCE)

//#define OFFSET_X 164.0 // mm, the distance between the sidewall and x = 0
//#define OFFSET_Z 160.0 // mm, the distance between the back and z = 0

#define MESSAGE_RECEIVED "ok"
#define MESSAGE_DONE "done"
#define MESSAGE_POSITION "@"
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
#define SERIAL_KEY_PROGRAM 1113
#define SERIAL_KEY_READER 1114
#define SERIAL_DELIMITER '\n'

#define ERROR_EXPECTED_NUMBER -1
#define ERROR_EXPECTED_AXIS -2
#define ERROR_DESTINATION_TOO_HIGH -3
#define ERROR_DESTINATION_TOO_LOW -4
#define ERROR_OUT_OF_BOUNDS_MIN_X -5
#define ERROR_OUT_OF_BOUNDS_MAX_X -6
#define ERROR_AXIS_NOT_REFERENCED -7

#define DB_TEST "client/db/development.sqlite3"
#define DB_DEV "client/db/development.sqlite3"
#define DB_PROD "client/db/development.sqlite3"

#define PIN_THETA_POT 4
#define POT_180_VAL 715.0
#define POT_0_VAL 21.0

// Maximum number of characters to represent a number in an input.
#define MAX_NUMBER_CHAR 12

#endif
