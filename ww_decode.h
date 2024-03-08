
namespace ww_decode {

      void v_begin( uint32_t* pau32__transition, uint16_t* pu16__transition_index_max, seatalk_data_t* p_seatalk_data );
      void v_datum_from_transitions( );
      
      bool is_odd( );
      bool is_target_length( );
      void v_runs_from_transitions( );
      void v_frames_from_runs( );
      bool is_well_formed( );
      void v_bytes_from_frames( );
      datagram_type_t is_target( );
      double d_pws_from_datagram( );
      double d_hrm_from_datagram( );
      double d_rdr_from_datagram( );
      double d_aws_from_datagram( );
      double d_aro_from_datagram( );
      void v_record_from_datum( );
      double normal( double d_abnormal );

      extern uint32_t* pau32_transition; // Pointer to global array of bit transitions.
      extern uint16_t* pu16_transition_index_max; // Pointer to global max index of transitions array.
      extern datagram_type_t datagram_type; // Enumeration value of PWS, HRM, AWS, ARH.
      extern uint32_t u32_elapsed_time; // Time difference between the first and last transitions in a datagram.
      extern uint16_t u16_run[ TRANSITION_INDEX_BOUND + 1 ]; // Arry of run lengths.
      extern uint16_t u16_run_index_max;
      extern uint16_t u16_frame[ FRAME_INDEX_BOUND + 1 ]; // Array of datagram frames.
      extern uint16_t u16_frame_index_max;
      extern double d_datum_value;

      extern seatalk_data_t* p_seatalk_data;

      extern uint32_t u32_us_per_bit; // Microseconds per bit.
      extern uint32_t u32_us_per_min_target_datagram; // Elapsed time between the start of the first start bit and the start of the last stop bit ...
      extern uint32_t u32_us_per_max_target_datagram; // for the shortest and longest target datagrams.  The bit before the last stop bit must be 0.

  }
