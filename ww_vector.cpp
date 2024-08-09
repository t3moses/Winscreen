
#include "ww_constants.h"
#include "ww_types.h"
#include "ww_vector.h"
#include "ww_filter.h"

ww_filter rd_filter; // Filters the rudder data.
ww_filter hd_filter; // Filters the Seatalk heading data.
ww_filter aw_filter; // Filters the Seatalk apparent wind data.
ww_filter sc_filter; // Filters the selected course data.
ww_filter gc_filter; // Filters the GNSS course data.

var_display_data_t* ww_vector::p_var_display_data;
seatalk_data_t* ww_vector::p_seatalk_data;
gnss_data_t* ww_vector::p_gnss_data;
correction_data_t* ww_vector::p_correction_data;

double d_rudder_coefficient_a;
double d_rudder_coefficient_b;
double d_rudder_coefficient_c;

// Intermediate results.

double d_hrm; // Heading relative to magnetic north, degrees.
double d_hrn; // Heading relative to true north, degrees.
double d_hrt; // Heading relative to true wind, degrees.
double d_trh; // True wind relative to heading, degrees.
double d_spd; // Selected boat speed (either from Seatalk or GNSS), knots.
double d_crh; // Selected course (either from Seatalk or GNSS) relative to heading, degrees.
double d_crt; // Selected course (either from Seatalk or GNSS) relative to true wind.

radial_vector_t xd_aro; // Apparent wind radial vector relative to wind-vane offset.
radial_vector_t xd_hrm; // Radial heading relative to magnetic north.
radial_vector_t xd_hrn; // Radial heading relative to true north.
radial_vector_t xd_grh; // Radial GNSS speed/course relative to heading.

course_t e_course; // Source for course calculation.

void ww_vector::v_begin( var_display_data_t* p_var_display_data, seatalk_data_t* p_seatalk_data, gnss_data_t* p_gnss_data,
  correction_data_t* p_correction_data ) {

  ww_vector::p_var_display_data = p_var_display_data;
  ww_vector::p_seatalk_data = p_seatalk_data;
  ww_vector::p_gnss_data = p_gnss_data;
  ww_vector::p_correction_data = p_correction_data;
  d_rudder_coefficient_a = RUDDER_FORMULA_S0 + RUDDER_FORMULA_S1 - 2.0;
  d_rudder_coefficient_b = 3.0 - 2.0 * RUDDER_FORMULA_S0 - RUDDER_FORMULA_S1;
  d_rudder_coefficient_c = RUDDER_FORMULA_S0;

  rd_filter.v_begin( );
  hd_filter.v_begin( );
  aw_filter.v_begin( );
  sc_filter.v_begin( );
  gc_filter.v_begin( );

}


void ww_vector::v_display_from_instruments( ) {

//
// Calculate the display data as radial vectors in knots, degrees or feet.
// Angles are relative to heading.
// The data-flow is shown in the vectors_02.key file.
//

  xd_aro = ww_vector::x_radial_from_component( aw_filter.x_step( ww_vector::x_component_from_l_a( ww_vector::p_seatalk_data->d_aws, ww_vector::p_seatalk_data->d_aro )));
  xd_hrm = ww_vector::x_radial_from_component( hd_filter.x_step( ww_vector::x_component_from_l_a( COMPASS_RADIUS, ww_vector::p_seatalk_data->d_hrm )));
  xd_hrn = ww_vector::x_rotate( xd_hrm, (double)ww_vector::p_correction_data->s16_mrn );
  d_hrn = ww_vector::d_angle_from_radial( xd_hrn );
  ww_vector::p_var_display_data->xd_ph = ww_vector::x_radial_from_component( ww_vector::x_component_from_l_a( RUDDER_POST_Y, 180.0 ));
  ww_vector::p_var_display_data->xd_rh = ww_vector::x_reflect_quad( ww_vector::x_compress_angle( ww_vector::x_radial_from_component( rd_filter.x_step( x_component_from_l_a( RUDDER_RADIUS, ww_vector::p_seatalk_data->d_rdr )))));
  ww_vector::p_var_display_data->xd_nh = ww_vector::x_reflect_ref( xd_hrn );
  ww_vector::p_var_display_data->xd_mh = ww_vector::x_reflect_ref( xd_hrm );
  ww_vector::p_var_display_data->xd_sh = ww_vector::x_radial_from_component( sc_filter.x_step( 
	  ww_vector::x_component_from_l_a( ww_vector::p_seatalk_data->d_pws, 0.0 )));
  ww_vector::p_var_display_data->xd_gh = ww_vector::x_radial_from_component( gc_filter.x_step( ww_vector::x_component_from_l_a( ww_vector::p_gnss_data->d_sns, ww_vector::d_add_angle( ww_vector::p_gnss_data->d_grn, -d_hrn ))));
  ww_vector::p_var_display_data->d_hm = ww_vector::d_angle_from_radial( xd_hrm );
  ww_vector::p_var_display_data->xd_ah = ww_vector::x_rotate ( xd_aro, (double)ww_vector::p_correction_data->s16_orh );
  ww_vector::p_var_display_data->xd_th = ww_vector::x_subtract_radial( ww_vector::p_var_display_data->xd_ah, ww_vector::p_var_display_data->xd_sh );
  for( int8_t s8_index = 0; s8_index <= HULL_POINTS_BOUND; s8_index++ ) {
	  ww_vector::p_var_display_data->axd_hh[ s8_index ] = ww_vector::x_radial_from_component( ww_vector::x_component_from_x_y( (double)as8_hull[ 2 * s8_index ], (double)as8_hull[ 2 * s8_index + 1 ]));
  };
  d_trh = ww_vector::d_angle_from_radial( ww_vector::p_var_display_data->xd_th );
  d_hrt = -d_trh;
  ww_vector::p_var_display_data->d_gh = ww_vector::d_angle_from_radial( ww_vector::p_var_display_data->xd_gh );
  #if VMG_FROM_HDG
    d_crh = ww_vector::d_angle_from_radial( ww_vector::p_var_display_data->xd_sh );
    d_spd = ww_vector::d_length_from_radial( ww_vector::p_var_display_data->xd_sh );
  #else // VMG from course
    d_crh = ww_vector::d_angle_from_radial( ww_vector::p_var_display_data->xd_gh );
    d_spd = ww_vector::d_length_from_radial( ww_vector::p_var_display_data->xd_gh );
  #endif
  d_crt = ww_vector::d_add_angle( d_crh, d_hrt );
  ww_vector::p_var_display_data->d_vt = ww_vector::d_projection( d_spd, d_crt );
  ww_vector::p_var_display_data->d_tn = ww_vector::d_add_angle( d_trh, d_hrn );
  ww_vector::p_var_display_data->b_up = (( d_crt >= -90.0 ) && ( d_crt <= 90.0 ));
  ww_vector::p_var_display_data->xd_vh.l = ww_vector::p_var_display_data->d_vt;
  ww_vector::p_var_display_data->xd_vh.a = d_trh;
  ww_vector::p_var_display_data->pac_utc = ww_vector::p_gnss_data->pac_utc;

}



radial_vector_t ww_vector::x_reflect_ref( radial_vector_t x_vector ) {

//
// Return the radial vector that is the mirror image of the input vector about the reference axis.
//

radial_vector_t x_vector_1;

  x_vector_1.l = x_vector.l;
  x_vector_1.a = - x_vector.a;

  return x_vector_1;

}



radial_vector_t ww_vector::x_reflect_quad( radial_vector_t x_vector ) {

//
// Return the radial vector that is the mirror image of the input vector about the quadrature axis.
//

radial_vector_t x_vector_1;

  x_vector_1.l = x_vector.l;
  x_vector_1.a = ww_vector::d_normalize_angle( 180.0 - x_vector.a );

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

  double d_crt = d_add_angle( ww_vector::p_var_display_data->d_gh, -d_trh );

  if( d_crt > -90.0 && d_crt < 90.0 ) return true; else return false;

}



double ww_vector::d_normalize_angle( double d_a ) {

  if( d_a <= -180.0 ) return d_a + 360.0;
  else if( d_a > 180.0 ) return d_a - 360.0;
  else return d_a;

}
