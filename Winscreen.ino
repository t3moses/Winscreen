/*

The MIT License (MIT)
---------------------

Copyright © 2023, Tim Moses

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the “Software”),
to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES
OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH
THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

*/

// Uncomment the next line in order to print diagnostics.

#define DEBUG

// Uncomment the next two definitions in order to run with real Seatalk and GNSS data.
// Speeds must be >= 0.0.
// Angles must be between -179.9 and +180.0.
// Choose HRM and CRN such that CRN - HRM is close to MRN ( -12.8 ).  
// Otherwise the resulting CRH and VMG will be unrealistic.

// #define SEATALK_DATA_AVAILABLE
// #define GNSS_DATA_AVAILABLE

#define DEMO_PWS 5.0
#define DEMO_HRM -30.0
#define DEMO_AWS 6.0
#define DEMO_ARO -30.0
#define DEMO_BRE -2.0
#define DEMO_BRN 2.0
#define DEMO_RDR 10.0

#include "ww_constants.h"
#include "ww_types.h"
#include "ww_url.h"
#include "ww_seatalk.h"
#include "ww_gnss.h"
#include "ww_filter.h"
#include "ww_decode.h"
#include "ww_vector.h"
#include "ww_hmi.h"
#include "ww_page.h"
#include "ww_json.h"
#include "ww_storage.h"
#include <WiFi.h>
#include <WebServer.h>
#include "ww_client.h"
#include "ArduinoJson.h"
#include <WebSocketsServer.h>
#include <ESPmDNS.h>

// This section implements a WiFi access point and Web server with WebSockets.

const char* ssid = "Cariad"; // Configure client devices to join the "ssid" network.
IPAddress local_ip( 192,168,8,35 );
IPAddress gateway( 192,168,1,5 );
IPAddress subnet( 255,255,255,0 );

char ac_json_null[ JSON_BOUND + 1];
char ac_json_req_this[ JSON_BOUND + 1];
char ac_json_resp_next[ JSON_BOUND + 1];
char ac_json_const_01[ JSON_BOUND + 1];
char ac_json_const_02[ JSON_BOUND + 1];
char ac_json_const_03[ JSON_BOUND + 1];
char ac_json_const_04[ JSON_BOUND + 1];
char ac_json_var[ JSON_BOUND + 1];

char* pac_json_null = ac_json_null;
char* pac_json_req_this = ac_json_req_this;
char* pac_json_resp_next = ac_json_resp_next;
char* pac_json_const_01 = ac_json_const_01;
char* pac_json_const_02 = ac_json_const_02;
char* pac_json_const_03 = ac_json_const_03;
char* pac_json_const_04 = ac_json_const_04;
char* pac_json_var = ac_json_var;

uint32_t au32_transition[ TRANSITION_INDEX_BOUND + 1 ]; // Array of Seatalk logic transition timings.
uint32_t* pau32_transition = au32_transition;

uint16_t u16_transition_index_max; // Maximum index of the transition array for the latest datagram.
uint16_t* pu16_transition_index_max = &u16_transition_index_max;

seatalk_data_t seatalk_data; // Structure containing all raw Seatalk sensor data.
seatalk_data_t* p_seatalk_data = &seatalk_data;

gnss_data_t gnss_data; // Structure containing all raw GNSS data.
gnss_data_t* p_gnss_data = &gnss_data;

correction_data_t correction_data; // Structure containing correction data.
correction_data_t* p_correction_data = &correction_data;

var_display_data_t var_display_data; // Structure containing all processed data.
var_display_data_t* p_var_display_data = &var_display_data;

int8_t s8_updates_per_heart_beat;
int8_t s8_update_count;
int8_t s8_debounce_count;

// Instantiate the library instances.

ww_seatalk seatalk;
ww_gnss gnss;
ww_filter filter;
ww_hmi hmi;
ww_json json;
WebServer server(80);
WebSocketsServer webSocket = WebSocketsServer(81);

void setup() {

  Serial.begin( 115200 );
  delay( 1000 );

// Set up test points.

  pinMode( TEST1, OUTPUT );
  pinMode( TEST2, OUTPUT );
  digitalWrite( TEST1, LOW );
  digitalWrite( TEST2, LOW );

// Set up a wifi access point.

  printf( WiFi.softAPConfig( local_ip, gateway, subnet ) ? "SoftAP CONFIG SUCCESS\n" : "SoftAP CONFIG FAIL\n" );
  delay(100);
  printf( WiFi.softAP( ssid ) ? "SoftAP START SUCCESS\n" : "SoftAP START FAILl\n" );
  delay(100);

  ww_client::v_begin( );
  ww_page::v_begin( );
  ww_decode::v_begin( pau32_transition, pu16_transition_index_max, &seatalk_data );
  ww_storage::v_begin( p_correction_data );
  ww_vector::v_begin( p_var_display_data, p_seatalk_data, p_gnss_data, p_correction_data );

  gnss.v_begin( &var_display_data, &gnss_data );
  hmi.v_begin();
  json.v_begin( p_var_display_data, p_correction_data );
  MDNS.begin( "winscreen" );
  seatalk.v_begin( );
  server.begin();
  webSocket.begin();

// Register the Web server and WebSocket callback functions.
  
  server.on("/", handle_onConnect); // Page 0 HTML.
  server.on("/"+ww_url::s_string_from_int( 1 ), handle_01); // Page 1 HTML.
  server.on("/"+ww_url::s_string_from_int( 2 ), handle_02); // Page 2 HTML.
  server.on("/"+ww_url::s_string_from_int( 3 ), handle_03); // Page 3 HTML.
  server.on("/"+ww_url::s_string_from_int( 4 ), handle_04); // Page 4 HTML.
  server.on("/"+ww_url::s_string_from_int( 5 ), handle_05); // Page 5 HTML.
  server.on("/"+ww_url::s_string_from_int( 6 ), handle_06); // Page 6 HTML.
  server.on("/splash", handle_splash); // Splash SVG.
  server.onNotFound(handle_NotFound);
  webSocket.onEvent(webSocketEvent); // Register a call-back for handling WebSocket events.

// Register the Web server using http over tcp on port 80.
  
  MDNS.addService( "_http", "_tcp", 80 );

  seatalk.v_capture_datagram( ); // Capture the first datagram.
  seatalk.v_clear_availability( ); // Reset the datagram-available flag.

  s8_updates_per_heart_beat = int8_t( HMI_UPDATE_RATE / HEART_BEAT_RATE );
  s8_update_count = 0;
  var_display_data.s8_heart_beat = 0;

  s8_debounce_count = DEBOUNCE_COUNT;

// Fill the non-variable JSON arrays.

  json.v_json_null( pac_json_null );
  json.v_json_req_this( pac_json_req_this );
  json.v_json_const_01( pac_json_const_01 );
  json.v_json_const_02( pac_json_const_02 );
  json.v_json_const_03( pac_json_const_03 );
  json.v_json_const_04( pac_json_const_04 );

}



void loop() {

// Start by processing Seatalk datagrams.

  if( seatalk.b_datagram_available( )) {
    
    u16_transition_index_max = seatalk.u16_get_transition_index( );
    
    for( int16_t s16_index = 0; s16_index <= u16_transition_index_max; s16_index++ ) {

      au32_transition[ s16_index ] = seatalk.u32_get_transition_time( s16_index );

    }

    ww_decode::v_datum_from_transitions( ); // Populate the seatalk_data structure.

    seatalk.v_capture_datagram( ); // Capture the next datagram.
    seatalk.v_clear_availability( ); // Reset the datagram available flag.

  } 

// Next, update the HMI.

  if( hmi.b_update_now( )) {

// Operate the heartbeat indicator.

    s8_update_count++;
 
    if( s8_update_count >= s8_updates_per_heart_beat ) {

      var_display_data.s8_heart_beat ^= 0x01;
      s8_update_count = 0;

    }

// Lockout HMI input for debounce purposes.

    s8_debounce_count = ( s8_debounce_count > 0 ) ? s8_debounce_count - 1 : 0;

// Update the filters' coefficients if the admin has altered the bandwidth.

    filter.v_configure( correction_data.s16_fc, QQ );

// Get unfiltered north and east components of boat speed from gnss data.

    gnss.v_x_cr_from_gnss( );
    
#ifndef SEATALK_DATA_AVAILABLE

// Overwrite data with hard-coded demonstration values.

    seatalk_data.d_pws = DEMO_PWS;  
    seatalk_data.d_hrm = DEMO_HRM;
    seatalk_data.d_aws = DEMO_AWS;
    seatalk_data.d_aro = DEMO_ARO;
    seatalk_data.d_rdr = DEMO_RDR;
    
#endif

#ifndef GNSS_DATA_AVAILABLE

// Overwrite data with hard-coded demonstration values.

    gnss_data.x_cr.x = DEMO_BRE;
    gnss_data.x_cr.y = DEMO_BRN;

#endif

// Perform vector calculations.  Derive var_disply_data from seatalk_data and gnss_data.

    ww_vector::v_display_from_instruments( );

    server.handleClient(); // Check for http events, such as client connections and page requests.
    webSocket.loop(); // Check for websocket events, such as socket establishment and client messages received.

  // For each client, send json variable data.

    for( int8_t s8_client = 0; s8_client <= CLIENT_BOUND; s8_client++ ) {

      if( ww_client::ap_client_list[ s8_client ]->s8_socket != DISCONNECTED ) {

// Send json variables for the page corresponding to client s8_client
// using the socket over which the client is connected.

        if( ww_page::ap_page_list[ ww_client::ap_client_list[ s8_client ]->s8_this ]->pac_html == ww_page::ac_html_00 ) {
          json.v_json_null( pac_json_var );
        }
        else if( ww_page::ap_page_list[ ww_client::ap_client_list[ s8_client ]->s8_this ]->pac_html == ww_page::ac_html_01 ) {
          e_perspective_t e_perspective = ww_page::ap_page_list[ ww_client::ap_client_list[ s8_client ]->s8_this ]->e_perspective;
          json.v_json_var_01( e_perspective, pac_json_var );
        }
        else if( ww_page::ap_page_list[ ww_client::ap_client_list[ s8_client ]->s8_this ]->pac_html == ww_page::ac_html_02 ) {
          json.v_json_var_02( pac_json_var );
        }
        else if( ww_page::ap_page_list[ ww_client::ap_client_list[ s8_client ]->s8_this ]->pac_html == ww_page::ac_html_03 ) {
          json.v_json_var_03( pac_json_var );
        }
        else if( ww_page::ap_page_list[ ww_client::ap_client_list[ s8_client ]->s8_this ]->pac_html == ww_page::ac_html_04 ) {
          json.v_json_var_04( pac_json_var );
        }
        uint8_t u8_socket = (uint8_t)ww_client::ap_client_list[ s8_client ]->s8_socket;
        webSocket.sendTXT( u8_socket, pac_json_var );
      }
    }

    hmi.v_clear_update();

  }
}


// Web server callback functions.

void handle_onConnect() { // Callback handler for the initial page load.
  server.send( 200, "text/html", ww_page::ap_page_list[ 0 ]->pac_html );
}

void handle_01() {
  server.send( 200, "text/html", ww_page::ap_page_list[ 1 ]->pac_html );
}

void handle_02() { 
  server.send( 200, "text/html", ww_page::ap_page_list[ 2 ]->pac_html );
}

void handle_03() {
  server.send( 200, "text/html", ww_page::ap_page_list[ 3 ]->pac_html );
}

void handle_04() {
  server.send( 200, "text/html", ww_page::ap_page_list[ 4 ]->pac_html );
}

void handle_05() {
  server.send( 200, "text/html", ww_page::ap_page_list[ 5 ]->pac_html );
}

void handle_06() {
  server.send( 200, "text/html", ww_page::ap_page_list[ 6 ]->pac_html );
}

void handle_splash() { // Callback handler for a client request for the splash screen.
  server.send( 200, "image/svg+xml", ww_page::ac_splash );
}

void handle_NotFound() { // Callback handler for resource not found.
  server.send( 200, "text/html", ww_page::ap_page_list[ 0 ]->pac_html );
}



void webSocketEvent( uint8_t u8_socket, WStype_t socket_event, uint8_t* payload, size_t payload_size ) {

// Process socket events.
// u8_socket identifies the socket.
// socket_event identifies the event type.
// payload points to a uint8_t array of size payload_size.
// This function only responds to CLIENT_BOUND + 1 clients.

int8_t s8_client;
int8_t s8_socket = (int8_t)u8_socket;

  if( u8_socket <= SOCKET_BOUND ){

    switch ( socket_event ) {

      case WStype_CONNECTED: // Request the url of the page loaded by the client.
                             // The client response is processed below, with if( strcmp( ac_key, "this" ) == 0 ).

        webSocket.sendTXT( u8_socket, pac_json_req_this );

      break;

      case WStype_DISCONNECTED: // The client connected to socket u8_socket has disconnected.  
                                // So, update the corresponding entry in the client list.

        s8_client = ww_client::s8_client_from_socket( s8_socket );
        ww_client::ap_client_list[ s8_client ]->s8_socket = DISCONNECTED;

      break;

      case WStype_TEXT: // Functions to perform when a client sends text data.

// Messages are of the form "key":"value".
// First of all, convert payload to char arrays for key and value.

char ac_key[ WS_KEY_INDEX_BOUND + 1 ];
char ac_value[ WS_VALUE_INDEX_BOUND + 1 ];
int8_t s8_payload_index;
int8_t s8_key_index;
int8_t s8_value_index;
String s_temp;

      s8_payload_index = 0;
      s8_key_index = 0;
      s8_value_index = 0;

      while (( *( payload + s8_payload_index ) != ':' ) && ( s8_payload_index < payload_size )) {
        ac_key[ s8_key_index ] = *( payload + s8_payload_index );
        s8_payload_index += 1;
        s8_key_index += 1;
      }
      s8_payload_index += 1;
      ac_key[ s8_key_index ] = '\0';

      while ( s8_payload_index < payload_size ) {
        ac_value[ s8_value_index ] = *( payload + s8_payload_index );
        s8_payload_index += 1;
        s8_value_index += 1;
      }
      ac_value[ s8_value_index ] = '\0';

// Now use if statements based on strcmp() == 0 with key and value to identify the message and its contents.

      if( strcmp( ac_key, "this" ) == 0 ) {

// The client has loaded a new page and responded to a req:this message.
// The response is either '/' or "/this?client=client".
// if '/', then this ia an initial connections, so intiailize a client.
// Otherwise, update the client identified by s8_socket with the current this, socket and next.

        if( ww_url::b_base_url( ac_value )) {
          ww_client::v_initialize_client( s8_socket );
        }
        else {
          ww_client::v_advance_client( s8_socket, ac_value );
          s8_client = ww_client::s8_client_from_socket( s8_socket );

// Send constants.

          if( ww_page::ap_page_list[ ww_client::ap_client_list[ s8_client ]->s8_this ]->pac_html == ww_page::ac_html_01 ) {
            webSocket.sendTXT( u8_socket, pac_json_const_01 );
          }
          else if( ww_page::ap_page_list[ ww_client::ap_client_list[ s8_client ]->s8_this ]->pac_html == ww_page::ac_html_02 ) {
            webSocket.sendTXT( u8_socket, pac_json_const_02 );
          }
          else if( ww_page::ap_page_list[ ww_client::ap_client_list[ s8_client ]->s8_this ]->pac_html == ww_page::ac_html_03 ) {
            webSocket.sendTXT( u8_socket, pac_json_const_03 );
          }
          else if( ww_page::ap_page_list[ ww_client::ap_client_list[ s8_client ]->s8_this ]->pac_html == ww_page::ac_html_04 ) {
            webSocket.sendTXT( u8_socket, pac_json_const_04 );
          }
        }
      }
      else if( strcmp( ac_key, "req" ) == 0 ) {
        if( strcmp( ac_value, "next" ) == 0 ) {
          s8_client = ww_client::s8_client_from_socket( s8_socket );
          json.v_json_resp_next( s8_client, pac_json_resp_next );
          webSocket.sendTXT( u8_socket, pac_json_resp_next );
        }
      }
      else if( strcmp( ac_key, "offset" ) == 0 ) {
        if( strcmp( ac_value, "dec" ) == 0 ) {
// -----------------------------------------------------------
// Tell ww_storage to decrease windvane offset correction value.
// -----------------------------------------------------------
          if( b_unblocked( )) {
            correction_data.s16_orh = ( correction_data.s16_orh > ( OFFSET_LOWER_BOUND + OFFSET_INCREMENT )) ? correction_data.s16_orh - OFFSET_INCREMENT : OFFSET_LOWER_BOUND;
          }
        }
        else if( strcmp( ac_value, "inc" ) == 0 ) {
// -----------------------------------------------------------
// Tell ww_storage to increase windvane offset correction value.
// -----------------------------------------------------------
          if( b_unblocked( )) {
            correction_data.s16_orh = ( correction_data.s16_orh < ( OFFSET_UPPER_BOUND - OFFSET_INCREMENT )) ? correction_data.s16_orh + OFFSET_INCREMENT : OFFSET_UPPER_BOUND;
          }
        }
      } // ac_key == 'offset'
      else if( strcmp( ac_key, "variation" ) == 0 ) {
        if( strcmp( ac_value, "dec" ) == 0 ) {
// -----------------------------------------------------------
// Tell ww_storage to decrease compass variation correction value.
// -----------------------------------------------------------
          if( b_unblocked( )) {
            correction_data.s16_mrn = ( correction_data.s16_mrn < ( VARIATION_UPPER_BOUND - VARIATION_INCREMENT )) ? correction_data.s16_mrn + VARIATION_INCREMENT : VARIATION_UPPER_BOUND;
          }
        }
        else if( strcmp( ac_value, "inc" ) == 0 ) {
// -----------------------------------------------------------
// Tell ww_storage to increase compass variation correction value.
// -----------------------------------------------------------
          if( b_unblocked( )) {
            correction_data.s16_mrn = ( correction_data.s16_mrn > ( VARIATION_LOWER_BOUND + VARIATION_INCREMENT )) ? correction_data.s16_mrn - VARIATION_INCREMENT : VARIATION_LOWER_BOUND;
          }
        }
      } // ac_key == 'variation'
      else if( strcmp( ac_key, "fc" ) == 0 ) {
        if( strcmp( ac_value, "dec" ) == 0 ) {
// -----------------------------------------------------------
// Tell ww_storage to decrease filter cutoff frequency index correction value.
// -----------------------------------------------------------
          if( b_unblocked( )) {
            correction_data.s16_fc = ( correction_data.s16_fc > 0 ) ? correction_data.s16_fc - 1 : correction_data.s16_fc;
          }
        }
        else if( strcmp( ac_value, "inc" ) == 0 ) {
// -----------------------------------------------------------
// Tell ww_storage to increase filter cutoff frequency index correction value.
// -----------------------------------------------------------
          if( b_unblocked( )) {
            correction_data.s16_fc = ( correction_data.s16_fc < FC_INDEX_BOUND ) ? correction_data.s16_fc + 1 : correction_data.s16_fc;
          }
        }
      } // ac_key == 'fc'
      else if( strcmp( ac_key, "corrections" ) == 0 ) {
        if( strcmp( ac_value, "commit" ) == 0 ) {
// -----------------------------------------------------------
// Tell ww_storage to commit correction data to non-volatile storage.
// -----------------------------------------------------------
          ww_storage::v_store( correction_data );
        } // ac_value == 'commit'
      } // ac_key == 'corrections'
      break;
    } // switch ( socket_event )
  } // if( u8_socket <= CLIENT_BOUND )
} //webSocketEvent



bool b_unblocked() {

// Implement debouncing of client buttons.

  if( s8_debounce_count > 0 ) {
    return false;
  }
  else {
    s8_debounce_count = DEBOUNCE_COUNT;
    return true;
  }
}