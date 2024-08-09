#ifndef JSON
  #define JSON

  #if (ARDUINO >= 100)
    #include "Arduino.h"
  #else
    #include "WProgram.h"
  #endif

class ww_json {

  public:

    ww_json();
    void v_begin( var_display_data_t* p_var_display_data,
                  correction_data_t* p_correction_data );
    void v_json_null( char* pac_json_null );
    void v_json_req_this( char* pac_json_req_this );
    void v_json_resp_next( int8_t s8_client, char* pac_json_resp_next );
    void v_json_const_01( char* pac_json_const_01 );
    void v_json_const_02( char* pac_json_const_02 );
    void v_json_const_03( char* pac_json_const_03 );
    void v_json_const_04( char* pac_json_const_04 );
    void v_json_var_01( perspective_t e_perspective, char* pac_json_var_01 );
    void v_json_var_02( char* pac_json_var_02 );
    void v_json_var_03( char* pac_json_var_03 );
    void v_json_var_04( char* pac_json_var_04 );
    arrowhead_t x_arrow_head_from_radial( radial_vector_t x_radial_vector );

    var_display_data_t* p_var_display_data;
    correction_data_t* p_correction_data;

    arrowhead_t xa_nr; // True and magnetic north arrowheads.
    arrowhead_t xa_mr;

    int16_t s16_nr_0_x; // true north arrow point 0 x coordinate relative to the panel centre (pixels).
    int16_t s16_nr_0_y; // ditto point 0 y coordinate.
    int16_t s16_nr_1_x; // ditto point 1 x coordinate.
    int16_t s16_nr_1_y; // ditto point 1 y coordinate.
    int16_t s16_nr_2_x; // ditto point 2 x coordinate.
    int16_t s16_nr_2_y; // ditto point 2 y coordinate.
    int16_t s16_mr_0_x; // magnetic north arrow point 0 x coordinate relative to the panel centre (pixels).
    int16_t s16_mr_0_y; // ditto point 0 y coordinate.
    int16_t s16_mr_1_x; // ditto point 1 x coordinate.
    int16_t s16_mr_1_y; // ditto point 1 y coordinate.
    int16_t s16_mr_2_x; // ditto point 2 2 coordinate.
    int16_t s16_mr_2_y; // ditto point 2 y coordinate.
    int16_t s16_vr_x; // vmg vector x coordinate relative to the panel centre (pixels).
    int16_t s16_vr_y; // vmg vector y coordinate relative to the panel centre (pixels).
    int16_t s16_rb_x; // rudder base vector x coordinate relative to the panel centre (pixels).
    int16_t s16_rb_y; // rudder base vector y coordinate relative to the panel centre (pixels).
    int16_t s16_rt_x; // rudder tip vector x coordinate relative to the panel centre (pixels).
    int16_t s16_rt_y; // rudder tip vector y coordinate relative to the panel centre (pixels).
    int16_t s16_ar_x; // apparent wind vector x coordinate relative to the panel centre (pixels).
    int16_t s16_ar_y; // apparent wind vector y coordinate relative to the panel centre (pixels).
    int16_t s16_tr_x; // true wind vector x coordinate relative to the panel centre (pixels).
    int16_t s16_tr_y; // true wind vector y coordinate relative to the panel centre (pixels).
    int16_t s16_cr_x; // speed/course vector x coordinate relative to the panel centre according to Seatalk (pixels).
    int16_t s16_cr_y; // speed/course vector y coordinate relative to the panel centre according to Seatalk (pixels).
    int16_t s16_gr_x; // speed/course vector x coordinate relative to the panel centre according to GNSS (pixels).
    int16_t s16_gr_y; // speed/course vector y coordinate relative to the panel centre according to GNSS (pixels).
    int16_t s16_vmg; // vmg ball x coordinate relative to the panel centre (pixels).
    int16_t s16_crh; // crh (leeway) x coordinate relative to the panel centre (pixels).
    int16_t s16_trn; // trn ball x coordinate relative to the panel centre (pixels).
    int16_t s16_arh; // arh x coordinate relative to the panel centre (pixels).

    int8_t s8_heart_beat;
    char ac_utc[ FIELD_INDEX_BOUND + 1 ];
    char* pac_utc = ac_utc;

    double d_hrr; // Heading relative to reference (degrees).  Reference may be true wind direction or true north.

    String as_hx[ HULL_POINTS_BOUND + 1 ]; // Array of strings representing hull point x and y coordiantes.
    String as_hy[ HULL_POINTS_BOUND + 1 ];

    String s_0; // String reperspective of the hull point coordinates (pixels).
    String s_1; // Even values are x coordiantes.
    String s_2; // Odd values are y coordinaes.
    String s_3; // All values are relative to the centre of the panel.
    String s_4;
    String s_5;
    String s_6;
    String s_7;
    String s_8;
    String s_9;
    String s_10;
    String s_11;
    String s_12;
    String s_13;
    String s_14;
    String s_15;
    String s_16;
    String s_17;
    String s_18;
    String s_19;
    String s_20;
    String s_21;
    String s_22;
    String s_23;
    String s_24;
    String s_25;

    String s_nr_0_x; // String reperspective of the x and y components of the true and magnetic north arrowheads (pixels).
    String s_nr_0_y;
    String s_nr_1_x;
    String s_nr_1_y;
    String s_nr_2_x;
    String s_nr_2_y;

    String s_mr_0_x;
    String s_mr_0_y;
    String s_mr_1_x;
    String s_mr_1_y;
    String s_mr_2_x;
    String s_mr_2_y;

    radial_vector_t xd_ar; // Apparent wind radial vector.

    String s_ar_x; // x and y coordinates of the apparent wind vector (pixels) relative to the centre of the panel.
    String s_ar_y;

    radial_vector_t xd_tr; // True wind radial vector.

    String s_tr_x; // x and y coordinates of the true wind vector (pixels) relative to the centre of the panel.
    String s_tr_y;

    radial_vector_t xd_cr; // Speed/course radial vector.

    String s_cr_x; // x and y coordinates of the speed/course vector according to Seatalk (pixels) relative to the centre of the panel.
    String s_cr_y;

    String s_gr_x; // x and y coordinates of the speed/course vector according to GNSS (pixels) relative to the centre of the panel.
    String s_gr_y;

    radial_vector_t xd_vr; // VMG radial vector.

    String s_vr_x; // x and y coordinates of the VMG vector (pixels) relative to the centre of the panel.
    String s_vr_y;

 //   radial_vector_t xd_gh; // VMG radial vector.

    String s_crs_dot_x; // x and y coordinates of the centre of the GNSS course dot (pixels) relative to the centre of the panel.
    String s_crs_dot_y;

    component_vector_t xd_rp; // Rudder post radial vector.

    String s_rp_x; // x and y coordinates of the rudder post (pixels) relative to the centre of the panel.
    String s_rp_y;

    component_vector_t xd_rt; // Rudder tip radial vector.

    String s_rt_x; // x and y coordinates of the rudder tip (pixels) relative to the centre of the panel.
    String s_rt_y;

    String s_vmg_clr; // String reperspective of the VMG vector colour.
    String s_vmg_s_clr; // String reperspective of the VMG shadow vector colour.
    String s_vmg_b_clr; // String reperspective of the VMG ball colour.
    String s_hbt_clr; // String reperspective of the heartbeat colour.
    
    String s_cr_shadow_begin_x; // String reperspective of the begin and end coordiantes of the speed/course shadow vector (pixels).
    String s_cr_shadow_begin_y; // Relative to the panel centre.
    String s_cr_shadow_end_x;
    String s_cr_shadow_end_y;

    String s_tr_shadow_begin_x; // String reperspective of the begin and end coordiantes of the true wind shadow vector (pixels).
    String s_tr_shadow_begin_y; // Relative to the panel centre.
    String s_tr_shadow_end_x;
    String s_tr_shadow_end_y;

    String s_vr_shadow_begin_x; // String reperspective of the begin and end coordiantes of the VMG shadow vector (pixels).
    String s_vr_shadow_begin_y; // Relative to the panel centre.
    String s_vr_shadow_end_x;
    String s_vr_shadow_end_y;
  
    String s_vmg_ball_radius; // String reperspective of the vmg, crh and trn ball radius (pixels).
    String s_crh_ball_radius;
    String s_trn_ball_radius;
    String s_crh_ball_centre_x; // String reperspective of the vmg, crh and trn ball x coordinates (pixels).
    String s_vmg_ball_centre_x;
    String s_trn_ball_centre_x;

    String s_arh; // String reperspective of the apparent wind angle relative to heading (degrees).
    String s_crh; // String reperspective of the course relative to heading (degrees).
    String s_trn; // String reperspective of the true wind angle relative to true north (degrees).
    String s_vmg; // String reperspective of VMG (knots).

    String s_offset; // String reperspective of wind vane offset (degrees).
    String s_variation; // String reperspective of compass variation (degrees).
    String s_fc; // String reperspective of the low-pass filter cutoff (Hz).

};

#endif
