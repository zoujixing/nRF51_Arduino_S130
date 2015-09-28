

#ifndef _WIRING_
#define _WIRING_

#ifdef __cplusplus
extern "C" {
#endif

#define APP_TIMER_PRESCALER                  0    //Value of the RTC1 PRESCALER register.
#define APP_TIMER_MAX_TIMERS                 6    //Maximum number of simultaneously created timers.
#define APP_TIMER_OP_QUEUE_SIZE              4    //Size of timer operation queues. 


extern void lfclk_config(void);
extern void rtc_timer_init(void);
extern uint32_t getRunTimer(void);

extern uint32_t millis( void );
extern uint32_t micros( void );

extern void delay( uint32_t ms ) ;
extern void delayMicroseconds( uint32_t us );

#ifdef __cplusplus
}
#endif

#endif 
