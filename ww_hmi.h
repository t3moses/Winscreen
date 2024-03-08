
#ifndef HMI
  #define HMI

  #if (ARDUINO >= 100)
    #include "Arduino.h"
  #else
    #include "WProgram.h"
  #endif

  class ww_hmi {
    
    public:
    
      ww_hmi();
      void v_begin();
      bool b_update_now( );
      void v_clear_update();
      
      static void IRAM_ATTR timer_group1_handler( /*volatile int timer_number*/ );

  };
  
#endif
