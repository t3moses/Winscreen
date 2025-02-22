
#include "ww_constants.h"
#include "ww_types.h"
#include "ww_filter.h"

// Source ... https://www.earlevel.com/main/2011/01/02/biquad-formulas/

static const double ad_fc[ FC_INDEX_BOUND + 1 ] = { 0.02, 0.05, 0.1, 0.2, 0.5, 1.0 };

// The following variables are class variables.
// And v_configure is a class method.
// So, the filter parameters are common to all instances.
// v_begin and x_step are instance functions that make use of the class variables.

double ww_filter::d_s;
double ww_filter::d_q;
double ww_filter::d_k;
double ww_filter::d_norm;
double ww_filter::d_a0;
double ww_filter::d_a1;
double ww_filter::d_a2;
double ww_filter::d_b1;
double ww_filter::d_b2;

ww_filter::ww_filter( ) {}

void ww_filter::v_configure( int16_t s16_fc, double d_q ) {

  ww_filter::d_s = HMI_UPDATE_RATE / ad_fc[ s16_fc ]; // Samples per cycle.  
  ww_filter::d_q = d_q;
  ww_filter::d_k = tan( PI / ww_filter::d_s );
  ww_filter::d_norm = 1.0 / (1.0 + ww_filter::d_k / ww_filter::d_q + ww_filter::d_k * ww_filter::d_k );
  ww_filter::d_a0 = ww_filter::d_k * ww_filter::d_k * ww_filter::d_norm;
  ww_filter::d_a1 = 2.0 * ww_filter::d_a0;
  ww_filter::d_a2 = ww_filter::d_a0;
  ww_filter::d_b1 = 2.0 * ( ww_filter::d_k * ww_filter::d_k - 1.0 ) * ww_filter::d_norm;
  ww_filter::d_b2 = ( 1.0 - ww_filter::d_k / ww_filter::d_q + ww_filter::d_k * ww_filter::d_k ) * ww_filter::d_norm;

}

void ww_filter::v_begin( ) {

  x_z1.x = 0.0;
  x_z1.y = 0.0;
  x_z2.x = 0.0;
  x_z2.y = 0.0;

}



component_vector_t ww_filter::x_step( component_vector_t x_in ) {

  component_vector_t x_out;

  double d_out_x = x_in.x * ww_filter::d_a0 + x_z1.x;
  double d_out_y = x_in.y * ww_filter::d_a0 + x_z1.y;

  x_z1.x = x_in.x * ww_filter::d_a1 + x_z2.x - ww_filter::d_b1 * d_out_x;
  x_z2.x = x_in.x * ww_filter::d_a2 - ww_filter::d_b2 * d_out_x;

  x_z1.y = x_in.y * ww_filter::d_a1 + x_z2.y - ww_filter::d_b1 * d_out_y;
  x_z2.y = x_in.y * ww_filter::d_a2 - ww_filter::d_b2 * d_out_y;

  x_out.x = d_out_x;
  x_out.y = d_out_y;

  return x_out;

}



double ww_filter::d_fc_from_index( uint16_t s16_fc ) {

uint16_t s16_index;

  s16_index = ( s16_fc < 0 ) ? 0: s16_fc;
  s16_index = ( s16_fc > FC_INDEX_BOUND ) ? FC_INDEX_BOUND: s16_fc;
  return ad_fc[ s16_index ];

}