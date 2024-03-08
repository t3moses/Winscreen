
#ifndef TYPES
  #define TYPES

  #if (ARDUINO >= 100)
    #include "Arduino.h"
  #else
    #include "WProgram.h"
  #endif

#include "ww_constants.h"

typedef enum { PWS, HRM, AWS, ARO, NOTA } datagram_type_t;
typedef enum { ADMIN, USER } client_role_t;
typedef enum { WAITING_FOR_CONNECTION, WAITING_FOR_VARIABLES } client_state_t;
typedef enum { NORTH_UP, HEAD_UP, WIND_UP } e_perspective_t;

struct component_vector_t {

//
// x is the coordinate starboard abeam, y is the coordinate ahead.
//

  double x;
  double y;
  
};

struct radial_vector_t {

//
// l is the length, a is the angle clockwise from ahead.
//

  double l;
  double a;
  
};

struct fix_t {

  double d_utc_hours; // hours since zulu ( 0.0 .. 23.9999997 ).
  double d_latitude_minutes; // minutes north of equator ( -5400.0 .. +5400.0 ).
  double d_longitude_minutes; // minutes east of prime meridian ( -10800.0 .. +10800.0 ).
  
};

struct seatalk_data_t {
  
  double d_pws; // Speed according to the paddle wheel.
  double d_hrm; // Heading relative to magnetic north according to Seatalk.
  double d_aws; // Apparent wind speed according to Seatalk.
  double d_aro; // Apparent wind relative to wind vane offset according to Seatalk.
  double d_rdr; // Rudder position according to Seatalk.
  
};

struct gnss_data_t {
  
  component_vector_t x_cr;

};

struct var_display_data_t {

// Results of vector calculations for perspective by ww_json.

  radial_vector_t xd_nh; // true north relative to heading in feet and degrees
  radial_vector_t xd_mh; // magnetic north relative to heading in feet and degrees
  radial_vector_t xd_ch; // speed/course relative to heading
  radial_vector_t xd_th; // true wind relative to heading
  radial_vector_t xd_ah; // apparent wind relative to heading
  radial_vector_t xd_vh; // vmg relative to heading
  radial_vector_t xd_hh[ HULL_POINTS_BOUND + 1 ]; // hull shape
  radial_vector_t xd_rh; // compressed rudder vector relative to rudder post and heading
  radial_vector_t xd_ph; // rudder post relative to boat centre and heading
  double d_vmg; // vmg
  double d_dev; // compass deviation in degrees.
  double d_crs; // course relative to true north in degrees.
  bool b_upwind; // true value indicates upwind vmg.
  double d_crh; // leeway
  double d_trn; // north-relative true wind
  int8_t s8_heart_beat; // Indicates whether heart beat is on or off.
  char ac_utc[ 7 ]; // UTC time from satellite.

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
