#ifndef FILTER
  #define FILTER

  #if (ARDUINO >= 100)
    #include "Arduino.h"
  #else
    #include "WProgram.h"
  #endif

class ww_filter {

  public:

    ww_filter();  
    void static v_configure( int16_t s16_fc, double d_q  );
    void v_begin( );
    component_vector_t x_step( component_vector_t x_in );
    double static d_fc_from_index( uint16_t s16_fc );

    static double d_s;
    static double d_q;
    static double d_k;
    static double d_norm;
    static double d_a0;
    static double d_a1;
    static double d_a2;
    static double d_b0;
    static double d_b1;
    static double d_b2; 

  private:

    component_vector_t x_z1;
    component_vector_t x_z2;

};

#endif
