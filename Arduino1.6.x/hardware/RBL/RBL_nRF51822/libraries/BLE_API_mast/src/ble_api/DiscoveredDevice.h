/* mbed Microcontroller Library
 * Copyright (c) 2006-2013 ARM Limited
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef DISCOVERED_DEVICE_H_
#define DISCOVERED_DEVICE_H_
 
#include "Gap.h"
#include "DiscoveredService.h"
#include "DiscoveredCharacteristic.h" 
 
#define DEVICE_SERVICE_MAX_NUM			4
#define DEVICE_CHARS_OF_SERVICE_MAX_NUM	4
 
class DiscoveredDevice
{
public:

	typedef struct{
		
		uint8_t							charsIndex;
		uint8_t 						preNumChars;
		uint8_t 						currNumChars;
		DiscoveredService				service;
		DiscoveredCharacteristic		chars[DEVICE_CHARS_OF_SERVICE_MAX_NUM];
	}service_t;	

public:
	
	DiscoveredDevice() :  connHandle(0xFFFF),
					      serviceIndex(0),
						  preNumService(0),
						  currNumServices(0) {
		uint8_t index;
		
		for(index=0; index<DEVICE_SERVICE_MAX_NUM;index++) {
			service[index].charsIndex = 0;
			service[index].preNumChars=0;
			service[index].currNumChars=0;			
		}
	};

	void resetDiscoveredDevice(Gap::Handle_t handle) {
		connHandle = handle;
		serviceIndex = 0;
		preNumService = 0;
		currNumServices = 0;
		for(uint8_t index=0; index<DEVICE_SERVICE_MAX_NUM;index++) {
			service[index].charsIndex = 0;
			service[index].preNumChars=0;
			service[index].currNumChars=0;			
		}
	}
	
public:

	uint8_t 	connHandle;
	uint8_t 	serviceIndex;
	uint8_t		preNumService;
	uint8_t 	currNumServices;
	service_t	service[DEVICE_SERVICE_MAX_NUM];
};
 
#endif
 