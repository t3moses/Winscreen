
#include "ww_types.h"
#include "ww_constants.h"
#include "ww_gnss.h"
#include <driver/i2c.h>



ww_gnss::ww_gnss() {}

void ww_gnss::v_begin( var_display_data_t* p_var_display_data, gnss_data_t* p_gnss_data ) {

// Make library-global copies of project-global pointers.

  ww_gnss::p_gnss_data = p_gnss_data;
  ww_gnss::p_var_display_data = p_var_display_data;

// Configure the I2C API.
  
  i2c_config_t i2c_configuration;
  i2c_configuration.mode = I2C_MODE_MASTER;
  i2c_configuration.sda_io_num = SDA_PIN;
  i2c_configuration.scl_io_num = SCL_PIN;
  i2c_configuration.sda_pullup_en = GPIO_PULLUP_ENABLE;
  i2c_configuration.scl_pullup_en = GPIO_PULLUP_ENABLE;
  i2c_configuration.master.clk_speed = I2C_CLOCK_FREQ;
  i2c_configuration.clk_flags = 0;
  i2c_param_config( I2C_NUM_0, &i2c_configuration );
  i2c_driver_install( I2C_NUM_0, I2C_MODE_MASTER, 0, 0, 0 );

// Initialize the index to the array of fixes
// and the heartbeat status.

  s16_current_fix_index = 0;
  p_var_display_data->s8_heart_beat = 0;

}


void ww_gnss::v_x_cr_from_gnss( ) {

// Input a chunk of GNSS data.

  v_nmea_from_gnss( );

// If the chunk completes the data for a fix ...

  if( b_fix_from_gxgll( )) {

// Toggle the heartbeat status.

    p_var_display_data->s8_heart_beat ^= 0x01;

    double d_fix_hours = ww_gnss::d_utc_hours_from_pac( pac_tim );
    if( d_fix_hours != ax_fix_series[ s16_current_fix_index ].d_utc_hours ) {

// Only if the fix is new, increment s16_current_fix_index mod FIX_SERIES_INDEX_MAX.
// And add the fix to the series of fixes, deleting the oldest one.

      s16_previous_fix_index = s16_current_fix_index;
      s16_current_fix_index = ( s16_current_fix_index < FIX_SERIES_INDEX_MAX ) ? ( s16_current_fix_index + 1 ) : 0;

      ax_fix_series[ s16_current_fix_index ].d_utc_hours = ww_gnss::d_utc_hours_from_pac( pac_tim );
      ax_fix_series[ s16_current_fix_index ].d_latitude_minutes = ww_gnss::d_lat_minutes_from_pac( pac_lat, pac_n_s );
      ax_fix_series[ s16_current_fix_index ].d_longitude_minutes = ww_gnss::d_lon_minutes_from_pac( pac_lon, pac_w_e );

// Use the fix data to calculate the north and east components of boat speed.

      ww_gnss::v_x_cr_from_afix_series( );

      ww_gnss::v_utc_time_from_pac( pac_tim );

    }
  }
}



void ww_gnss::v_x_cr_from_afix_series( ) {

// Calculate the north and east components of boat speed in knots.
// Take account of roll-over at midnight UTC and 180 degrees longitude.
// One knot is one minute of latitude per hour.

  double d_delta_time_hours = ax_fix_series[ s16_current_fix_index ].d_utc_hours - ax_fix_series[ s16_previous_fix_index ].d_utc_hours;
  d_delta_time_hours = ( d_delta_time_hours >= 0.0 )? d_delta_time_hours: d_delta_time_hours + 24.0 * 60.0;

  double d_delta_latitude_minutes = ax_fix_series[ s16_current_fix_index ].d_latitude_minutes - ax_fix_series[ s16_previous_fix_index ].d_latitude_minutes;

  double d_delta_longitude_minutes = ax_fix_series[ s16_current_fix_index ].d_longitude_minutes - ax_fix_series[ s16_previous_fix_index ].d_longitude_minutes;

  if( d_delta_longitude_minutes > ( 180.0 * 60.0 )) d_delta_longitude_minutes -= ( 360.0 * 60.0 );
  else if( d_delta_longitude_minutes < ( -180.0 * 60.0 )) d_delta_longitude_minutes += ( 360.0 * 60.0 );

  p_gnss_data->x_cr.x = d_delta_longitude_minutes * cos(( RADIANS_FROM_DEGREES / 60.0 ) * ax_fix_series[ s16_current_fix_index ].d_latitude_minutes ) / d_delta_time_hours;
  p_gnss_data->x_cr.y = d_delta_latitude_minutes / d_delta_time_hours;

}



void ww_gnss::v_utc_time_from_pac( const char* pac_tim ) {

  for( uint8_t u8_index = 0; u8_index <= 5; u8_index++ ) {
    p_var_display_data->ac_utc[ u8_index ] = *( pac_tim + u8_index );
  }
  p_var_display_data->ac_utc[ 6 ] = '\0';

}



void ww_gnss::v_nmea_from_gnss( ) {
 
// Fill au8_nmea[ ] with NMEA_INDEX_MAX + 1 characters from the gnss module.

  i2c_cmd_handle_t cmd1 = i2c_cmd_link_create();
  i2c_master_start( cmd1 );
  i2c_master_write_byte( cmd1, ( GNSS_ADDRESS << 1 ) | I2C_MASTER_READ, true );
  
  for( uint8_t u8_index = 0; u8_index <= ( NMEA_INDEX_MAX - 1 ); u8_index++ ) {
    i2c_master_read_byte( cmd1, pau8_nmea + u8_index, I2C_MASTER_ACK );    
  }

  i2c_master_read_byte( cmd1, pau8_nmea + NMEA_INDEX_MAX, I2C_MASTER_NACK );    

  i2c_master_stop( cmd1 );
  i2c_master_cmd_begin( I2C_NUM_0, cmd1, I2C_TIMEOUT_TICKS );
  i2c_cmd_link_delete( cmd1 );

}



bool ww_gnss::b_fix_from_gxgll(  ) {

//
// Parse the NMEA data for the $GxGGA datagram fix fields,
// including UTC time, latitude, N/S indicator, longitude and E/W
// indicator.   These are stored in character arrays au8_tim[ ], 
// au8_lat[ ], au8_n_s[ ], au8_lat[ ] and au8_n_s[ ].
//

uint8_t u8_nmea;
bool b_valid;
static uint8_t u8_state = 0;
uint8_t u8_nmea_index;
static uint8_t u8_field_index;

  u8_nmea_index = 0;

  while( true ) {
    
    u8_nmea = (char)au8_nmea[ u8_nmea_index ];

    switch ( u8_state ) {
    case 0:
      if( u8_nmea == '$' ) u8_state = 1;
      break;
    case 1:
      if( u8_nmea == 'G' ) u8_state = 2;
      else u8_state = 0;
      break;
    case 2:
      u8_state = 3;
      break;
    case 3:
      if( u8_nmea == 'G' ) u8_state = 4;
      else u8_state = 0;
      break;
    case 4:
      if( u8_nmea == 'G' ) u8_state = 5;
      else u8_state = 0;
      break;
    case 5:
      if( u8_nmea == 'A' ) u8_state = 6;
      else u8_state = 0;
      break;
    case 6:
      if( u8_nmea == ',' ) { b_valid = false; u8_field_index = 0; u8_state = 7; }
      break;
    case 7:
      if( u8_nmea == ',' ) { ac_tim[ u8_field_index ] = '\0'; u8_field_index = 0; u8_state = 8; }
      else { ac_tim[ u8_field_index ] = u8_nmea; u8_field_index++; }
      break;
    case 8:
      if( u8_nmea == ',' ) { ac_lat[ u8_field_index ] = '\0'; u8_field_index = 0; u8_state = 9; }
      else { ac_lat[ u8_field_index ] = u8_nmea; u8_field_index++; }
      break;
    case 9:
      if( u8_nmea == ',' ) { ac_n_s[ u8_field_index ] = '\0'; u8_field_index = 0; u8_state = 10; }
      else { ac_n_s[ u8_field_index ] = u8_nmea; u8_field_index++; }
      break;
    case 10:
      if( u8_nmea == ',' ) { ac_lon[ u8_field_index ] = '\0'; u8_field_index = 0; u8_state = 11; }
      else { ac_lon[ u8_field_index ] = u8_nmea; u8_field_index++; }
      break;
    case 11:
      if( u8_nmea == ',' ) { ac_w_e[ u8_field_index ] = '\0';  b_valid = true; u8_state = 0; }
      else { ac_w_e[ u8_field_index ] = u8_nmea; u8_field_index++; }
      break;
    }          

    if( u8_nmea_index >= NMEA_INDEX_MAX ) return b_valid;
    
    u8_nmea_index++;

  }
}



double ww_gnss::d_utc_hours_from_pac( char* pac_tim ) {

//
// Convert the UTC time character array to hours.
//

  double d_h = (double)( *( pac_tim + 0 ) - '0' ) * 10.0 +
    (double)( *( pac_tim + 1 ) - '0' );

  double d_m = (double)( *( pac_tim + 2 ) - '0' ) * 10.0 +
    (double)( *( pac_tim + 3 ) - '0' );

  double d_s = ww_gnss::d_double_from_pac( pac_tim + 4 );

  return ((( d_s / 60.0 + d_m )) / 60.0 ) + d_h; 

}



double ww_gnss::d_lat_minutes_from_pac( char* pac_lat, char* pac_n_s ) {

//
// Convert the latitude and north-south indicator character arrays to minutes.
//

  double d_d = (double)( *( pac_lat + 0 ) - '0' ) * 10.0 +
    (double)( *( pac_lat + 1 ) - '0' );

  double d_m = ww_gnss::d_double_from_pac( pac_lat + 2 );

  double d_mins = d_d * 60.0 + d_m;

  if( *pac_n_s == 'S' ) d_mins *= -1.0;

  return d_mins;

}



double ww_gnss::d_lon_minutes_from_pac( char* pac_lon, char* pac_w_e ) {

//
// Convert the longitude and west-east indicator character arrays to minutes.
//

  double d_d = (double)( *( pac_lon + 0 ) - '0' ) * 100.0 +
    (double)( *( pac_lon + 1 ) - '0' ) * 10.0 +
    (double)( *( pac_lon + 2 ) - '0' );

  double d_m = ww_gnss::d_double_from_pac( pac_lon + 3 );

  double d_mins = d_d * 60.0 + d_m;

  if( *pac_w_e == 'W' ) d_mins *= -1.0;

  return d_mins;

}



double ww_gnss::d_double_from_pac( char* pac_char ) {

//
// Return the value of a char array as a double.
//

bool b_decimal = false;
double d_integer_part = 0.0;
double d_divisor = 1.0;
double d_fractional_part = 0.0;

// Walk through the character array, stopping once the terminator is reached.
// Convert each character to a uint8_t.  Add its value to the intermediate value,
// taking account of its position.

  for ( uint8_t u8_index = 0; u8_index <= FIELD_INDEX_BOUND; u8_index++ ) {

    switch ( *( pac_char + u8_index )) {
    case '\0':
      return d_integer_part + d_fractional_part;
      break;
    case '.':
      b_decimal = true;
      break;
    default:
      if( b_decimal ) {
        d_divisor *= 10.0;
        d_fractional_part += (double)( *( pac_char + u8_index ) - '0' ) / d_divisor;
      }
      else {
        d_integer_part *= 10.0;
        d_integer_part += (double)( *( pac_char + u8_index ) - '0' );
      }
      break;
    }
  }
  return 0.0;
}
