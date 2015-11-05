
/*

    Copyright (c) 2014 RedBearLab, All rights reserved.

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
    See the GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
    EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
    MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
    IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
    CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
    TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
    SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

*/

#include "Wire.h"
#include "nrf_soc.h"
#include "nrf_sdm.h"

/**********************************************************************
name :
function : 
return: 0--SUCCESS, 1--FAIL
**********************************************************************/
bool TwoWire::twi_master_clear_bus(void)
{
    bool bus_clear;
    uint32_t twi_state;
    uint32_t sck_pin_config;
    uint32_t sda_pin_config;
    
    twi_state = twi->ENABLE;
    twi->ENABLE = TWI_ENABLE_ENABLE_Disabled << TWI_ENABLE_ENABLE_Pos;
    //Config I2C pins.
    sck_pin_config = NRF_GPIO->PIN_CNF[this->SCL_Pin]; 
    NRF_GPIO->PIN_CNF[this->SCL_Pin] = (GPIO_PIN_CNF_SENSE_Disabled << GPIO_PIN_CNF_SENSE_Pos) 
                                     | (GPIO_PIN_CNF_DRIVE_S0D1     << GPIO_PIN_CNF_DRIVE_Pos) 
                                     | (GPIO_PIN_CNF_PULL_Pullup    << GPIO_PIN_CNF_PULL_Pos)  
                                     | (GPIO_PIN_CNF_INPUT_Connect  << GPIO_PIN_CNF_INPUT_Pos) 
                                     | (GPIO_PIN_CNF_DIR_Output     << GPIO_PIN_CNF_DIR_Pos);

    sda_pin_config = NRF_GPIO->PIN_CNF[this->SDA_Pin];    
    NRF_GPIO->PIN_CNF[this->SDA_Pin] = (GPIO_PIN_CNF_SENSE_Disabled << GPIO_PIN_CNF_SENSE_Pos) 
                                     | (GPIO_PIN_CNF_DRIVE_S0D1     << GPIO_PIN_CNF_DRIVE_Pos) 
                                     | (GPIO_PIN_CNF_PULL_Pullup    << GPIO_PIN_CNF_PULL_Pos)  
                                     | (GPIO_PIN_CNF_INPUT_Connect  << GPIO_PIN_CNF_INPUT_Pos) 
                                     | (GPIO_PIN_CNF_DIR_Output     << GPIO_PIN_CNF_DIR_Pos);     
                                  
    NRF_GPIO->OUTSET = ( 1 << this->SCL_Pin );
    NRF_GPIO->OUTSET = ( 1 << this->SDA_Pin );
    TWI_DELAY(4);
    
    if( ( (NRF_GPIO->IN >> this->SCL_Pin) & 0X1UL ) && ( (NRF_GPIO->IN >> this->SDA_Pin) & 0x1UL ) )
    {   //I2C bus is free, return success.
        bus_clear = 0;
    }
    else
    {    //Try to release I2C bus.
        uint_fast8_t index;
        bus_clear = 1;
        for( index=18; index--;)
        {
            NRF_GPIO->OUTCLR = ( 1 << SCL_Pin );
            TWI_DELAY(4);
            NRF_GPIO->OUTSET = ( 1 << SDA_Pin );
            TWI_DELAY(4);
            
            if( (NRF_GPIO->IN >> SDA_Pin) & 0x1UL == 1 )
            {
                bus_clear = 0;
                break;
            }
        }
    }
    
    NRF_GPIO->PIN_CNF[this->SCL_Pin] = sck_pin_config;
    NRF_GPIO->PIN_CNF[this->SDA_Pin] = sda_pin_config;

    twi->ENABLE = twi_state;    
    
    return bus_clear;
}

/**********************************************************************
name :
function : 
return: 0--SUCCESS, 1--FAIL
**********************************************************************/
bool TwoWire::twi_master_init(void)
{    
    uint8_t softdevice_enabled;
    uint32_t err_code = NRF_SUCCESS;

    NRF_GPIO->PIN_CNF[this->SCL_Pin] = (GPIO_PIN_CNF_SENSE_Disabled << GPIO_PIN_CNF_SENSE_Pos)
                                     | (GPIO_PIN_CNF_DRIVE_S0D1     << GPIO_PIN_CNF_DRIVE_Pos)
                                     | (GPIO_PIN_CNF_PULL_Pullup    << GPIO_PIN_CNF_PULL_Pos)
                                     | (GPIO_PIN_CNF_INPUT_Connect  << GPIO_PIN_CNF_INPUT_Pos)
                                     | (GPIO_PIN_CNF_DIR_Input      << GPIO_PIN_CNF_DIR_Pos);    
                                  
    NRF_GPIO->PIN_CNF[this->SDA_Pin] = (GPIO_PIN_CNF_SENSE_Disabled << GPIO_PIN_CNF_SENSE_Pos)
                                     | (GPIO_PIN_CNF_DRIVE_S0D1     << GPIO_PIN_CNF_DRIVE_Pos)
                                     | (GPIO_PIN_CNF_PULL_Pullup    << GPIO_PIN_CNF_PULL_Pos)
                                     | (GPIO_PIN_CNF_INPUT_Connect  << GPIO_PIN_CNF_INPUT_Pos)
                                     | (GPIO_PIN_CNF_DIR_Input      << GPIO_PIN_CNF_DIR_Pos);
                                  
    twi->EVENTS_RXDREADY = 0;
    twi->EVENTS_TXDSENT  = 0;
    twi->PSELSCL = this->SCL_Pin;
    twi->PSELSDA = this->SDA_Pin;
    twi->FREQUENCY = twi_frequency; //TWI_FREQUENCY_FREQUENCY_K100 << TWI_FREQUENCY_FREQUENCY_Pos;
    //Config PPI channel for I2C.
    err_code = sd_softdevice_is_enabled(&softdevice_enabled);
    APP_ERROR_CHECK(err_code);
    if (softdevice_enabled == 0)
    {    
        NRF_PPI->CH[this->PPI_channel].EEP = (uint32_t)&twi->EVENTS_BB;
        NRF_PPI->CH[this->PPI_channel].TEP = (uint32_t)&twi->TASKS_SUSPEND;
        NRF_PPI->CHEN &= ~(1 << this->PPI_channel);
    }
    else
    {
        err_code = sd_ppi_channel_assign(this->PPI_channel, &twi->EVENTS_BB, &twi->TASKS_SUSPEND);
        APP_ERROR_CHECK(err_code);
        err_code = sd_ppi_channel_enable_clr(1 << this->PPI_channel);
        APP_ERROR_CHECK(err_code);
    }
    
    twi->ENABLE = TWI_ENABLE_ENABLE_Enabled << TWI_ENABLE_ENABLE_Pos;    
    
    return twi_master_clear_bus();
}
/**********************************************************************
name :
function : 
return: 0--SUCCESS, 1--FAIL
**********************************************************************/
uint8_t TwoWire::twi_master_write(uint8_t *data, uint8_t data_length, uint8_t issue_stop_condition)
{
    uint32_t timeout = MAX_TIMEOUT_LOOPS;
    
    if(data_length == 0)
    {
        return 1;
    }
    twi->TXD = *data++;
    twi->TASKS_STARTTX = 1;
    while(1)
    {   
        while( (twi->EVENTS_TXDSENT == 0) && (--timeout) );//&& (twi->EVENTS_ERROR == 0) );
        
        if( 0 == timeout )//|| twi->EVENTS_ERROR != 0)
        {    //If timeout, Reinitializes I2C.
            twi->EVENTS_ERROR = 0;
            twi->ENABLE    = TWI_ENABLE_ENABLE_Disabled << TWI_ENABLE_ENABLE_Pos; 
            twi->POWER = 0; 
            TWI_DELAY(5);
            twi->POWER = 1; 
            twi->ENABLE    = TWI_ENABLE_ENABLE_Enabled << TWI_ENABLE_ENABLE_Pos;
            
            twi_master_init();
            return 1;
        }
        twi->EVENTS_TXDSENT = 0;
        if( --data_length == 0)
        {    
            break;
        }
        
        twi->TXD = *data++;
    }
    if(issue_stop_condition)
    {    
        twi->EVENTS_STOPPED = 0;
        twi->TASKS_STOP     = 1;
        while(twi->EVENTS_STOPPED == 0)
        {
            //do nothing, wait for stop sequence is sent
        }        
    }
    return 0;
}
/**********************************************************************
name :
function : 
return: 0--SUCCESS, 1--FAIL
**********************************************************************/
uint8_t TwoWire::twi_master_read(uint8_t *data, uint8_t data_length, uint8_t issue_stop_condition)
{    
    uint8_t softdevice_enabled;
    uint32_t timeout = MAX_TIMEOUT_LOOPS, err_code = NRF_SUCCESS;
    
    err_code = sd_softdevice_is_enabled(&softdevice_enabled);
    APP_ERROR_CHECK(err_code);
    if( 0 == data_length )
    {
        return 1;
    }
    else if( 1== data_length )//&& issue_stop_condition == 1)
    {
        if (softdevice_enabled == 0)
        {    
            NRF_PPI->CH[this->PPI_channel].EEP = (uint32_t)&twi->EVENTS_BB;
            NRF_PPI->CH[this->PPI_channel].TEP = (uint32_t)&twi->TASKS_STOP;
            NRF_PPI->CHEN |= (1 << this->PPI_channel);
        }
        else
        {
            err_code = sd_ppi_channel_assign(this->PPI_channel, &twi->EVENTS_BB, &twi->TASKS_STOP);
            APP_ERROR_CHECK(err_code);
            err_code = sd_ppi_channel_enable_set(1 << this->PPI_channel);
            APP_ERROR_CHECK(err_code);
        }
    }
    else
    {    
        if (softdevice_enabled == 0)
        {    
            NRF_PPI->CH[this->PPI_channel].EEP = (uint32_t)&twi->EVENTS_BB;
            NRF_PPI->CH[this->PPI_channel].TEP = (uint32_t)&twi->TASKS_SUSPEND;
            NRF_PPI->CHEN |= (1 << this->PPI_channel);
        }
        else
        {
            err_code = sd_ppi_channel_assign(this->PPI_channel, &twi->EVENTS_BB, &twi->TASKS_SUSPEND);
            APP_ERROR_CHECK(err_code);
            err_code = sd_ppi_channel_enable_set(1 << this->PPI_channel);
            APP_ERROR_CHECK(err_code);            
        }
    }
    
    twi->EVENTS_RXDREADY = 0;
    twi->TASKS_STARTRX = 1;
    
    while(1)
    {    
        while( twi->EVENTS_RXDREADY == 0 && (--timeout) )
        {
            //do nothing, just wait
        }
    
        if( timeout == 0 )
        {    
            twi->EVENTS_ERROR = 0;
            twi->ENABLE       = TWI_ENABLE_ENABLE_Disabled << TWI_ENABLE_ENABLE_Pos; 
            twi->POWER        = 0; 
            TWI_DELAY(5);
            twi->POWER        = 1; 
            twi->ENABLE       = TWI_ENABLE_ENABLE_Enabled << TWI_ENABLE_ENABLE_Pos;
    
            twi_master_init();          
          
            return 1;            
        }
        
        twi->EVENTS_RXDREADY = 0;
        *data++ = twi->RXD;
    
        if( --data_length == 1 )
        {    
            if (softdevice_enabled == 0)
            {    
                //NRF_PPI->CH[this->PPI_channel].EEP = (uint32_t)&twi->EVENTS_BB;
                NRF_PPI->CH[this->PPI_channel].TEP = (uint32_t)&twi->TASKS_STOP;
            }
            else
            {
                err_code = sd_ppi_channel_assign(this->PPI_channel, &twi->EVENTS_BB, &twi->TASKS_STOP);
                APP_ERROR_CHECK(err_code);
            }
        }
        
        if( data_length == 0 )
        {    
            twi->TASKS_STOP = 1;
            break;
        }
        TWI_DELAY(20);
        twi->TASKS_RESUME = 1;
    }
    while( twi->EVENTS_STOPPED == 0 )
    {
        //do nothing
    }

    twi->EVENTS_STOPPED = 0;
    
    if (softdevice_enabled == 0)
    {        
        NRF_PPI->CHEN &= ~(1 << this->PPI_channel);
    }
    else
    {
        err_code = sd_ppi_channel_enable_clr( 1 << this->PPI_channel );
        APP_ERROR_CHECK(err_code);
    }
    return 0;
}

/**********************************************************************
name :
function : 
**********************************************************************/
TwoWire::TwoWire(NRF_TWI_Type *twi) : 
                    twi(twi), RX_BufferIndex(0), RX_BufferLength(0),
                    TX_BufferIndex(0), TX_BufferLength(0), Transfer_Addr(0),
                    twi_status(UNINITIALIZED)
{
    if(twi == NRF_TWI0)
        PPI_channel = PPI_CHANNEL_FOR_TWI0;
    else if(twi = NRF_TWI1)
        PPI_channel = PPI_CHANNEL_FOR_TWI1;
}
/**********************************************************************
name :
function : 
**********************************************************************/
void TwoWire::begin(uint32_t scl, uint32_t sda, uint8_t speed)
{    
    uint32_t nrf_pin1, nrf_pin2;
    
    if( speed == 2 )
    {
        this->twi_frequency = TWI_FREQUENCY_FREQUENCY_K400;
    }
    else if( speed == 1 )
    {
        this->twi_frequency = TWI_FREQUENCY_FREQUENCY_K250;
    }
    else
    {
        this->twi_frequency = TWI_FREQUENCY_FREQUENCY_K100;    
    }
    nrf_pin1 = Pin_nRF51822_to_Arduino(scl);
    nrf_pin2 = Pin_nRF51822_to_Arduino(sda);
    
    if( nrf_pin1<31 && nrf_pin2<31 )
    { 
        this->SCL_Pin = nrf_pin1;
        this->SDA_Pin = nrf_pin2;
        twi_master_init();

        twi_status = MASTER_IDLE;    
    }
}
/**********************************************************************
name :
function : 
**********************************************************************/
void TwoWire::begin()
{
    if(this->twi == NRF_TWI0)
        begin(TWI0_SCL_PIN, TWI0_SDA_PIN, 0);
    else if(this->twi == NRF_TWI1)
        begin(TWI1_SCL_PIN, TWI1_SDA_PIN, 0);
}
/**********************************************************************
name :
function : 
**********************************************************************/
void TwoWire::beginTransmission( uint8_t address )
{
    Transfer_Addr = address;
    TX_BufferIndex = 0;
    twi_status = MASTER_SEND;
}
/**********************************************************************
name :
function : 
**********************************************************************/
void TwoWire::beginTransmission( int address )
{
    beginTransmission( (uint8_t)address );
}
/**********************************************************************
name :
function : 
return: 0--SUCCESS, 1--FAIL
**********************************************************************/
uint8_t TwoWire::endTransmission( uint8_t stop)
{
    uint8_t twi_flag=1;
    
    if(TX_BufferLength > 0 && !twi_master_clear_bus() )
    {
        twi->ADDRESS = Transfer_Addr;
        twi_flag = twi_master_write(TX_Buffer, TX_BufferLength, stop);
    }
    
    TX_BufferLength = 0;
    twi_status = MASTER_IDLE;
    
    return twi_flag;
}
/**********************************************************************
name :
function : 
return: 0--SUCCESS, 1--FAIL
**********************************************************************/
uint8_t TwoWire::endTransmission(void)
{
    uint8_t twi_flag;
    
    twi_flag = endTransmission(1);
    
    return twi_flag;
}
/**********************************************************************
name :
function : 
return: 0--SUCCESS, 1--FAIL
**********************************************************************/
size_t TwoWire::write(uint8_t data)
{
    if(twi_status == MASTER_SEND)
    {    
        if(TX_BufferLength >= BUFF_MAX_LENGTH)
        {
            return 1;
        }
        TX_Buffer[TX_BufferLength++] = data;
        return 0;
    }
    else
    {
        return 1;
    }
}
/**********************************************************************
name :
function : 
return: the length of data that to be written
**********************************************************************/
size_t TwoWire::write(const uint8_t *data, size_t quantity )
{
    if( twi_status == MASTER_SEND )
    {
        for( size_t i=0; i<quantity; ++i )
        {    
            if(TX_BufferLength >= BUFF_MAX_LENGTH)
            {
                return i;
            }
            TX_Buffer[TX_BufferLength++] = data[i];
        }
    }
    else
    {
        return 0;
    }
    
    return quantity;
}
/**********************************************************************
name :
function : 
return: the length of data that to read
**********************************************************************/
uint8_t TwoWire::requestFrom(uint8_t addr, uint8_t quantity, uint8_t stop)
{
    uint8_t flag=1;
    uint8_t read_num = 0;
    
    if(quantity > BUFF_MAX_LENGTH)  
        quantity = BUFF_MAX_LENGTH;

    read_num = quantity;
    if(quantity > 0 && !twi_master_clear_bus() )
    {   
        twi->ADDRESS = addr;
        flag = twi_master_read(RX_Buffer, quantity, stop);
    }

    if(flag != 0)
    {
        read_num = 0;
    }
    else 
    {
        RX_BufferIndex = 0;
        RX_BufferLength = quantity;
    }

    return read_num;
}
/**********************************************************************
name :
function : 
**********************************************************************/
uint8_t TwoWire::requestFrom(uint8_t address, uint8_t quantity)
{
    return requestFrom((uint8_t) address, (uint8_t) quantity, (uint8_t) true);
}
/**********************************************************************
name :
function : 
**********************************************************************/
uint8_t TwoWire::requestFrom(int address, int quantity) 
{
    return requestFrom((uint8_t) address, (uint8_t) quantity, (uint8_t) true);
}
/**********************************************************************
name :
function : 
**********************************************************************/
uint8_t TwoWire::requestFrom(int address, int quantity, int sendStop) 
{
    return requestFrom((uint8_t) address, (uint8_t) quantity, (uint8_t) sendStop);
}
/**********************************************************************
name :
function : 
**********************************************************************/
int TwoWire::available(void)
{
    if(RX_BufferIndex <= RX_BufferLength)
    {
        return (RX_BufferLength - RX_BufferIndex);
    }
    else
    {
        return 0;
    }
}
/**********************************************************************
name :
function : 
**********************************************************************/
int TwoWire::read(void)
{
    if(RX_BufferIndex < RX_BufferLength)
    {
        return RX_Buffer[RX_BufferIndex++];
    }
    else
    {
        return -1;
    }
}
/**********************************************************************
name :
function : 
**********************************************************************/
int TwoWire::peek(void)
{
    if(RX_BufferIndex < RX_BufferLength)
    {
        return RX_Buffer[RX_BufferIndex];
    }
    else
    {
        return -1;
    }
}
/**********************************************************************
name :
function : 
**********************************************************************/
void TwoWire::flush(void)
{
    //do nothing
}


TwoWire Wire = TwoWire(NRF_TWI0);
//TwoWire Wire1 = TwoWire(NRF_TWI1);






