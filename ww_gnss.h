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
    void v_begin( var_display_data_t* p_var_display_data, gnss_data_t* p_gnss_data );
    void v_x_gr_from_gnss( );
    
  private:
  
    void v_nmea_from_gnss( );
    bool b_fix_from_gxgll( );
    void v_x_gr_from_afix_series( );
    void v_utc_time_from_pac( const char* pac_tim );
    double d_utc_hours_from_pac( char* pac_tim );
    double d_lat_minutes_from_pac( char* pac_lat, char* pac_n_s );
    double d_lon_minutes_from_pac( char* pac_lon, char* pac_w_e );
    double d_double_from_pac( char* pac_char );

    fix_t x_fix;
    fix_t ax_fix_series[ FIX_SERIES_INDEX_MAX + 1 ];

    int8_t c_updates_per_heart_beat;
    int8_t c_update_count;

    int16_t s16_current_fix_index;
    int16_t s16_previous_fix_index;

    uint8_t au8_nmea[ NMEA_INDEX_BOUND + 1 ];
    uint8_t* pau8_nmea = au8_nmea;

    uint8_t au8_sns[ FIELD_INDEX_BOUND + 1 ];
    uint8_t* pau8_sns = au8_sns;

    uint8_t au8_crn[ FIELD_INDEX_BOUND + 1 ];
    uint8_t* pau8_crn = au8_crn;

    uint8_t au8_tim[ FIELD_INDEX_BOUND + 1 ];
    uint8_t* pau8_tim = au8_tim;

    uint8_t au8_lat[ FIELD_INDEX_BOUND + 1 ];
    uint8_t* pau8_lat = au8_lat;

    uint8_t au8_lon[ FIELD_INDEX_BOUND + 1 ];
    uint8_t* pau8_lon = au8_lon;

    uint8_t au8_n_s[ FIELD_INDEX_BOUND + 1 ];
    uint8_t* pau8_n_s = au8_n_s;

    uint8_t au8_w_e[ FIELD_INDEX_BOUND + 1 ];
    uint8_t* pau8_w_e = au8_w_e;

    char ac_tim[ FIELD_INDEX_BOUND + 1 ];
    char* pac_tim = ac_tim;

    char ac_lat[ FIELD_INDEX_BOUND + 1 ];
    char* pac_lat = ac_lat;

    char ac_lon[ FIELD_INDEX_BOUND + 1 ];
    char* pac_lon = ac_lon;

    char ac_n_s[ FIELD_INDEX_BOUND + 1 ];
    char* pac_n_s = ac_n_s;

    char ac_w_e[ FIELD_INDEX_BOUND + 1 ];
    char* pac_w_e = ac_w_e;

    component_vector_t x_gc;

    gnss_data_t* p_gnss_data;
    var_display_data_t* p_var_display_data;

};

#endif
