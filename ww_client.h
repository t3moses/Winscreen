#ifndef CLIENT
  #define CLIENT

  #if (ARDUINO >= 100)
    #include "Arduino.h"
  #else
    #include "WProgram.h"
  #endif

class ww_client {

  public:
  
    ww_client( );
    static void v_begin( );
    static int8_t s8_client_from_socket( int8_t s8_socket );
    static void v_initialize_client( int8_t s8_socket );
    static void v_advance_client( int8_t s8_socket, char pac_value[] );
    static String s_next_from_client( int8_t s8_client );

    static ww_client* ap_client_list[ CLIENT_BOUND + 1 ];

    client_role_t e_role;
    int8_t s8_socket;
    int8_t s8_this;
    int8_t s8_next;

};

#endif
