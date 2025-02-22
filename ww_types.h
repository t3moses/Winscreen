
#ifndef TYPES
  #define TYPES

  #if (ARDUINO >= 100)
    #include "Arduino.h"
  #else
    #include "WProgram.h"
  #endif

#include "ww_constants.h"

// typedef enum { HDG, FIX, CRS } course_t;
typedef enum { PWS, HRM, AWS, ARO, NOTA } datagram_type_t;
typedef enum { ADMIN, USER } client_role_t;
typedef enum { WAITING_FOR_CONNECTION, WAITING_FOR_VARIABLES } client_state_t;
typedef enum { NORTH_UP, HEAD_UP, WIND_UP } perspective_t;

struct component_vector_t {

//
// x is the coordinate starboard abeam, y is the coordinate ahead.
//

  double x;
  double y;
  
};

struct radial_vector_t {

//
// l is the length, a is the angle clockwise from reference.
//

  double l;
  double a;
  
};

struct seatalk_data_t {
  
  double d_pws; // Speed according to the paddle wheel.
  double d_hrm; // Heading relative to magnetic north according to Seatalk.
  double d_aws; // Apparent wind speed according to Seatalk.
  double d_aro; // Apparent wind relative to wind vane offset according to Seatalk.
  double d_rdr; // Rudder position according to Seatalk.
  
};

struct gnss_data_t {
  
  double d_sns; // Boat speed as measured by the GNSS module.
  double d_grn; // Course relative to true north as measure by the GNSS module.
  char* pac_utc; // Pointer to array of char representing time since midnight zulu as measured by the GNSS module.

};

struct var_display_data_t {

// Results of vector calculations for perspective by ww_json.

  radial_vector_t xd_nh; // true north arrow point relative to heading in feet and degrees
  radial_vector_t xd_mh; // magnetic north arrow point relative to heading in feet and degrees
  radial_vector_t xd_gh; // speed/course relative to heading according to GNSS in knots and degrees
  radial_vector_t xd_sh; // speed/course relative to heading according to SeaTalk in knots and degrees
  radial_vector_t xd_th; // true wind speed and angle relative to heading in knots and degrees
  radial_vector_t xd_ah; // apparent wind speed and angle relative to heading in knots and degrees
  radial_vector_t xd_vh; // vmg speed and angle relative to heading in knots and degrees
  radial_vector_t axd_hh[ HULL_POINTS_BOUND + 1 ]; // hull shape
  radial_vector_t xd_rh; // compressed rudder tip position and angle relative to rudder post and heading in feet and degrees
  radial_vector_t xd_ph; // rudder post position and angle relative to boat centre and heading in feet and degrees
  double d_ah; // apparent wind angle relative to heading
  double d_vt; // vmg in knots
  double d_gh; // leeway according to GNSS in degrees
  double d_tn; // true wind relative to true north in degrees
  double d_hm; // heading relative to magnetic north according to Seatalk in degrees
  bool b_up; // true value indicates upwind vmg, false indicates downwind
  int8_t s8_hb; // indicates whether heart beat is on or off
  char* pac_utc; // pointer to char array containing utc from gnss

};

struct correction_data_t {
  
  int16_t s16_orh; // Wind vane offset.
  int16_t s16_mrn; // Compass variation.
  int16_t s16_fc; // Index into the ad_fc[ ] array of doubles in Hz.
  
};

struct arrowhead_t { // Three vectors define the corners of an arrowhead.

  radial_vector_t x_p0;
  radial_vector_t x_p1;
  radial_vector_t x_p2;
   
};

#endif
