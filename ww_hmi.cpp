
#include "ww_constants.h"
#include "ww_types.h"
#include "ww_hmi.h"
#include "driver/timer.h"

bool b_update;

ww_hmi::ww_hmi() {}

  
  
void IRAM_ATTR timer_group1_isr( void *para ) {

//  volatile int timer_no = ( int )para;

  ww_hmi::timer_group1_handler( /*timer_no*/ );

}



void ww_hmi::v_begin() {
    
// TIMER_GROUP_1, TIMER_0 controls the HMI update function.

  timer_config_t timer_0_config = {
    .alarm_en = TIMER_ALARM_EN,
    .counter_en = TIMER_PAUSE,
    .intr_type = TIMER_INTR_LEVEL,
    .counter_dir = TIMER_COUNT_UP,
    .auto_reload = TIMER_AUTORELOAD_EN,
    .divider = HMI_UPDATE_PRESCALER
  };
  noInterrupts();
  timer_init( TIMER_GROUP_1, TIMER_0, &timer_0_config );
  timer_set_counter_value( TIMER_GROUP_1, TIMER_0, 0x00000000ULL );
  timer_set_alarm_value( TIMER_GROUP_1, TIMER_0, TICKS_PER_HMI_UPDATE_INTERVAL );
  timer_enable_intr( TIMER_GROUP_1, TIMER_0 );
  timer_isr_register( TIMER_GROUP_1, TIMER_0, timer_group1_isr, (void *)TIMER_0, ESP_INTR_FLAG_IRAM, NULL );
  timer_start( TIMER_GROUP_1, TIMER_0 );
  interrupts();

}



bool ww_hmi::b_update_now( ) {

// Return true if it is time to update the HMI.

bool b_result;

  noInterrupts();
  b_result = b_update;
  interrupts();
  return b_result;

}



void ww_hmi::v_clear_update() {

  noInterrupts();
  b_update = false;
  interrupts();

}


void IRAM_ATTR ww_hmi::timer_group1_handler( /*volatile int timer_number*/ ) {

  timer_spinlock_take( TIMER_GROUP_1 );
  timer_group_clr_intr_status_in_isr( TIMER_GROUP_1, TIMER_0 );

  b_update = true;

  timer_group_enable_alarm_in_isr( TIMER_GROUP_1, TIMER_0 );
  timer_spinlock_give( TIMER_GROUP_1 );

}
