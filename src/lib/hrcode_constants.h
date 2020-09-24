#ifndef _HRCODE_CONSTANTS_H
#define _HRCODE_CONSTANTS_H

// Variables that you can change
#define HRCODE_NB_ROWS 2
#define HRCODE_NB_COLS 2

#define HRCODE_OUT_FILE_NAME "out.bmp"

#define HRCODE_DPI 300 // Dots per inch (pixels per inch)

//#define FONT_NAME "fonts/monospace/MonospaceBold.ttf"
//#define FONT_NAME "fonts/monospace/Monospace.ttf"
//#define FONT_NAME "fonts/cmu/cmunorm.ttf"
//#define FONT_NAME "fonts/Courier Prime/Courier Prime Bold.ttf"
//#define FONT_NAME "fonts/Courier Prime/Courier Prime.ttf"
//#define FONT_NAME "fonts/Courier Prime Sans/Courier Prime Sans Bold.ttf"
//#define FONT_NAME "fonts/Courier Prime Sans/Courier Prime Sans.ttf"
//#define FONT_NAME "fonts/droid-sans-mono/DroidSansMono.ttf"
#define HRCODE_FONT_NAME "fonts/LiberationMono-Regular.ttf"
//#define FONT_NAME "fonts/LiberationMono-Bold.ttf"
#define HRCODE_FONT_SIZE 64
//#define TEXT_Y_OFFSET 4

#define HRCODE_SPACE_BETWEEN_STICKERS 4.0 // mm
#define HRCODE_OUTER_DIA 36.0 // mm
#define HRCODE_OUTER_RADIUS (HRCODE_OUTER_DIA / 2.0) // mm
#define HRCODE_INNER_DIA 32.0 // mm
#define HRCODE_INNER_RADIUS (HRCODE_INNER_DIA / 2.0) // mm
#define HRCODE_STICKER_WIDTH (HRCODE_OUTER_DIA + HRCODE_SPACE_BETWEEN_STICKERS) // mm

#define HRCODE_MARKER_DIA 4.4 // mm
#define HRCODE_MARKER_RADIUS (HRCODE_MARKER_DIA / 2.0) // mm
#define HRCODE_MARKERS_INTERSPACE 16.5 // mm
#define HRCODE_MARKERS_DIST_Y_FROM_MIDDLE 9.3 // mm
#define HRCODE_MARKERS_DIST_FROM_MIDDLE_SQ (HRCODE_MARKERS_INTERSPACE*HRCODE_MARKERS_INTERSPACE/4.0+HRCODE_MARKERS_DIST_Y_FROM_MIDDLE*HRCODE_MARKERS_DIST_Y_FROM_MIDDLE) // mm

#define HRCODE_CHAR_HEIGHT 4.0 // mm

#define HRCODE_LINE_1_WIDTH 25.0 // mm
#define HRCODE_LINE_2_WIDTH 25.0 // mm
#define HRCODE_LINE_3_WIDTH 15.0 // mm

#define HRCODE_TEXT_LINE_OFFSET -1.5 // mm
#define HRCODE_LINE_INTERSPACE 6.8 // mm

#endif
