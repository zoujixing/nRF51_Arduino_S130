
#include "nRF51GapScanReport.h"
#include "Arduino.h"

uint32_t ScanReport::add(const Gap::AdvertisementCallbackParams_t *params)
{
	uint8_t index;
	ScanReport_t data;
	
	if(params->isScanResponse == 1)
		return NRF_ERROR_NOT_SUPPORTED;
		
	if(this->count >= CFG_SCAN_REPORT_LIST_NUM)
	{
		return NRF_ERROR_NO_MEM;
	}
	
	data.peer_addr.addr_type = params->peerAddrType;
	memcpy(data.peer_addr.addr, params->peerAddr, 6);
	
    data.rssi=params->rssi;
    data.adv_type = params->type;
    data.adv_data_len = params->advertisingDataLen;
    memcpy(data.adv_data, params->advertisingData, data.adv_data_len); 

	for(index=0; index<count; index++)
	{
		if( 0x00 == memcmp(list[index].peer_addr.addr, data.peer_addr.addr, BLE_GAP_ADDR_LEN) )
		{
			return NRF_ERROR_BUSY;
		}
	}
	
	memcpy(&list[this->count], &data, sizeof(ScanReport_t));
	this->count++;
	
	return NRF_SUCCESS;
}

uint32_t ScanReport::print(void)
{
	uint8_t index, index_;
    for(index=0; index<this->count; index++)
    {
        Serial.print("The index : ");
        Serial.println(index, HEX);
        Serial.print("The addr type : ");
        Serial.println(list[index].peer_addr.addr_type, HEX);
        Serial.print("The addr is : ");
        for(index_=0; index_<6; index_++)
        {
            Serial.print(list[index].peer_addr.addr[index_], HEX);
            Serial.print(" ");  
        }
        Serial.println(" ");  
        
        Serial.print("The rssi : ");
        Serial.println(list[index].rssi, DEC);
        
        Serial.print("The adv_type : ");
        Serial.println(list[index].adv_type, HEX);
        
        Serial.println("The adv_data : ");
        Serial.println((const char *)list[index].adv_data);
    }	
}
