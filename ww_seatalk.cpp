
#include "ww_constants.h"
#include "ww_types.h"
#include "driver/timer.h"
#include "ww_seatalk.h"

timer_config_t datagram_end_timer_config;
timer_config_t* p_datagram_end_timer_config = &datagram_end_timer_config;

gpio_config_t seatalk_config;
gpio_config_t* p_seatalk_config = &seatalk_config;

static volatile uint32_t u32_transition_time[ TRANSITION_INDEX_BOUND + 1 ];
static volatile uint16_t u16__transition_index;

static volatile bool b__capture_datagram;
static volatile bool b__datagram_available;



void IRAM_ATTR bit_transition_isr( void *para ) {

  ww_seatalk::bit_transition_handler();

}



void IRAM_ATTR datagram_end_isr( void *para ) {

  ww_seatalk::dataagram_end_handler();

}



ww_seatalk::ww_seatalk() {}



void ww_seatalk::v_begin( ) {

  datagram_end_timer_config = {
    .alarm_en = TIMER_ALARM_EN,
    .counter_en = TIMER_PAUSE,
    .intr_type = TIMER_INTR_LEVEL,
    .counter_dir = TIMER_COUNT_UP,
    .auto_reload = TIMER_AUTORELOAD_EN,
    .divider = DATAGRAM_WATCHDOG_PRESCALE
  };

  gpio_install_isr_service( ESP_INTR_FLAG_IRAM );

  seatalk_config.pin_bit_mask = ( 01ull << SEATALK_BUS );
  seatalk_config.mode = GPIO_MODE_INPUT;
  seatalk_config.pull_up_en = GPIO_PULLUP_DISABLE;
  seatalk_config.pull_down_en = GPIO_PULLDOWN_DISABLE;
  seatalk_config.intr_type = GPIO_INTR_ANYEDGE;

  gpio_config( p_seatalk_config );

  noInterrupts();
  
  timer_init( TIMER_GROUP_0, TIMER_0, p_datagram_end_timer_config );
  timer_isr_register( TIMER_GROUP_0, TIMER_0, datagram_end_isr, (void *)TIMER_0, ESP_INTR_FLAG_IRAM, NULL );
  timer_set_counter_value( TIMER_GROUP_0, TIMER_0, 0x00000000ULL );
  timer_set_alarm_value( TIMER_GROUP_0, TIMER_0, TICKS_PER_FRAME );
  timer_enable_intr( TIMER_GROUP_0, TIMER_0 );
  timer_start( TIMER_GROUP_0, TIMER_0 );

  gpio_isr_handler_add( SEATALK_BUS, bit_transition_isr, (void *)SEATALK_BUS );
  gpio_intr_enable( SEATALK_BUS );

  interrupts();

}



void ww_seatalk::v_capture_datagram( ) {

// Prime the bit_transition handler to capture a datagram.

  noInterrupts();
  u16__transition_index = 0;
  b__capture_datagram = true;
  interrupts();
  
}



void ww_seatalk::v_clear_availability( ) {

// Reset the datagram-available flag following the processing of a datagram.

  noInterrupts();
  b__datagram_available = false;
  interrupts();
  
}



bool ww_seatalk::b_datagram_available( ) {

// Check if a datagram is ready to be processed.

bool b___datagram_available;

  noInterrupts();
  b___datagram_available = b__datagram_available;
  interrupts();
  return b___datagram_available; 
}



uint16_t ww_seatalk::u16_get_transition_index( ) {

// Return the value of the transition array index corresponding to the end of the last datagram to be captured.

uint16_t u16___transition_index;

  noInterrupts();
  if( u16__transition_index > 0 ) u16__transition_index--;
  u16___transition_index = u16__transition_index;
  interrupts();
  return u16___transition_index;
  
}



uint32_t ww_seatalk::u32_get_transition_time( uint16_t u16_index ) {

// Return the value of one bit transition time.

uint32_t u32___transition_time;

  noInterrupts();
  u32___transition_time = u32_transition_time[ u16_index ];
  interrupts();
  return u32___transition_time;
  
}



void IRAM_ATTR ww_seatalk::bit_transition_handler() {

//
// Runs when a Seatalk bit transition occurs.
// Restart the dataagram-end watchdog.
// The watchdog will alarm when a datagram reception is complete.
//

  if( b__capture_datagram ) {
  
    u32_transition_time[ u16__transition_index ] = micros();
 
    u16__transition_index++;
   
    timer_set_counter_value( TIMER_GROUP_0, TIMER_0, 0x00000000ULL );
    timer_start( TIMER_GROUP_0, TIMER_0 );

  }
}



void IRAM_ATTR ww_seatalk::dataagram_end_handler() {

//
// Runs when no bit transitions have occurred for one frame duration.
// Stop the bit-transition handler from adding data until the datagram has been processed.
// Flag that a datagram is available for processing.
// Pause the watchdog until the datagram has been processed and the next bit-transition occurs.
//

  timer_spinlock_take( TIMER_GROUP_0 );

  timer_group_clr_intr_status_in_isr( TIMER_GROUP_0, TIMER_0 );

  b__capture_datagram = false;
  b__datagram_available = true;
  timer_pause( TIMER_GROUP_0, TIMER_0 );

  timer_group_enable_alarm_in_isr( TIMER_GROUP_0, TIMER_0 );

  timer_spinlock_give( TIMER_GROUP_0 );
  
}
