
#include "Arduino.h"

#ifdef __cplusplus
extern "C"{
#endif
/**********************************************************************
name :
function : 
**********************************************************************/
uint8_t shiftIn( uint8_t ulDataPin, uint8_t ulClockPin, uint8_t ulBitOrder )
{
	uint8_t value = 0 ;
	uint8_t i ;

	for ( i=0 ; i < 8 ; ++i )
    {
		digitalWrite( ulClockPin, HIGH ) ;

		if ( ulBitOrder == LSBFIRST )
        {
			value |= digitalRead( ulDataPin ) << i ;
        }
		else
        {
			value |= digitalRead( ulDataPin ) << (7 - i) ;
        }

		digitalWrite( ulClockPin, LOW ) ;
	}

	return value ;
}
/**********************************************************************
name :
function : 
**********************************************************************/
void shiftOut( uint8_t ulDataPin, uint8_t ulClockPin, uint8_t ulBitOrder, uint32_t ulVal )
{
	uint8_t i ;

	for ( i=0 ; i < 8 ; i++ )
    {
		if ( ulBitOrder == LSBFIRST )
        {
			digitalWrite( ulDataPin, !!(ulVal & (1 << i)) ) ;
        }
		else	
        {
			digitalWrite( ulDataPin, !!(ulVal & (1 << (7 - i))) ) ;
        }

		digitalWrite( ulClockPin, HIGH ) ;
		digitalWrite( ulClockPin, LOW ) ;		
	}
}

#ifdef __cplusplus
} // extern "C"
#endif
