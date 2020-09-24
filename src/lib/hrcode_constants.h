#ifndef _HRCODE_CONSTANTS_H
#define _HRCODE_CONSTANTS_H

// Variables that you can change
#define NB_ROWS 2
#define NB_COLS 2

#define OUT_FILE_NAME "out.bmp"

#define DPI 300 // Dots per inch (pixels per inch)

//#define FONT_NAME "fonts/monospace/MonospaceBold.ttf"
//#define FONT_NAME "fonts/monospace/Monospace.ttf"
//#define FONT_NAME "fonts/cmu/cmunorm.ttf"
//#define FONT_NAME "fonts/Courier Prime/Courier Prime Bold.ttf"
//#define FONT_NAME "fonts/Courier Prime/Courier Prime.ttf"
//#define FONT_NAME "fonts/Courier Prime Sans/Courier Prime Sans Bold.ttf"
//#define FONT_NAME "fonts/Courier Prime Sans/Courier Prime Sans.ttf"
//#define FONT_NAME "fonts/droid-sans-mono/DroidSansMono.ttf"
#define FONT_NAME "fonts/LiberationMono-Regular.ttf"
//#define FONT_NAME "fonts/LiberationMono-Bold.ttf"
#define FONT_SIZE 64
//#define TEXT_Y_OFFSET 4

#define SPACE_BETWEEN_STICKERS 4.0 // mm
#define OUTER_DIA 36.0 // mm
#define OUTER_RADIUS (OUTER_DIA / 2.0) // mm
#define INNER_DIA 32.0 // mm
#define INNER_RADIUS (INNER_DIA / 2.0) // mm
#define STICKER_WIDTH (OUTER_DIA + SPACE_BETWEEN_STICKERS) // mm

#define MARKER_DIA 4.4 // mm
#define MARKER_RADIUS (MARKER_DIA / 2.0) // mm
#define MARKERS_INTERSPACE 16.5 // mm
#define MARKERS_DIST_Y_FROM_MIDDLE 9.3 // mm
#define MARKERS_DIST_FROM_MIDDLE_SQ (MARKERS_INTERSPACE*MARKERS_INTERSPACE/4.0+MARKERS_DIST_Y_FROM_MIDDLE*MARKERS_DIST_Y_FROM_MIDDLE) // mm

#define CHAR_HEIGHT 4.0 // mm

#define LINE_1_WIDTH 25.0 // mm
#define LINE_2_WIDTH 25.0 // mm
#define LINE_3_WIDTH 15.0 // mm

#define TEXT_LINE_OFFSET -1.5 // mm
#define LINE_INTERSPACE 6.8 // mm

#endif
