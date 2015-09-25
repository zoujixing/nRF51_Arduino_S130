

#ifndef _WIRING_SHIFT_
#define _WIRING_SHIFT_

#ifdef __cplusplus
 extern "C" {
#endif


extern uint8_t shiftIn( uint8_t ulDataPin, uint8_t ulClockPin, uint8_t ulBitOrder ) ;

extern void shiftOut( uint8_t ulDataPin, uint8_t ulClockPin, uint8_t ulBitOrder, uint32_t ulVal ) ;


#ifdef __cplusplus
}
#endif

#endif /* _WIRING_SHIFT_ */
