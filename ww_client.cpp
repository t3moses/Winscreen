
#include "ww_constants.h"
#include "ww_types.h"
#include "ww_client.h"
#include "ww_url.h"

ww_client* ww_client::ap_client_list[ CLIENT_BOUND + 1 ]; // Array containing pointers to client instances.

ww_client::ww_client( ) { // Instance constructor.

  ww_client::s8_socket = DISCONNECTED;

}



void ww_client::v_begin() {

// Initialize each client.  Only client 0 has admin privileges.

  ww_client::ap_client_list[ 0 ] = new ww_client( );
  ww_client::ap_client_list[ 1 ] = new ww_client( );
  ww_client::ap_client_list[ 2 ] = new ww_client( );
  ww_client::ap_client_list[ 3 ] = new ww_client( );
  ww_client::ap_client_list[ 4 ] = new ww_client( );
  ww_client::ap_client_list[ 5 ] = new ww_client( );

}



void ww_client::v_initialize_client( int8_t s8_socket ) {

// Find the first client list entry whose socket is DISCONNECTED.
// Initialize the corresponding client.

  for( int8_t s8_index = 0; s8_index <= CLIENT_BOUND; s8_index++ ){

    if( ww_client::ap_client_list[ s8_index ]->s8_socket == DISCONNECTED ) {

      if( s8_index == 0 ) ww_client::ap_client_list[ s8_index ]->e_role = ADMIN;
      else ww_client::ap_client_list[ s8_index ]->e_role = USER;
      ww_client::ap_client_list[ s8_index ]->s8_socket = s8_socket;
      ww_client::ap_client_list[ s8_index ]->s8_this = 0;
      ww_client::ap_client_list[ s8_index ]->s8_next = 1;
      return;

    }
  }
}



String ww_client::s_next_from_client( int8_t s8_client ) {

  return ww_url::s_url_from_page_and_client( ww_client::ap_client_list[ s8_client ]->s8_next, s8_client );

}



void ww_client::v_advance_client( int8_t s8_socket, char ac_value[] ) {

  int8_t s8_client;
  int8_t s8_this;
  int8_t s8_next;

printf("ac_value: %s\n", ac_value );

  s8_client = ww_url::s8_client_from_url( ac_value );
  ww_client::ap_client_list[ s8_client ]->s8_socket = s8_socket;
  s8_this = ww_url::s8_this_from_url( ac_value );
  ww_client::ap_client_list[ s8_client ]->s8_this = s8_this;

  if( ww_client::ap_client_list[ s8_client ]->e_role == ADMIN ) {
    s8_next = ( s8_this < PAGE_BOUND ) ? s8_this + 1 : 1;
  }
  else s8_next = ( s8_this < PAGE_BOUND - 2 ) ? s8_this + 1 : 1;
  
  ww_client::ap_client_list[ s8_client ]->s8_next = s8_next;

}

int8_t ww_client::s8_client_from_socket( int8_t s8_socket ) {

// Return the index of the client connected to socket s8_socket.

  for( int8_t s8_index = 0; s8_index <= CLIENT_BOUND; s8_index++ ){

    if( ww_client::ap_client_list[ s8_index ]->s8_socket == s8_socket ) return s8_index;

  }
  return DISCONNECTED;
}
