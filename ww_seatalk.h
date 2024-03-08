#ifndef SEATALK
  #define SEATALK

  #if (ARDUINO >= 100)
    #include "Arduino.h"
  #else
    #include "WProgram.h"
  #endif
  

  class ww_seatalk {
    
    public:
    
      ww_seatalk();
      void v_begin( );
      void v_capture_datagram( );
      void v_clear_availability();
      bool b_datagram_available();
      uint16_t u16_get_transition_index( );
      uint32_t u32_get_transition_time( uint16_t u16_index );

      static void IRAM_ATTR dataagram_end_handler();
      static void IRAM_ATTR bit_transition_handler();
      
    private:
    
  };
  
#endif
