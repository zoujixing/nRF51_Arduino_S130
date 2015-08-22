
#ifndef NRF51_GAP_SCAN_REPORT_H__
#define NRF51_GAP_SCAN_REPORT_H__

#include <string.h>
#include <stdint.h>
#include "projectconfig.h"
#include "nrf_error.h"
#include "nRF51Gap.h"

class ScanReport
{
public : 
	
	typedef struct{
		Gap::peer_addr_t 			peer_addr;
		int8_t						rssi;
		uint8_t						adv_type;
		uint8_t						adv_data_len;
		uint8_t 					adv_data[31];
	}ScanReport_t;
	
private : 
	
	uint8_t 		count;
	uint8_t 		index;
	ScanReport_t	list[CFG_SCAN_REPORT_LIST_NUM];
	
public : 
	
	ScanReport() {
		count = 0;
		index = 0;
		memset(list, 0x00,sizeof(list));
	}
	
	uint32_t reset() {
		count = 0;
		index = 0;
		memset(list, 0x00,sizeof(list));		
	}
	
	uint32_t get_count(void) {
		return count;
	}
	
	uint32_t add(const Gap::AdvertisementCallbackParams_t *params);
	uint32_t print(void);
};




#endif

