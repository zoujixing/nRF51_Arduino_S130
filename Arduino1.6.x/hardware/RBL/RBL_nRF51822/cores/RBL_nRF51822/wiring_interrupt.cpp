
#include "wiring_interrupt.h"
#include "nrf_soc.h"
#include "nrf_sdm.h"

static pinHandle_t pinHandle[31];
static uint8_t 	   channelEnabled[31]={0};
static uint32_t	   portRISE = 0;
static uint32_t	   portFALL = 0;

extern UARTClass Serial;

void attachInterrupt(uint8_t pin, irqHandle_t handle, uint8_t mode)
{
	uint8_t nrf_pin;
	uint8_t softdevice_enabled;
	uint32_t err_code = NRF_SUCCESS;
	
	nrf_pin = Pin_nRF51822_to_Arduino(pin);
	if(nrf_pin >= 31) 
		return;
	
	if(mode != CHANGE && mode != FALLING && mode != RISING )
		return;
	// register handle
	if(mode == CHANGE) {
		pinHandle[nrf_pin].riseHandle = handle;
		pinHandle[nrf_pin].fallHandle = handle;
	}
	else if(mode == RISING) {
		pinHandle[nrf_pin].riseHandle = handle;
		pinHandle[nrf_pin].fallHandle = NULL;
	}
	else {
		pinHandle[nrf_pin].riseHandle = NULL;
		pinHandle[nrf_pin].fallHandle = handle;	
	}
	// Config pin input_pull
	NRF_GPIO->PIN_CNF[nrf_pin] = (GPIO_PIN_CNF_SENSE_Disabled << GPIO_PIN_CNF_SENSE_Pos) | 
								 (GPIO_PIN_CNF_DRIVE_S0S1 << GPIO_PIN_CNF_DRIVE_Pos)	 |
						 		 (GPIO_PIN_CNF_PULL_Pullup << GPIO_PIN_CNF_PULL_Pos)	 |
						 		 (GPIO_PIN_CNF_INPUT_Connect << GPIO_PIN_CNF_INPUT_Pos)	 |
						 		 (GPIO_PIN_CNF_DIR_Input << GPIO_PIN_CNF_DIR_Pos);
						
	// Config pin event mode
	NRF_GPIO->PIN_CNF[nrf_pin] &= ~(GPIO_PIN_CNF_SENSE_Msk);

	if (mode == RISING) {
		portRISE |= (1 << nrf_pin);
	} 
	else if (mode == FALLING) {
		portFALL |= (1 << nrf_pin);
	}
	else {
		portRISE |= (1 << nrf_pin);
		portFALL |= (1 << nrf_pin);
	}
	
    if (((portRISE >> nrf_pin) & 1) || ((portFALL >>nrf_pin) & 1)) {
        if ((NRF_GPIO->IN >> nrf_pin) & 1) {
            NRF_GPIO->PIN_CNF[nrf_pin] |= (GPIO_PIN_CNF_SENSE_Low << GPIO_PIN_CNF_SENSE_Pos);    // | (GPIO_PIN_CNF_PULL_Disabled << GPIO_PIN_CNF_PULL_Pos);
        } else {
            NRF_GPIO->PIN_CNF[nrf_pin] |= (GPIO_PIN_CNF_SENSE_High << GPIO_PIN_CNF_SENSE_Pos);     //| (GPIO_PIN_CNF_PULL_Disabled << GPIO_PIN_CNF_PULL_Pos);
        }
    }	
	
	// Config irq
	channelEnabled[nrf_pin] = 1;
	
	NRF_GPIOTE->EVENTS_PORT = 0;
	NRF_GPIOTE->INTENSET = GPIOTE_INTENSET_PORT_Set << GPIOTE_INTENSET_PORT_Pos;
	
	err_code = sd_softdevice_is_enabled(&softdevice_enabled);
	APP_ERROR_CHECK(err_code);
	if(softdevice_enabled == 0)
	{	
		NVIC_SetPriority(GPIOTE_IRQn, APP_IRQ_PRIORITY_LOW);
		NVIC_EnableIRQ(GPIOTE_IRQn);
	}
	else
	{
		err_code = sd_nvic_SetPriority(GPIOTE_IRQn, APP_IRQ_PRIORITY_LOW);
		APP_ERROR_CHECK(err_code);
		err_code = sd_nvic_EnableIRQ(GPIOTE_IRQn);
		APP_ERROR_CHECK(err_code);
	}
		
	//NVIC_SetPriority(GPIOTE_IRQn, 3);
    //NVIC_EnableIRQ(GPIOTE_IRQn);
}

void detachInterrupt(uint8_t pin )
{
	uint8_t nrf_pin;
	
	nrf_pin = Pin_nRF51822_to_Arduino(pin);
	if(nrf_pin >= 31)
		return;
	
	memset(&pinHandle[nrf_pin], 0x00, sizeof(pinHandle_t));
	channelEnabled[nrf_pin]=0;
	portRISE &= ~(1 << nrf_pin);
	portFALL &= ~(1 << nrf_pin);
}

static void irq_handle(uint8_t pin, gpio_irq_event event)
{
	uint8_t  pin_state;
	uint32_t cnt=0, cnt1=0;
	
	switch(event)
	{
		case IRQ_RISE:
		{
			pin_state = ( (NRF_GPIO->IN >> pin) & 1UL );
			for(cnt=0; cnt<100000; cnt++) {
				if(cnt >= 99999) {
					return;
				}
				if(((NRF_GPIO->IN >> pin) & 1UL ) != pin_state) {
					pin_state = ( (NRF_GPIO->IN >> pin ) & 1UL );
					cnt1 = 0;
				}
				else{
					//wait about 1ms to make sure it's not voltage jitter
					if(cnt1++ > 5000) {
						break;
					}
				}
			}
			
			NVIC_ClearPendingIRQ(GPIOTE_IRQn);
			
			if( (NRF_GPIO->IN >> pin ) & 1UL ) {
				if(pinHandle[pin].riseHandle != NULL)
					(pinHandle[pin].riseHandle)(); 
			}
		}break;
		case IRQ_FALL:
		{
			pin_state = ( (NRF_GPIO->IN >> pin ) & 1UL );
			for(cnt=0; cnt<100000; cnt++) {
				if(cnt >= 99999) {
					return;
				}
				if(((NRF_GPIO->IN >> pin ) & 1UL ) != pin_state) {
					pin_state = ( (NRF_GPIO->IN >> pin ) & 1UL );
					cnt1 = 0;
				}
				else{
					//wait about 1ms to make sure it's not voltage jitter
					if(cnt1++ > 5000) {
						break;
					}
				}
			}
			
			NVIC_ClearPendingIRQ(GPIOTE_IRQn);
			
			if( ((NRF_GPIO->IN >> pin ) & 1UL) == 0) {
				if(pinHandle[pin].fallHandle != NULL)
					(pinHandle[pin].fallHandle)(); 
			}
		}break;
		case IRQ_NONE: 
		break;
	}
}



void GPIOTE_IRQHandler(void)
{
	volatile uint32_t newVal = NRF_GPIO->IN;
	
	//Serial.println("irq");
	if ((NRF_GPIOTE->EVENTS_PORT != 0) && ((NRF_GPIOTE->INTENSET & GPIOTE_INTENSET_PORT_Msk) != 0)) {
        NRF_GPIOTE->EVENTS_PORT = 0;
	
		for(uint8_t i=0; i<31; i++){
			if(channelEnabled[i]) {
				if( ((newVal>>i)&1)  && ( ( (NRF_GPIO->PIN_CNF[i] >>GPIO_PIN_CNF_SENSE_Pos) & GPIO_PIN_CNF_SENSE_Low) != GPIO_PIN_CNF_SENSE_Low) && ( (portRISE>>i)&1) ) {
					irq_handle(i, IRQ_RISE);
				}
				else if((((newVal >> i) & 1) == 0) &&
                         (((NRF_GPIO->PIN_CNF[i] >> GPIO_PIN_CNF_SENSE_Pos) & GPIO_PIN_CNF_SENSE_Low) == GPIO_PIN_CNF_SENSE_Low) &&
                         ((portFALL >> i) & 1)) {
					irq_handle(i, IRQ_FALL);		 
				}
			}
			
			if (NRF_GPIO->PIN_CNF[i] & GPIO_PIN_CNF_SENSE_Msk) {
				NRF_GPIO->PIN_CNF[i] &= ~(GPIO_PIN_CNF_SENSE_Msk);

				if (newVal >> i & 1) {
					NRF_GPIO->PIN_CNF[i] |= (GPIO_PIN_CNF_SENSE_Low << GPIO_PIN_CNF_SENSE_Pos);
				} 
				else {
					NRF_GPIO->PIN_CNF[i] |= (GPIO_PIN_CNF_SENSE_High << GPIO_PIN_CNF_SENSE_Pos);
				}
			}
		}
	}
}









