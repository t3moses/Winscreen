
#include "ww_constants.h"
#include "ww_types.h"
#include "ww_decode.h"

uint32_t* ww_decode::pau32_transition; // Pointer to global array of bit transitions.
uint16_t* ww_decode::pu16_transition_index_max; // Pointer to global max index of transitions array.
datagram_type_t ww_decode::datagram_type; // Enumeration value of PWS, HRM, AWS, ARH.
uint32_t ww_decode::u32_elapsed_time; // Time difference between the first and last transitions in a datagram.
uint16_t ww_decode::u16_run[ TRANSITION_INDEX_BOUND + 1 ]; // Arry of run lengths.
uint16_t ww_decode::u16_run_index_max;
uint16_t ww_decode::u16_frame[ FRAME_INDEX_BOUND + 1 ]; // Array of datagram frames.
uint16_t ww_decode::u16_frame_index_max;
double ww_decode::d_datum_value;

seatalk_data_t* ww_decode::p_seatalk_data;

uint32_t ww_decode::u32_us_per_bit;
uint32_t ww_decode::u32_us_per_min_target_datagram;
uint32_t ww_decode::u32_us_per_max_target_datagram;



void ww_decode::v_begin( uint32_t* pau32_transition, uint16_t* pu16_transition_index_max, seatalk_data_t* p_seatalk_data ) {

// Make library-global copies of project-global pointers.

  ww_decode::pau32_transition = pau32_transition;
  ww_decode::pu16_transition_index_max = pu16_transition_index_max;
  ww_decode::p_seatalk_data = p_seatalk_data;

  ww_decode::u32_us_per_bit = 1000000 / BAUD_RATE; // Microseconds per bit.

// The duration of a target datagram in microseconds lies between these two limits.

  ww_decode::u32_us_per_min_target_datagram = (uint32_t)(((double)( MIN_FRAMES_PER_TARGET_DATAGRAM * BITS_PER_FRAME ) - 1.5 ) * ww_decode::u32_us_per_bit );
  ww_decode::u32_us_per_max_target_datagram = (uint32_t)(((double)( MAX_FRAMES_PER_TARGET_DATAGRAM * BITS_PER_FRAME ) - 0.5 ) * ww_decode::u32_us_per_bit );

}



void ww_decode::v_datum_from_transitions( ) {

// Check that u16_transition_index_max is odd,
// i.e there are an even number of transitions, and so
// the Seatalk bus was in the idle condition when data captured commenced.

  if( !ww_decode::is_odd( )) return;

// Check that the duration of the datagram is no shorter than the shortest target datagram and
// no longer than the longest target datagram.

  if( !ww_decode::is_target_length( )) return;

// Decode datagram timings to runs by subtracting u32_get_transition_time[ 0 ] and multiplying by BAUD_RATE / 1 000 000.

  ww_decode::v_runs_from_transitions( );

// Decode runs to frames by loading runs of 0s and 1s into frames, completing the last frame wth a sequence of 1s.

  v_frames_from_runs( );

// Check frame command bits and start and stop bits.

  if( !ww_decode::is_well_formed( )) return;

// Decode frames to bytes: shift right by 1 and & with 0x00FF.

  v_bytes_from_frames( );

// Identiify the datagram as PWS, HRM, AWS, ARH.

  datagram_type = ww_decode::is_target( );

// Decode the datagram.

  switch ( datagram_type ) {
  case PWS:
    p_seatalk_data->d_pws = d_pws_from_datagram( );
    break;
  case HRM:
    p_seatalk_data->d_hrm = d_hrm_from_datagram( );
    p_seatalk_data->d_rdr = d_rdr_from_datagram( );
    break;
  case AWS:
    p_seatalk_data->d_aws = d_aws_from_datagram( );
    break;
  case ARO:
    p_seatalk_data->d_aro = d_aro_from_datagram( );
    break;
  default:
    return;
    break;
  }

}
     
bool ww_decode::is_odd( ) {

  if((( *pu16_transition_index_max ) % 2 ) == 1 ) return true;
  else return false;

}


      
bool ww_decode::is_target_length( ) {

  u32_elapsed_time = ( *( pau32_transition + *pu16_transition_index_max ) > *pau32_transition ) ?
  
    *( pau32_transition + *pu16_transition_index_max ) - *pau32_transition : // No wrap around.
    
    MAXLONG - *pau32_transition + *( pau32_transition + *pu16_transition_index_max ) + 1; // Wrap around.

  if(( u32_elapsed_time > u32_us_per_min_target_datagram ) && ( u32_elapsed_time < u32_us_per_max_target_datagram )) return true;
  else return false;

}


      
void ww_decode::v_runs_from_transitions( ) {

//
// Decode each timing interval into a run length in bits.
// Allow for jitter in the captured timings.
// Note: The number of runs is 1 less than the number of transitions.
// The length of the final run is not known at this point.
// 

  for( uint16_t u16_run_index = 0; u16_run_index < *pu16_transition_index_max; u16_run_index++ ) {

    double d_run_length = (double)( *( pau32_transition + u16_run_index + 1 ) - *( pau32_transition + u16_run_index )) * (double)BAUD_RATE / 1000000.0;

    double d_jitter = modf( d_run_length, &d_run_length ); // d_run_length becomes the interger number of bits and d_jitter becomes part of a bit

    uint16_t u16_run_length = (uint16_t)d_run_length;
   
    u16_run[ u16_run_index ] = ( d_jitter < 0.5 ) ? u16_run_length : u16_run_length + 1;

  }

  u16_run_index_max = *pu16_transition_index_max - 1; // Omitting any run of 1s after the last transition. 
  
}



      
void ww_decode::v_frames_from_runs( ) {

//
// Fit runs into frames until all runs are exhausted.
// Complete the last frame with a run of 1s.
//

uint16_t u16_frame_index = 0;
uint16_t u16_run_index = 0;
uint16_t u16_run_value = 0;
uint16_t u16_frame_bit_position;
uint16_t u16_run_bit_position;

  do { // while u16_run_index <= u16_run_index_max, process all runs.

    u16_frame[ u16_frame_index ] = 0;
    u16_frame_bit_position = 0;

    do { // while u16_frame_bit_position < BITS_PER_FRAME, fill one frame.

      u16_run_bit_position = 0;

      do { // while u16_run_bit_position < *u16_run[ u16_run_index ], add a run to the frame.

        if( u16_run_value == 1 ) u16_frame[ u16_frame_index ] = u16_frame[ u16_frame_index ] | ( 0x0001 << BITS_PER_FRAME );
        u16_frame[ u16_frame_index ] = u16_frame[ u16_frame_index ] >> 1;

        u16_run_bit_position++;
        u16_frame_bit_position++;
       
      } while ( u16_run_bit_position < u16_run[ u16_run_index ] );
 
      u16_run_value = u16_run_value ^ 0x0001; // Invert the run value.
 
      u16_run_index++; // Move to the next run, if there is one.

      if( u16_run_index > u16_run_index_max ) break;
          
    } while ( u16_frame_bit_position < BITS_PER_FRAME );

    if( u16_frame_bit_position == BITS_PER_FRAME) u16_frame_index++; // The loop terminated because the frame was not the final one.
    
  } while ( u16_run_index <= u16_run_index_max );

  while ( u16_frame_bit_position < BITS_PER_FRAME ) { // Complete the last frame with 1s.

    u16_frame[ u16_frame_index ] = u16_frame[ u16_frame_index ] | ( 0x0001 << BITS_PER_FRAME );
    u16_frame[ u16_frame_index ] = u16_frame[ u16_frame_index ] >> 1;

    u16_frame_bit_position++;

  }

  u16_frame_index_max = u16_frame_index;

}


      
bool ww_decode::is_well_formed( ) {

// Check that the command bit in the first frame is 1.

  if( !( u16_frame[ 0 ] & 0x0200 )) return false;

// Check that the command bits in all other frames are 0.

  for( uint16_t u16_frame_index = 1; u16_frame_index <= u16_frame_index_max; u16_frame_index++ ) {

      if( u16_frame[ u16_frame_index ] & 0x0200 ) return false;
    
  }

// Check that the start bits in all frames are 0.

  for( uint16_t u16_frame_index = 0; u16_frame_index <= u16_frame_index_max; u16_frame_index++ ) {

    if( u16_frame[ u16_frame_index ] & 0x0001 ) return false;
    
  }

// Check that the stop bits in all frames are 1.

  for( uint16_t u16_frame_index = 0; u16_frame_index <= u16_frame_index_max; u16_frame_index++ ) {

    if( !( u16_frame[ u16_frame_index ] & 0x0400 )) return false;
    
  }
  
  return true;
}


      
void ww_decode::v_bytes_from_frames( ) {

// Delete the start and stop bits.

  for( uint16_t u16_frame_index = 0; u16_frame_index <= u16_frame_index_max; u16_frame_index++ ) {

    u16_frame[ u16_frame_index ] = ( u16_frame[ u16_frame_index ] >> 1 ) & 0x00FF;
    
  } 
}


      
datagram_type_t ww_decode::is_target( ) {

  switch ( u16_frame[ 0 ] ) {
  case 0x0020:
    return PWS;
    break;
  case 0x009C:
    return HRM;
    break;
  case 0x0011:
    return AWS;
    break;
  case 0x0010:
    return ARO;
    break;
  default: return NOTA;
  }
}



double ww_decode::d_pws_from_datagram( ) {

//
// 20  01  XX  XX
// XXXX/10
//

  return (double)( u16_frame[ 2 ] + 256 * u16_frame[ 3 ] ) / 10.0;
  
}



double ww_decode::d_hrm_from_datagram( ) {

//
// 9C U1 VW RR
//
// The two lower bits of U * 90 ( 0..270 in steps of 90 ) +
// the six lower bits of VW * 2 ( 0.. 126 in steps of 2 ) +
// number of bits set in the two higher bits of U ( 0..3 in steps of 1 ).
//
// Adjust to return values in the range -179.. +180.
//

  double d_adjusted_hrm = (( u16_frame[ 1 ] >> 4 ) & 0x3 ) * 90.0 + ( u16_frame[ 2 ] & 0x3F ) * 2.0 + ( u16_frame[ 1 ] >> 4 & 0xC ) + ((( u16_frame[ 1 ] >> 4 ) & 0xC ) == 0xC );
  
  return ww_decode::normal( d_adjusted_hrm );
  
}



double ww_decode::d_rdr_from_datagram( ) {

//
// 9C U1 VW RR
//
// Convert RR into a double in the range -128.0..127.0.
//

  return (double)(int8_t)u16_frame[ 3 ];

}



double ww_decode::d_aws_from_datagram( ) {

//
// 11  01  XX  0Y
// (XX & 0x7F) + Y/10
//

  return (double)( u16_frame[ 2 ] & 0x7F ) + (double)( u16_frame[ 3 ] & 0x0F ) / 10.0;
    
}



double ww_decode::d_aro_from_datagram( ) {

//
// 10  01  XX  YY
// XXYY/2.
//
// Adjust to return values in the range -179 .. +180.
//

  return ww_decode::normal( (double)( 256 * u16_frame[ 2 ] + u16_frame[ 3 ] ) / 2.0 );
 
}



double ww_decode::normal( double d_abnormal ) {

  if( d_abnormal <= -180.0 ) return d_abnormal + 360.0;
  else if( d_abnormal > 180.0 ) return d_abnormal - 360.0;
  else return d_abnormal;

}
