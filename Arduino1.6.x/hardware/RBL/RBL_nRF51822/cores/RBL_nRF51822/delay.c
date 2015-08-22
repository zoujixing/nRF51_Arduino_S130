
#include "Arduino.h"

#ifdef __cplusplus
extern "C" {
#endif

#define RTC1_CYCLE_TIME                      APP_TIMER_TICKS(120000, 0)

static app_timer_id_t                        m_rtc_start_timer_id; 

// widen rtc1 to 40-bit (1099511627775 ticks = 33554431999969us = 388 days)
// (dont overflow uint64_t when multipying by 1000000)
extern uint64_t rtc1_overflow_count;

/**********************************************************************
name :
function : 
**********************************************************************/
void lfclk_config(void)
{	
	//LFCLK Init
    NRF_CLOCK->LFCLKSRC             = (CLOCK_LFCLKSRC_SRC_Xtal << CLOCK_LFCLKSRC_SRC_Pos);
    NRF_CLOCK->EVENTS_LFCLKSTARTED  = 0;
    NRF_CLOCK->TASKS_LFCLKSTART     = 1;
    while (NRF_CLOCK->EVENTS_LFCLKSTARTED == 0)
    {
        //Do nothing.
    }
    NRF_CLOCK->EVENTS_LFCLKSTARTED = 0;
}
/**********************************************************************
name :
function : 
**********************************************************************/
void timer_error_check(uint32_t err_code)
{
	if(err_code != NRF_SUCCESS)
	{
		NVIC_SystemReset();
	}
}
/**********************************************************************
name :
function : 
**********************************************************************/
void m_rtc_start_handle(void * p_context)
{
	//UNUSED_PARAMETER(p_context);
}
/**********************************************************************
name :
function : 
**********************************************************************/
void rtc_timer_init()
{	
	uint32_t err_code;
	//8 timer application, not use scheduler.
	APP_TIMER_INIT(0, 8, 5);  
	
	//m_rtc_start_timer_id task do nothing, just make sure RTC1 is running.
	err_code = app_timer_create(&m_rtc_start_timer_id,
					 APP_TIMER_MODE_REPEATED,
					 m_rtc_start_handle);
	APP_ERROR_CHECK(err_code);

	err_code = app_timer_start(m_rtc_start_timer_id, RTC1_CYCLE_TIME, NULL);
    APP_ERROR_CHECK(err_code);	
}
/**********************************************************************
name :
function : 
**********************************************************************/
uint64_t millis64( void )
{	
	//divide by 32768
	uint32_t time_;
	app_timer_cnt_get( &time_ );
	return (rtc1_overflow_count + time_) * 1000 >> 15;  
}
/**********************************************************************
name :
function : 
**********************************************************************/
uint64_t micros64( void )
{
	//accurate to 30.517us, divide by 32768
	uint32_t time_;
	app_timer_cnt_get( &time_ );
	return (rtc1_overflow_count + time_) * 1000000 >> 15;  
}
/**********************************************************************
name :
function : 
**********************************************************************/
uint32_t millis( void )
{
	return millis64();
}
/**********************************************************************
name :
function : 
**********************************************************************/
uint32_t micros( void )
{
	return micros64();
}
/**********************************************************************
name :
function : 
**********************************************************************/
void delay( uint32_t ms )
{
	uint32_t start = millis();
	while (millis() - start < ms)
	{
	    yield();
	}
}
/**********************************************************************
name :
function : 
**********************************************************************/
void delayMicroseconds( uint32_t us )
{
    while (us--)
    {
        __ASM(" NOP\n\t"
        " NOP\n\t"
        " NOP\n\t"
        " NOP\n\t"
        " NOP\n\t"
        " NOP\n\t"
        " NOP\n\t"
        " NOP\n\t"
        " NOP\n\t"
        " NOP\n\t"
        " NOP\n\t");
    };
}

#ifdef __cplusplus
}
#endif
