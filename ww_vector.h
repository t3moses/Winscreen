
namespace ww_vector {

      void v_begin( var_display_data_t* p_var_display_data, 
        seatalk_data_t* p_seatalk_data, 
        gnss_data_t* p_gnss_data,
        correction_data_t* p_correction_data );
      void v_display_from_instruments( );
      void v_display_from_instruments( );
      radial_vector_t x_compress_angle( radial_vector_t x_radial_vector );
      component_vector_t x_component_from_l_a( double d_l, double d_a );
      component_vector_t x_component_from_x_y( double d_x, double d_y );
      component_vector_t x_component_from_radial( radial_vector_t x_radial_vector );
      component_vector_t x_add_component( component_vector_t x_component_vector_1, component_vector_t x_component_vector_2 );      
      component_vector_t x_subtract_component( component_vector_t x_component_vector_1, component_vector_t x_component_vector_2 );
      radial_vector_t x_reflect_ref( radial_vector_t x_radial_vector );
      radial_vector_t x_reflect_quad( radial_vector_t x_radial_vector );
      radial_vector_t x_rotate( radial_vector_t x_radial_vector, double d_angle );
      radial_vector_t x_subtract_radial( radial_vector_t x_radial_vector_1, radial_vector_t x_radial_vector_2 );
      double d_angle_from_radial( radial_vector_t x_radial_vector );
      double d_projection( double d_length, double d_angle );
      bool b_upwind_from_instruments( );
      double d_length_from_radial( radial_vector_t x_radial_vector );
      double d_length_from_component( component_vector_t x_component_vector );
      double d_angle_from_component( component_vector_t x_component_vector );
      double d_add_angle( double d_angle_1, double d_angle_2 );
      double d_normalize_angle( double d_a );
      radial_vector_t x_radial_from_component( component_vector_t x_component_vector );

      extern var_display_data_t* p_var_display_data;
      extern seatalk_data_t* p_seatalk_data;
      extern gnss_data_t* p_gnss_data;
      extern correction_data_t* p_correction_data;

  }
