#include "ww_constants.h"
#include "ww_url.h"

int8_t ww_url::s8_int_from_char( char* pac_in ) {

// Return the value of pac_in, interpretted as a decimal integer.

int8_t s8_out = 0;
int8_t s8_index = 0;
int8_t s8_sign;

  if( pac_in[ s8_index ] == '-' ) { s8_sign = -1; s8_index++; } else s8_sign = 1;

  while( pac_in[ s8_index ] != '\0' ) {

    s8_out = 10 * s8_out + (int8_t)pac_in[ s8_index ] - 48;
    s8_index++;

  }
  return s8_sign * s8_out;
}

String ww_url::s_string_from_int( int8_t s8_in ) {

// Return a string that is a decimal representation of s8_in.

String s_out;
char ac_out[ URL_BOUND + 1 ];

  sprintf( ac_out, "%d", s8_in );
  s_out = ac_out;
  return s_out;
}

String ww_url::s_url_from_page_and_client( int8_t s8_page, int8_t s8_client ) {

String s_out;
char ac_out[ URL_BOUND + 1 ];

  sprintf( ac_out, "/%d?client=%d", s8_page, s8_client );

  s_out = ac_out;

  return s_out;
}

bool ww_url::b_base_url( char* pac_url ) {

// Return true if ac_url[] is a base url.  Otherwise return false.

  int8_t s8_index = 0;
  int8_t s8_end_index = 0;

  while(( *( pac_url + s8_index ) != '\0' ) && ( s8_index < URL_BOUND )) {
    s8_index++;
  }
  s8_end_index = s8_index;
  if( *( pac_url + s8_end_index - 1 ) != '/' ) return false;
  else return true;
}

  int8_t ww_url::s8_from_url( char* pac_url, char c_before, char c_after ) {

// Return a portion of ac_url interpretted as a decimal.

int8_t s8_before_index;
int8_t s8_after_index;
int8_t s8_index;
int8_t s8_out = 0;

  for( s8_index = 0; s8_index <= URL_BOUND; s8_index++ ) {
    if( *( pac_url + s8_index ) == c_before ) s8_before_index = s8_index;
    if( *( pac_url + s8_index ) == c_after ) {
      s8_after_index = s8_index;
      break;
    }
  } 

  for( int8_t s8_index = ( s8_before_index + 1 ); s8_index < s8_after_index; s8_index++ ) {
    s8_out = 10 * s8_out + (int8_t)(*( pac_url + s8_index )) - 48;
  }

  return s8_out;
}

int8_t ww_url::s8_this_from_url( char* pac_url ) {
  return ww_url::s8_from_url( pac_url, '/', '?' );
}

int8_t ww_url::s8_client_from_url( char* pac_url ) {
  return ww_url::s8_from_url( pac_url, '=', '\0' );
  }
