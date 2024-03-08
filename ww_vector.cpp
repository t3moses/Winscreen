
#include "ww_constants.h"
#include "ww_types.h"
#include "ww_vector.h"
#include "ww_filter.h"

ww_filter rh_filter; // Filters the rudder data.
ww_filter mh_filter; // Filters the seatalk heading data.
ww_filter ah_filter; // Filters the Seatalk apparent wind data.
ww_filter cr_filter; // Filters the GNSS boat data.

var_display_data_t* ww_vector::p_var_display_data;
seatalk_data_t* ww_vector::p_seatalk_data;
gnss_data_t* ww_vector::p_gnss_data;
correction_data_t* ww_vector::p_correction_data;

double d_rudder_coefficient_a;
double d_rudder_coefficient_b;
double d_rudder_coefficient_c;

// Intermediate results.

double d_hrn; // Heading relative to true north.
double d_trh; // True wind relative to heading.

radial_vector_t xd_hrm; // Radial heading relative to magnetic north.
radial_vector_t xd_hrn; // Radial heading relative to true north.
radial_vector_t xd_crn; // Radial speed/course relative to true north.



void ww_vector::v_begin( var_display_data_t* p_var_display_data, seatalk_data_t* p_seatalk_data, gnss_data_t* p_gnss_data,
  correction_data_t* p_correction_data ) {

  ww_vector::p_var_display_data = p_var_display_data;
  ww_vector::p_seatalk_data = p_seatalk_data;
  ww_vector::p_gnss_data = p_gnss_data;
  ww_vector::p_correction_data = p_correction_data;
  d_rudder_coefficient_a = RUDDER_FORMULA_S0 + RUDDER_FORMULA_S1 - 2.0;
  d_rudder_coefficient_b = 3.0 - 2.0 * RUDDER_FORMULA_S0 - RUDDER_FORMULA_S1;
  d_rudder_coefficient_c = RUDDER_FORMULA_S0;

  rh_filter.v_begin( );
  mh_filter.v_begin( );
  ah_filter.v_begin( );
  cr_filter.v_begin( );

}


void ww_vector::v_display_from_instruments( ) {

//
// Calculate the display data as radial vectors in knots, degrees or feet.
// Angles are relative to heading.
// The data-flow is shown in the vectors_02.key file.
//

  ww_vector::p_var_display_data->xd_rh = ww_vector::x_reflect_x( ww_vector::x_compress_angle( ww_vector::x_radial_from_component( rh_filter.x_step( ww_vector::x_component_from_l_a( RUDDER_LENGTH, ww_vector::p_seatalk_data->d_rdr )))));
  xd_hrm = ww_vector::x_radial_from_component( mh_filter.x_step( ww_vector::x_component_from_l_a( COMPASS_RADIUS, ww_vector::p_seatalk_data->d_hrm )));
  ww_vector::p_var_display_data->xd_mh = ww_vector::x_reflect_y( xd_hrm );
  xd_hrn = ww_vector::x_rotate( xd_hrm, (double)ww_vector::p_correction_data->s16_mrn );
  ww_vector::p_var_display_data->xd_nh = ww_vector::x_reflect_y( xd_hrn );
  ww_vector::p_var_display_data->xd_ah = ww_vector::x_rotate( ww_vector::x_radial_from_component( ah_filter.x_step( ww_vector::x_component_from_l_a( ww_vector::p_seatalk_data->d_aws, ww_vector::p_seatalk_data->d_aro ))), (double)ww_vector::p_correction_data->s16_orh );
  xd_crn = ww_vector::x_radial_from_component( cr_filter.x_step( ww_vector::p_gnss_data->x_cr ));
  ww_vector::p_var_display_data->d_dev = ww_vector::d_angle_from_radial( xd_crn ) - ww_vector::d_angle_from_radial( xd_hrm ) - (double)ww_vector::p_correction_data->s16_mrn;
  ww_vector::p_var_display_data->d_crn = ww_vector::d_angle_from_radial( xd_crn );
  d_hrn = ww_vector::d_angle_from_radial( xd_hrn );
  ww_vector::p_var_display_data->xd_ch = ww_vector::x_rotate( xd_crn, -d_hrn );
  ww_vector::p_var_display_data->xd_th = ww_vector::x_subtract_radial( ww_vector::p_var_display_data->xd_ah, ww_vector::p_var_display_data->xd_ch );
  ww_vector::p_var_display_data->d_crh = ww_vector::d_angle_from_radial( ww_vector::p_var_display_data->xd_ch );
  ww_vector::p_var_display_data->d_trn = ww_vector::d_add_angle( ww_vector::d_angle_from_radial( ww_vector::p_var_display_data->xd_th ), ww_vector::d_angle_from_radial( xd_hrn ));
  d_trh = ww_vector::d_angle_from_radial( ww_vector::p_var_display_data->xd_th );
  ww_vector::p_var_display_data->d_vmg = ww_vector::d_projection( ww_vector::d_length_from_radial( xd_crn ), ww_vector::d_add_angle( ww_vector::p_var_display_data->d_crh, -d_trh ));
  ww_vector::p_var_display_data->b_upwind = ww_vector::b_upwind_from_instruments( );
  ww_vector::p_var_display_data->xd_vh = ww_vector::x_radial_from_component( ww_vector::x_component_from_l_a( ww_vector::p_var_display_data->d_vmg, d_trh ));
  for( int8_t s8_index = 0; s8_index <= HULL_POINTS_BOUND; s8_index++ ) {
    ww_vector::p_var_display_data->xd_hh[ s8_index ] = ww_vector::x_radial_from_component( ww_vector::x_component_from_x_y( (double)as8_hull[ 2 * s8_index ], (double)as8_hull[ 2 * s8_index + 1 ] ));
  }
  ww_vector::p_var_display_data->xd_ph = ww_vector::x_radial_from_component( ww_vector::x_component_from_l_a( RUDDER_POST_Y, 180.0 ));

}



radial_vector_t ww_vector::x_reflect_x( radial_vector_t x_vector ) {

//
// Return the radial vector that is the mirror image of the input vector about the x axis.
//

radial_vector_t x_vector_1;

  x_vector_1.l = x_vector.l;
  x_vector_1.a = ww_vector::d_normalize_angle( 180.0 - x_vector.a );

  return x_vector_1;

}



radial_vector_t ww_vector::x_reflect_y( radial_vector_t x_vector ) {

//
// Return the radial vector that is the mirror image of the input vector about the y axis.
//

radial_vector_t x_vector_1;

  x_vector_1.l = x_vector.l;
  x_vector_1.a = - x_vector.a;

  return x_vector_1;

}



radial_vector_t ww_vector::x_rotate( radial_vector_t x_vector, double d_angle ) {

//
// Rotate the input vector by the input angle.
//

radial_vector_t x_vector_1;

  x_vector_1.l = x_vector.l;
  x_vector_1.a = ww_vector::d_add_angle( x_vector.a, d_angle );

  return x_vector_1;

}



component_vector_t ww_vector::x_component_from_l_a( double d_l, double d_a ) {

component_vector_t x_vector;

  x_vector.x = d_l * sin( RADIANS_FROM_DEGREES * d_a );
  x_vector.y = d_l * cos( RADIANS_FROM_DEGREES * d_a );

  return x_vector;

}



component_vector_t ww_vector::x_component_from_x_y( double d_x, double d_y ) {

component_vector_t x_vector;

  x_vector.x = d_x;
  x_vector.y = d_y;

  return x_vector;

}



component_vector_t ww_vector::x_add_component( component_vector_t x_component_vector_1, component_vector_t x_component_vector_2 ) {

component_vector_t x_vector;

  x_vector.x = x_component_vector_1.x + x_component_vector_2.x;
  x_vector.y = x_component_vector_1.y + x_component_vector_2.y;

  return x_vector;

}



component_vector_t ww_vector::x_subtract_component( component_vector_t x_component_vector_1, component_vector_t x_component_vector_2 ) {

component_vector_t x_vector;

  x_vector.x = x_component_vector_1.x - x_component_vector_2.x;
  x_vector.y = x_component_vector_1.y - x_component_vector_2.y;

  return x_vector;

}



double ww_vector::d_angle_from_radial( radial_vector_t x_vector ) {

//
// Return the angle of the input vector.
//

  return x_vector.a;

}



radial_vector_t ww_vector::x_radial_from_component( component_vector_t x_component_vector ) {

//
// Return the component representation of the input radial vector.
//

radial_vector_t x_vector_1;

  x_vector_1.l = ww_vector::d_length_from_component( x_component_vector );
  x_vector_1.a = ww_vector::d_angle_from_component( x_component_vector );

  return x_vector_1;

}



component_vector_t ww_vector::x_component_from_radial( radial_vector_t x_vector_1 ) {

// Return radial vector x_vector_1 in component form.

component_vector_t x_vector_2;

  x_vector_2.x = x_vector_1.l * sin( RADIANS_FROM_DEGREES * x_vector_1.a );
  x_vector_2.y = x_vector_1.l * cos( RADIANS_FROM_DEGREES * x_vector_1.a );

  return x_vector_2;
    
}



radial_vector_t ww_vector::x_compress_angle( radial_vector_t x_radial_vector ) {

radial_vector_t x_radial_vector_1;

x_radial_vector_1.l = x_radial_vector.l;

  if( x_radial_vector.a >= 0.0 ) {

    x_radial_vector_1.a =  RUDDER_FORMULA_MAX_Y * ((
      d_rudder_coefficient_a * x_radial_vector.a / RUDDER_FORMULA_MAX_X + 
      d_rudder_coefficient_b ) * x_radial_vector.a / RUDDER_FORMULA_MAX_X + 
      d_rudder_coefficient_c ) * x_radial_vector.a / RUDDER_FORMULA_MAX_X;

  }
  
  else {

    x_radial_vector_1.a = - RUDDER_FORMULA_MAX_Y * ((
      d_rudder_coefficient_a * - x_radial_vector.a / RUDDER_FORMULA_MAX_X + 
      d_rudder_coefficient_b ) * - x_radial_vector.a / RUDDER_FORMULA_MAX_X + 
      d_rudder_coefficient_c ) * - x_radial_vector.a / RUDDER_FORMULA_MAX_X;

  }

  return x_radial_vector_1;

}



double ww_vector::d_length_from_radial( radial_vector_t x_vector ) {

//
// Return the length of radial vector x_vector.  Always a positive value.
//

  return x_vector.l;

}


double ww_vector::d_length_from_component( component_vector_t x_vector ) {

//
// Return the length of component vector x_vector.  Always a positive value.
//

  return sqrt( x_vector.x * x_vector.x + x_vector.y * x_vector.y );

}


double ww_vector::d_angle_from_component( component_vector_t x_vector ) {

//
// Return the ange of component vector x_vector.
//

  if( x_vector.x == 0.0 && x_vector.y == 0.0 ) {
    return 0.0;
  }
  else {
    return DEGREES_FROM_RADIANS * atan2( x_vector.x, x_vector.y );
  }
}



radial_vector_t ww_vector::x_subtract_radial( radial_vector_t x_vector_1, radial_vector_t x_vector_2 ) {

// x_vector_1 and x_vector_2 are radial vectors.
// Return x_vector_1 - x_vector_2 in radial form.

  return ww_vector::x_radial_from_component( ww_vector::x_subtract_component( ww_vector::x_component_from_radial( x_vector_1 ), ww_vector::x_component_from_radial( x_vector_2 )));

}



double ww_vector::d_add_angle( double d_a1, double d_a2 ) {

  return ww_vector::d_normalize_angle( d_a1 + d_a2 );

}



double ww_vector::d_projection( double d_length, double d_angle ) {

//
// Return the projection of the length onto a line at an angle of d_angle.
// Angles > 90 degrees and < -90 degrees result in a negative value.
//

  return( d_length * cos( RADIANS_FROM_DEGREES * d_angle ));

}



bool ww_vector::b_upwind_from_instruments( ) {

  double d_crt = d_add_angle( ww_vector::p_var_display_data->d_crh, -d_trh );

  if( d_crt > -90.0 && d_crt < 90.0 ) return true; else return false;

}



double ww_vector::d_normalize_angle( double d_a ) {

  if( d_a <= -180.0 ) return d_a + 360.0;
  else if( d_a > 180.0 ) return d_a - 360.0;
  else return d_a;

}
