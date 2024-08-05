#ifndef CONSTANTS
  #define CONSTANTS

  #if (ARDUINO >= 100)
    #include "Arduino.h"
  #else
    #include "WProgram.h"
  #endif

// Processor constants.

#define STOP while( true );
#define FCPU 80 * 1000 * 1000 // Hz

// Test points.  Ground - WW_0_09 J1 Pin 1

#define TEST1 GPIO_NUM_2 // GPIO2 - J1 Pin 2
#define TEST2 GPIO_NUM_3 // GPIO3 - J1 Pin 3

// Seatalk constants.

#define BAUD_RATE 4800
#define BITS_PER_FRAME 11
#define BIT_TIME_THRESHOLD 0.75 // Allows for start bits of 1 or 1.5 bits duration.
#define MAX_FRAMES_PER_TARGET_DATAGRAM 5 // Target datagrams contain no more than this number of frames.
#define FRAME_INDEX_BOUND 4 // Upper bound on the index of frames in a datagram (not just target datagrams).
#define TRANSITION_INDEX_BOUND ( BITS_PER_FRAME * FRAME_INDEX_BOUND ) // Upper bound on the number of transitions in a datagram.
#define MAXLONG 0xFFFFFFFF
#define DATAGRAM_WATCHDOG_PRESCALE 2048
#define TICKS_PER_FRAME (uint16_t)((( FCPU / DATAGRAM_WATCHDOG_PRESCALE ) * BITS_PER_FRAME ) / BAUD_RATE )
#define SEATALK_BUS GPIO_NUM_20 // GPIO20 - XIAO ESP32C3 Pin 8.
#define INITIAL_AW_OFFSET 0.0 // Initial wind vane offset.  Seatalk reported apparent wind angle value when head-to-wind.

// GNSS constants.

#define SDA_PIN GPIO_NUM_6 // GPIO6 - XIAO ESP32C3 Pin 5.
#define SCL_PIN GPIO_NUM_7 // GPIO7 - XIAO ESP32C3 Pin 6.
#define GNSS_ADDRESS 0x42 // GNSS module SPI bus address.
#define NMEA_INDEX_BOUND 200
#define NMEA_INDEX_MAX 100 // Maximum index of a chunk of NMEA data.
#define FIELD_INDEX_BOUND 8 // Maximum index of a NMEA field.
#define FIX_SERIES_INDEX_MAX 1 // Maximum index of the fix series.

#define I2C_CLOCK_FREQ 100000
#define I2C_TIMEOUT_MILLIS 20
#define I2C_TIMEOUT_TICKS ( I2C_TIMEOUT_MILLIS / portTICK_PERIOD_MS )

#define SNS_THRESHOLD 0.5 // Course and speed are ignored below this threshold, knots.

// Web socket constants.

#define DISCONNECTED -1
#define DISCONNECT_SECONDS 4
#define CLIENT_DISCONNECT ( HMI_UPDATE_RATE * DISCONNECT_SECONDS )
#define WS_KEY_INDEX_BOUND 80
#define WS_VALUE_INDEX_BOUND 80

// HMI constants

#define HMI_UPDATE_RATE 6.0 // The display update rate, Hz.
#define HMI_UPDATE_PRESCALER 2048
#define TICKS_PER_HMI_UPDATE_INTERVAL ( FCPU / ( HMI_UPDATE_PRESCALER * (uint16_t)HMI_UPDATE_RATE ))
#define PIXELS_PER_FOOT 5.0 // Scale of Page 1 (overview display)
#define PIXELS_PER_KNOT 10.0
#define PIXELS_PER_DEGREE 10.0
#define PAGE_BOUND 6 // Page number of the last page.
#define DEBOUNCE_COUNT 3 // Heartbeat count for debounce.
#define URL_BOUND 40 // Max index of array containing a url.
#define JSON_BOUND 4000 // Must be >= the array size defined by StaticJsonDocument<..> doc in ww_json.cpp.
#define HTML_BOUND 10000 // Must be >= the size of the HTML.
#define CLIENT_BOUND 5 // Maximum index for the client list.
#define SOCKET_BOUND 5 // Maximum index for the socket list.
#define SVG_BOUND 10000 //  Must be >= the size of the splash screen.

// Page 1 constants.  Overview display.

#define BACKGROUND "rgba(0,0,0,1.0)" // Black.
#define HULL_COLOUR "rgba(127,127,127,1.0)" // Opaque grey.
#define HULL_SCALE 1.6 // Defines the size of the displayed boat hull.
#define RUDDER_COLOUR "rgba(255,255,255,1.0)" // Opaque white.
#define RUDDER_WEIGHT 8
#define RUDDER_POST_Y 11.0 // Y position of the rudder post in relation to the origin (feet).
#define RUDDER_RADIUS 3.0 // Feet.
#define RUDDER_FORMULA_S0 3.5 // Parameters of the rudder angle compression function.
#define RUDDER_FORMULA_S1 0.0
#define RUDDER_FORMULA_MAX_X 90.0 // Degrees.
#define RUDDER_FORMULA_MAX_Y 90.0 // Degrees.
#define BOAT_COLOUR "rgba(255,255,210,1.0)" // Pale yellow.  Colour of the boat vector (CRH).
#define BOAT_SHADOW_COLOUR "rgba(255,255,210,1.0)"
#define BOAT_WEIGHT 8
#define BOAT_SHADOW_WEIGHT 2
#define SAT_COLOUR "rgb(255,192,203,1.0)" // Colour of the GNSS vector.
#define SAT_WEIGHT 2
#define TW_COLOUR "rgba(255,210,255,1.0)" // Pale magenta.  Colour of the true wind vector (TWS and TRH).
#define TW_SHADOW_COLOUR "rgba(255,210,255,1.0)"
#define TW_WEIGHT 8
#define TW_SHADOW_WEIGHT 2
#define AW_COLOUR "rgba(210,255,255,1.0)" // Pale cyan.  Colour of the apparent wind vector (AWS and ARH).
#define AW_WEIGHT 8
#define VMG_UPWIND_COLOUR "rgba(0,0,0,1.0)" // Opaque black.  Colour of the VMG upwind vector.
#define VMG_DOWNWIND_COLOUR "rgba(255,255,255,1.0)"
#define VMG_UPWIND_SHADOW_COLOUR "rgba(255,255,255,1.0)" // Opaque white.  Colour of the VMG upwind vector.
#define VMG_DOWNWIND_SHADOW_COLOUR "rgba(255,255,255,1.0)"
#define VMG_WEIGHT 4
#define VMG_SHADOW_WEIGHT 2

#define PANEL_WIDTH 300 // Pixels
#define VECTOR_PANEL_HEIGHT 300
#define BALL_PANEL_HEIGHT 35
#define FSD 20 // Knots.
#define DISPLAY_AREA_RATIO 0.9 // Portion of display used by vectors.
#define CRH_BALL_COLOUR BOAT_COLOUR
#define VMG_BALL_UPWIND_COLOUR VMG_DOWNWIND_COLOUR
#define VMG_BALL_DOWNWIND_COLOUR VMG_DOWNWIND_COLOUR
#define TRN_BALL_COLOUR TW_COLOUR

#define HULL_POINTS_BOUND 12 // Number of points in the hull shape minus 1.
#define COMPASS_RADIUS 25 // Radius of compass needle point (feet).
#define COURSE_RADIUS 25 // Radius of course needle point (feet).
#define TRUE_NORTH_COLOUR "rgba(255,255,255,1.0)" // Colour of the true north indicator.
#define MAGNETIC_NORTH_COLOUR "rgba(255,255,255,1.0)" // Colour of the magnetic north indicator.
#define COURSE_COLOUR "rgba(255,255,210,1.0)" // Pale yellow.  Colour of the course indicator.
#define MAGNETIC_NORTH_WEIGHT 2
#define NORTH_POINT_ANGLE 15 // Half the angle at point of north arrow (degrees)
#define NORTH_SIDE 20.0 // Length of the side of the north triangle (pixels).
#define AH_BAND 180.0 // ARH change represented by the ARH ribbon, degrees.
#define CH_BAND 40.0 // CRH change represented by the CRH ribbon, degrees.
#define VT_BAND 10.0 // VMG change represented by the VMG ribbon, knots.
#define TN_BAND 360.0 // TRN change represented by the TRN ribbon, degrees.

// Heartbeat constants

#define HEART_BEAT_X 5 // Top left of square from left edge of display, pixels.
#define HEART_BEAT_Y 5 // Top left of square from top edge of display, pixels.
#define HEART_BEAT_SIDE 15 // Length of side of square in pixels.
#define HEART_BEAT_RATE 2.0 // Hz.
#define HEART_BEAT_COLOUR_ON "rgba(255,255,255,1.0)" // Opaque white.
#define HEART_BEAT_COLOUR_OFF "rgba(0,0,0,1.0)" // Opaque black.

// Hull coordinates.

const int8_t as8_hull[ 30 ] = {
                      0, 17,
                     -4, 10,
                     -6, 5,
                     -7, 0,
                     -7, -5,
                     -6, -10,
                     -3, -15,
                     3, -15,
                     6, -10,
                     7, -5,
                     7, 0,
                     6, 5,
                     4, 10 }; // { x, y } feet relative to boat shape centre.

#define ARROWHEAD_LENGTH 0.15 // Relative length of an arrowhead.
#define ARROWHEAD_WIDTH 0.1 // Relative width of an arrowhead.

// Page 2 constants.  VMG display.

#define P2_BACKGROUND "rgba(0,0,0,1.0)" // Black.
#define P2_TEXT_COLOUR "rgba(255,255,255,1.0)" // White
#define P2_PANEL_WIDTH PANEL_WIDTH
#define P2_PANEL_HEIGHT 425
#define P2_ROW_TOP_1 80
#define P2_ROW_TOP_2 260
#define P2_ROW_TOP_3 400

// Page 3 constants.  Admin wind-vane offest, variation, and filter adjustments.

#define OFFSET_LOWER_BOUND -90.0 // Windvane offset limits.
#define OFFSET_UPPER_BOUND 90.0
#define OFFSET_INCREMENT 2
#define VARIATION_LOWER_BOUND -20.0 // Variation limits.
#define VARIATION_UPPER_BOUND 20.0
#define VARIATION_INCREMENT 1
#define P3_BACKGROUND "rgba(0,0,0,1.0)" // Black.
#define P3_TEXT_COLOUR "rgba(255,255,255,1.0)"
#define P3_OFFSET_COLOUR AW_COLOUR
#define P3_VARIATION_COLOUR BOAT_COLOUR
#define P3_ROW_WIDTH_1 75
#define P3_ROW_WIDTH_2 150
#define P3_ROW_WIDTH_3 300
#define P3_ROW_HEIGHT 65
#define P3_INDICATOR_WIDTH 5
#define P3_ZERO_COLOUR "rgba(255,255,255,1.0)"

// Page 4 constants.  Admin deviation display.

#define P4_BACKGROUND "rgba(0,0,0,1.0)" // Black.
#define P4_TEXT_COLOUR "rgba(255,255,255,1.0)"
#define P4_PANEL_WIDTH PANEL_WIDTH
#define P4_PANEL_HEIGHT 425
#define P4_ROW_TOP_1 60
#define P4_ROW_TOP_2 120
#define P4_ROW_TOP_3 180
#define P4_ROW_TOP_4 240
#define P4_ROW_TOP_5 280
#define P4_ROW_TOP_6 340

// Second-order low-pass data filter constants.

#define FC_INDEX_BOUND 5
#define INITIAL_FC_INDEX 2 // The initial index of the centre frequency in the ad_fc[] array.
#define QQ 0.707 // Q factor.

// Vector constants.

#define RADIANS_FROM_DEGREES 0.0174533
#define DEGREES_FROM_RADIANS 57.2958
#define MIN_NORTH 0.001 // Avoid divide by zero.

// Variation constants.

#define INITIAL_MRN -13.0 // Initial compass variation.

// non-volatile storage constants.

#define ERASE_TIMEOUT 3000 // User prompt timeout for NVS erasure (milliseconds).

#endif
