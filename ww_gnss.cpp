
#include "ww_types.h"
#include "ww_constants.h"
#include "ww_gnss.h"
#include <driver/i2c.h>

uint8_t aus_nmea[ NMEA_INDEX_BOUND + 1 ];
uint8_t* paus_nmea = aus_nmea;

uint8_t aus_sns[ FIELD_INDEX_BOUND + 1 ];
uint8_t* paus_sns = aus_sns;

uint8_t aus_crn[ FIELD_INDEX_BOUND + 1 ];
uint8_t* paus_crn = aus_crn;

char ac_utc[ FIELD_INDEX_BOUND + 1 ];
char* pac_utc = ac_utc;



ww_gnss::ww_gnss() {}

void ww_gnss::v_begin( gnss_data_t* p_gnss_data ) {

// Make library-global copies of project-global pointers.

  ww_gnss::p_gnss_data = p_gnss_data;

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

  for(uint8_t u8_index = 0; u8_index < FIELD_INDEX_BOUND; u8_index++ ) {
    ac_utc[ u8_index ] = '-';
  }
  ac_utc[ FIELD_INDEX_BOUND ] = '\0';

}



void ww_gnss::v_sns_crn_utc_from_gnss( ) {

  v_nmea_from_gnss( );

// Convert aus_sns[ ] and aus_crn[ ] to d_sns and d_grn.

  double d_sns = d_double_from_paus_number( paus_sns );
  double d_grn = d_double_from_paus_number( paus_crn );

  if( d_sns < SNS_THRESHOLD ) { d_sns = 0.0; d_grn = 0.0; }

  if( b_sns_from_nmea( )) {

    ww_gnss::p_gnss_data->d_sns = d_sns;

  }

  if( b_crn_from_nmea( )) {
    
    ww_gnss::p_gnss_data->d_grn = d_grn;

  }

  if( b_utc_from_nmea( )) {
    
    ww_gnss::p_gnss_data->pac_utc = pac_utc;

  }
}



void ww_gnss::v_nmea_from_gnss( ) {
 
// Fill aus_nmea[ ] with NMEA_INDEX_MAX + 1 characters from the gnss module.

  i2c_cmd_handle_t cmd1 = i2c_cmd_link_create();
  i2c_master_start( cmd1 );
  i2c_master_write_byte( cmd1, ( GNSS_ADDRESS << 1 ) | I2C_MASTER_READ, true );
  
  for( uint8_t us_index = 0; us_index <= ( NMEA_INDEX_MAX - 1 ); us_index++ ) {
    i2c_master_read_byte( cmd1, paus_nmea + us_index, I2C_MASTER_ACK );    
  }

  i2c_master_read_byte( cmd1, paus_nmea + NMEA_INDEX_MAX, I2C_MASTER_NACK );    

  i2c_master_stop( cmd1 );
  i2c_master_cmd_begin( I2C_NUM_0, cmd1, I2C_TIMEOUT_TICKS );
  i2c_cmd_link_delete( cmd1 );

}



bool ww_gnss::b_sns_from_nmea(  ) {

//
// Parse the NMEA data for the $GxVTG datagram.
// Extract the speed field.
//

uint8_t us_nmea;
bool b_valid;
static uint8_t state = 0;
uint8_t us_nmea_index;
static uint8_t us_field_index;

  us_nmea_index = 0;

  while( true ) {
    
    us_nmea = aus_nmea[ us_nmea_index ];
    
    switch ( state ) {
    case 0:
      if( us_nmea == '$' ) state = 1;
      break;
    case 1:
      if( us_nmea == 'G' ) state = 2;
      else state = 0;
      break;
    case 2:
      state = 3;
      break;
    case 3:
      if( us_nmea == 'V' ) state = 4;
      else state = 0;
      break;
    case 4:
      if( us_nmea == 'T' ) state = 5;
      else state = 0;
      break;
    case 5:
      if( us_nmea == 'G' ) state = 6;
      else state = 0;
      break;
    case 6:
      if( us_nmea == ',' ) state = 7;
      break;
    case 7:
      if( us_nmea == ',' ) state = 8;
      break;
    case 8:
      if( us_nmea == ',' ) state = 9;
      break;
    case 9:
      if( us_nmea == ',' ) state = 10;
      break;
    case 10:
      if( us_nmea == ',' ) { b_valid = false; us_field_index = 0; state = 11; }
      break;
    case 11:
      if( us_nmea == ',' ) { aus_sns[ us_field_index ] = '\0'; b_valid = true; state = 0; }
      else { aus_sns[ us_field_index ] = us_nmea; us_field_index++; }
      break;
    }          

    if( us_nmea_index >= NMEA_INDEX_MAX ) return b_valid;
    
    us_nmea_index++;

  }
}



bool ww_gnss::b_crn_from_nmea(  ) {

//
// Parse the NMEA data for the $GxVTG datagram.
// Extract the course field.
//

uint8_t us_nmea;
bool b_valid;
static uint8_t state = 0;
uint8_t us_nmea_index;
static uint8_t us_field_index;

  us_nmea_index = 0;
  
  while( true ) {
    
    us_nmea = aus_nmea[ us_nmea_index ];

    switch ( state ) {
    case 0:
      if( us_nmea == '$' ) state = 1;
      break;
    case 1:
      if( us_nmea == 'G' ) state = 2;
      else state = 0;
      break;
    case 2:
      state = 3;
      break;
    case 3:
      if( us_nmea == 'V' ) state = 4;
      else state = 0;
      break;
    case 4:
      if( us_nmea == 'T' ) state = 5;
      else state = 0;
      break;
    case 5:
      if( us_nmea == 'G' ) state = 6;
      else state = 0;
      break;
    case 6:
      if( us_nmea == ',' ) { b_valid = false; us_field_index = 0; state = 7; }
      break;
    case 7:
      if( us_nmea == ',' ) { aus_crn[ us_field_index ] = '\0'; b_valid = true; state = 0; }
      else { aus_crn[ us_field_index ] = us_nmea; us_field_index++; }
      break;
    }          

    if( us_nmea_index >= NMEA_INDEX_MAX ) return b_valid;
    
    us_nmea_index++;

  }
}



bool ww_gnss::b_utc_from_nmea( ) {

//
// Parse the NMEA data for the $GxGGA datagram.
// Extract the utc field.
//

uint8_t u8_nmea;
bool b_valid;
static uint8_t u8_state = 0;
uint8_t u8_nmea_index;
static uint8_t u8_field_index;

  u8_nmea_index = 0;
  
  while( true ) {
    
    u8_nmea = aus_nmea[ u8_nmea_index ];

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
      if(( u8_nmea == ',' ) || ( u8_nmea == '.' )) { ac_utc[ u8_field_index ] = '\0'; b_valid = true; u8_state = 0; }
      else { ac_utc[ u8_field_index ] = u8_nmea; u8_field_index++; }
      break;
  }          

    if( u8_nmea_index >= NMEA_INDEX_MAX ) return b_valid;
    
    u8_nmea_index++;

  } 
}


double ww_gnss::d_double_from_paus_number( uint8_t* paus_number ) {

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

  for ( uint8_t us_index = 0; us_index <= FIELD_INDEX_BOUND; us_index++ ) {

    switch ( *( paus_number + us_index )) {
    case '\0':
      return d_integer_part + d_fractional_part;
      break;
    case '.':
      b_decimal = true;
      break;
    default:
      if( b_decimal ) {
        d_divisor *= 10.0;
        d_fractional_part += (double)( *( paus_number + us_index ) - '0' ) / d_divisor;
      }
      else {
        d_integer_part *= 10.0;
        d_integer_part += (double)( *( paus_number + us_index ) - '0' );
      }
      break;
    }
  }
  return 0.0;
}



double ww_gnss::normal( double d_abnormal ) {

//
// Cast an angle in degrees into the range -179.9 .. +180.0.
//

  if( d_abnormal <= -180.0 ) return d_abnormal + 360.0;
  else if( d_abnormal > 180.0 ) return d_abnormal - 360.0;
  else return d_abnormal;

}