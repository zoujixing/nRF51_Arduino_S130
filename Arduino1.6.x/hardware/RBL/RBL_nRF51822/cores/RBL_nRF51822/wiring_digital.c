

#include "Arduino.h"

#ifdef __cplusplus
 extern "C" {
#endif

/**********************************************************************
name :
function : 
**********************************************************************/
void pinMode( uint8_t ulPin, uint8_t ulMode )
{	
	uint8_t pin;
	
	PPI_releaseFromPWM(ulPin);
	pin = Pin_nRF51822_to_Arduino(ulPin);
	if(pin < 31)
	{
		switch ( ulMode )
		{
			case INPUT:
				NRF_GPIO->PIN_CNF[pin] = (GPIO_PIN_CNF_SENSE_Disabled << GPIO_PIN_CNF_SENSE_Pos)
										| (GPIO_PIN_CNF_DRIVE_S0S1 << GPIO_PIN_CNF_DRIVE_Pos)
										| (GPIO_PIN_CNF_PULL_Disabled << GPIO_PIN_CNF_PULL_Pos)
										| (GPIO_PIN_CNF_INPUT_Connect << GPIO_PIN_CNF_INPUT_Pos)
										| (GPIO_PIN_CNF_DIR_Input << GPIO_PIN_CNF_DIR_Pos);
			break ;

			case INPUT_NOPULL:
				NRF_GPIO->PIN_CNF[pin] = (GPIO_PIN_CNF_SENSE_Disabled << GPIO_PIN_CNF_SENSE_Pos)
										| (GPIO_PIN_CNF_DRIVE_S0S1 << GPIO_PIN_CNF_DRIVE_Pos)
										| (GPIO_PIN_CNF_PULL_Disabled << GPIO_PIN_CNF_PULL_Pos)
										| (GPIO_PIN_CNF_INPUT_Connect << GPIO_PIN_CNF_INPUT_Pos)
										| (GPIO_PIN_CNF_DIR_Input << GPIO_PIN_CNF_DIR_Pos);
			break ;

			case INPUT_PULLDOWN:
				NRF_GPIO->PIN_CNF[pin] = (GPIO_PIN_CNF_SENSE_Disabled << GPIO_PIN_CNF_SENSE_Pos)
										| (GPIO_PIN_CNF_DRIVE_S0S1 << GPIO_PIN_CNF_DRIVE_Pos)
										| (GPIO_PIN_CNF_PULL_Pulldown << GPIO_PIN_CNF_PULL_Pos)
										| (GPIO_PIN_CNF_INPUT_Connect << GPIO_PIN_CNF_INPUT_Pos)
										| (GPIO_PIN_CNF_DIR_Input << GPIO_PIN_CNF_DIR_Pos);
			break ;

			case INPUT_PULLUP:
				NRF_GPIO->PIN_CNF[pin] = (GPIO_PIN_CNF_SENSE_Disabled << GPIO_PIN_CNF_SENSE_Pos)
										| (GPIO_PIN_CNF_DRIVE_S0S1 << GPIO_PIN_CNF_DRIVE_Pos)
										| (GPIO_PIN_CNF_PULL_Pullup << GPIO_PIN_CNF_PULL_Pos)
										| (GPIO_PIN_CNF_INPUT_Connect << GPIO_PIN_CNF_INPUT_Pos)
										| (GPIO_PIN_CNF_DIR_Input << GPIO_PIN_CNF_DIR_Pos);
			break ;

			case OUTPUT:
				NRF_GPIO->PIN_CNF[pin] = (GPIO_PIN_CNF_SENSE_Disabled << GPIO_PIN_CNF_SENSE_Pos)
										| (GPIO_PIN_CNF_DRIVE_H0H1 << GPIO_PIN_CNF_DRIVE_Pos)
										| (GPIO_PIN_CNF_PULL_Disabled << GPIO_PIN_CNF_PULL_Pos)
										| (GPIO_PIN_CNF_INPUT_Connect << GPIO_PIN_CNF_INPUT_Pos)
										| (GPIO_PIN_CNF_DIR_Output << GPIO_PIN_CNF_DIR_Pos);
			break ;

			case OUTPUT_S0S1:
				NRF_GPIO->PIN_CNF[pin] = (GPIO_PIN_CNF_SENSE_Disabled << GPIO_PIN_CNF_SENSE_Pos)
										| (GPIO_PIN_CNF_DRIVE_S0S1 << GPIO_PIN_CNF_DRIVE_Pos)
										| (GPIO_PIN_CNF_PULL_Disabled << GPIO_PIN_CNF_PULL_Pos)
										| (GPIO_PIN_CNF_INPUT_Connect << GPIO_PIN_CNF_INPUT_Pos)
										| (GPIO_PIN_CNF_DIR_Output << GPIO_PIN_CNF_DIR_Pos);
			break ;

			case OUTPUT_H0S1:
				NRF_GPIO->PIN_CNF[pin] = (GPIO_PIN_CNF_SENSE_Disabled << GPIO_PIN_CNF_SENSE_Pos)
										| (GPIO_PIN_CNF_DRIVE_H0S1 << GPIO_PIN_CNF_DRIVE_Pos)
										| (GPIO_PIN_CNF_PULL_Disabled << GPIO_PIN_CNF_PULL_Pos)
										| (GPIO_PIN_CNF_INPUT_Connect << GPIO_PIN_CNF_INPUT_Pos)
										| (GPIO_PIN_CNF_DIR_Output << GPIO_PIN_CNF_DIR_Pos);
			break ;

			case OUTPUT_S0H1:
				NRF_GPIO->PIN_CNF[pin] = (GPIO_PIN_CNF_SENSE_Disabled << GPIO_PIN_CNF_SENSE_Pos)
										| (GPIO_PIN_CNF_DRIVE_S0H1 << GPIO_PIN_CNF_DRIVE_Pos)
										| (GPIO_PIN_CNF_PULL_Disabled << GPIO_PIN_CNF_PULL_Pos)
										| (GPIO_PIN_CNF_INPUT_Connect << GPIO_PIN_CNF_INPUT_Pos)
										| (GPIO_PIN_CNF_DIR_Output << GPIO_PIN_CNF_DIR_Pos);
			break ;

			case OUTPUT_H0H1:
				NRF_GPIO->PIN_CNF[pin] = (GPIO_PIN_CNF_SENSE_Disabled << GPIO_PIN_CNF_SENSE_Pos)
										| (GPIO_PIN_CNF_DRIVE_H0H1 << GPIO_PIN_CNF_DRIVE_Pos)
										| (GPIO_PIN_CNF_PULL_Disabled << GPIO_PIN_CNF_PULL_Pos)
										| (GPIO_PIN_CNF_INPUT_Connect << GPIO_PIN_CNF_INPUT_Pos)
										| (GPIO_PIN_CNF_DIR_Output << GPIO_PIN_CNF_DIR_Pos);
			break ;

			case OUTPUT_D0S1:
				NRF_GPIO->PIN_CNF[pin] = (GPIO_PIN_CNF_SENSE_Disabled << GPIO_PIN_CNF_SENSE_Pos)
										| (GPIO_PIN_CNF_DRIVE_D0S1 << GPIO_PIN_CNF_DRIVE_Pos)
										| (GPIO_PIN_CNF_PULL_Disabled << GPIO_PIN_CNF_PULL_Pos)
										| (GPIO_PIN_CNF_INPUT_Connect << GPIO_PIN_CNF_INPUT_Pos)
										| (GPIO_PIN_CNF_DIR_Output << GPIO_PIN_CNF_DIR_Pos);
			break ;

			case OUTPUT_D0H1:
				NRF_GPIO->PIN_CNF[pin] = (GPIO_PIN_CNF_SENSE_Disabled << GPIO_PIN_CNF_SENSE_Pos)
										| (GPIO_PIN_CNF_DRIVE_D0H1 << GPIO_PIN_CNF_DRIVE_Pos)
										| (GPIO_PIN_CNF_PULL_Disabled << GPIO_PIN_CNF_PULL_Pos)
										| (GPIO_PIN_CNF_INPUT_Connect << GPIO_PIN_CNF_INPUT_Pos)
										| (GPIO_PIN_CNF_DIR_Output << GPIO_PIN_CNF_DIR_Pos);
			break ;

			case OUTPUT_S0D1:
				NRF_GPIO->PIN_CNF[pin] = (GPIO_PIN_CNF_SENSE_Disabled << GPIO_PIN_CNF_SENSE_Pos)
										| (GPIO_PIN_CNF_DRIVE_S0D1 << GPIO_PIN_CNF_DRIVE_Pos)
										| (GPIO_PIN_CNF_PULL_Disabled << GPIO_PIN_CNF_PULL_Pos)
										| (GPIO_PIN_CNF_INPUT_Connect << GPIO_PIN_CNF_INPUT_Pos)
										| (GPIO_PIN_CNF_DIR_Output << GPIO_PIN_CNF_DIR_Pos);
			break ;

			case OUTPUT_H0D1:
				NRF_GPIO->PIN_CNF[pin] = (GPIO_PIN_CNF_SENSE_Disabled << GPIO_PIN_CNF_SENSE_Pos)
										| (GPIO_PIN_CNF_DRIVE_H0D1 << GPIO_PIN_CNF_DRIVE_Pos)
										| (GPIO_PIN_CNF_PULL_Disabled << GPIO_PIN_CNF_PULL_Pos)
										| (GPIO_PIN_CNF_INPUT_Connect << GPIO_PIN_CNF_INPUT_Pos)
										| (GPIO_PIN_CNF_DIR_Output << GPIO_PIN_CNF_DIR_Pos);
			break ;
		}
	}
}
/**********************************************************************
name :
function : 
**********************************************************************/
void digitalWrite( uint8_t ulPin, uint32_t ulVal )
{
	uint8_t pin;
	pin = Pin_nRF51822_to_Arduino(ulPin);
	if(pin < 31)
	{
		if (ulVal)
			NRF_GPIO->OUTSET = (1 << pin);
		else
			NRF_GPIO->OUTCLR = (1 << pin);
	}
}
/**********************************************************************
name :
function : 
**********************************************************************/
int digitalRead( uint8_t ulPin )
{
	uint8_t pin;
	pin = Pin_nRF51822_to_Arduino(ulPin);
	if(pin < 31)
	{	
		return ((NRF_GPIO->IN >> pin) & 1UL);
	}
}

#ifdef __cplusplus
}
#endif
