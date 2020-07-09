#ifndef CONSTANTS_H
#define CONSTANTS_H

#define AXIS_X_MAX_POS 640.0 // mm
#define AXIS_Z_MAX_POS RAYON

#define ARM_WIDTH 111.0 // mm
#define ARM_LENGTH 370.0 // mm

#define ARMOIRE_WIDTH 1016.0 // mm
#define ARMOIRE_DEPTH 609.6 // mm
#define ARMOIRE_HEIGHT 1905.0 // mm

#define CAMERA_OFFSET 263.0 // mm
#define CAMERA_FIELD_OF_VIEW 75.0 // degrees

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

#define SERIAL_KEY_POLL 1111
#define SERIAL_KEY_SWEEP 1112
#define SERIAL_DELIMITER '\n'


#endif
