#ifndef GNSS
  #define GNSS

  #if (ARDUINO >= 100)
    #include "Arduino.h"
  #else
    #include "WProgram.h"
  #endif

class ww_gnss {

  public:
  
    ww_gnss();
    void v_begin( gnss_data_t* p_gnss_data );
    void v_sns_crn_utc_from_gnss( );
    
  private:
  
    void v_nmea_from_gnss( );
    bool b_sns_from_nmea( );
    bool b_crn_from_nmea( );
    bool b_utc_from_nmea( );
    double d_double_from_paus_number( uint8_t* paus_number );
    double normal( double d_abnormal );

    gnss_data_t* p_gnss_data;

};

#endif
