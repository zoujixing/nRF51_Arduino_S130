

#ifndef _WIRING_DIGITAL_
#define _WIRING_DIGITAL_

#ifdef __cplusplus
extern "C" {
#endif


extern void pinMode( uint8_t ulPin, uint8_t ulMode );
 
extern void digitalWrite( uint8_t ulPin, uint32_t ulVal );

extern int digitalRead( uint8_t ulPin );

#ifdef __cplusplus
}
#endif

#endif /* _WIRING_DIGITAL_ */
