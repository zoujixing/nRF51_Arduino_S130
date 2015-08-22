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
#ifndef NRF51_DISCOVERY_DEVICE_H__
#define NRF51_DISCOVERY_DEVICE_H__

#include "ble_api/DiscoveredService.h"
#include "ble_api/DiscoveredCharacteristic.h"

#include "nRF51DiscoveredCharacteristic.h"
#include "projectconfig.h"

#include "ble.h"
#include "ble_gattc.h"

class ServiceUUIDDiscoveryQueue {
public:
	ServiceUUIDDiscoveryQueue() :
		numIndices(0),
		serviceIndices() {
		/* empty */
	}

public:

	void reset(void) {
		numIndices = 0;
		for (unsigned i = 0; i < CFG_DISCOVERED_SERVICE_NUM; i++) {
			serviceIndices[i] = INVALID_INDEX;
		}
	}
	
	void enqueue(int serviceIndex) {
		serviceIndices[numIndices++] = serviceIndex;
	}
	
	int dequeue(void) {
		if (numIndices == 0) {
			return INVALID_INDEX;
		}

		unsigned valueToReturn = serviceIndices[0];
		numIndices--;
		for (unsigned i = 0; i < numIndices; i++) {
			serviceIndices[i] = serviceIndices[i + 1];
		}

		return valueToReturn;
	}
	
	unsigned getFirst(void) const {
		return serviceIndices[0];
	}
	
	size_t getCount(void) const {
		return numIndices;
	}

	/**
	 * Trigger UUID discovery for the first of the enqueued ServiceIndices.
	 */
	void triggerFirst(void);

private:
	static const int INVALID_INDEX = -1;

private:
	size_t numIndices;
	int    serviceIndices[CFG_DISCOVERED_SERVICE_NUM];
};


/**
 * A datatype to contain characteristic-indices for which long UUIDs need to
 * be discovered using read_val_by_uuid().
 */
class CharUUIDDiscoveryQueue {
public:
	CharUUIDDiscoveryQueue() :
		numIndices(0),
		charIndices() {
		/* empty */
	}

public:
	void reset(void) {
		numIndices = 0;
		for (unsigned i = 0; i < CFG_DISCOVERED_CHARS_NUM; i++) {
			charIndices[i] = INVALID_INDEX;
		}
	}
	void enqueue(int serviceIndex) {
		charIndices[numIndices++] = serviceIndex;
	}
	int dequeue(void) {
		if (numIndices == 0) {
			return INVALID_INDEX;
		}

		unsigned valueToReturn = charIndices[0];
		numIndices--;
		for (unsigned i = 0; i < numIndices; i++) {
			charIndices[i] = charIndices[i + 1];
		}

		return valueToReturn;
	}
	unsigned getFirst(void) const {
		return charIndices[0];
	}
	size_t getCount(void) const {
		return numIndices;
	}

	/**
	 * Trigger UUID discovery for the first of the enqueued charIndices.
	 */
	void triggerFirst(void);

private:
	static const int INVALID_INDEX = -1;

private:
	size_t numIndices;
	int    charIndices[CFG_DISCOVERED_CHARS_NUM];
};

class nRF51DiscoveredDevice 
{
public:
	
	typedef struct{
		
		uint8_t							charsIndex;
		uint8_t 						numChars;
		DiscoveredService				service;
		nRF51DiscoveredCharacteristic	chars[CFG_DISCOVERED_CHARS_NUM];
	}service_t;
	
	typedef enum{
		INACTIVE,
        SERVICE_DISCOVERY_ACTIVE,
        CHARACTERISTIC_DISCOVERY_ACTIVE,
        DISCOVER_SERVICE_UUIDS,
        DISCOVER_CHARACTERISTIC_UUIDS,
	}discoveredState_t;
	
public:

	nRF51DiscoveredDevice() {
		
		serviceIndex = 0;
		numServices  = 0;
		serviceMax	 = 0;
	}
	
	const uint8_t& getNumServies(void) const {
        return numServices;
    }
	
	const uint8_t& getServieMax(void) const {
        return serviceMax;
    }
	
	const uint8_t& getServieIndex(void) const {
        return serviceIndex;
    }	
	
private:
	void setupDiscoveredServices(const ble_gattc_evt_prim_srvc_disc_rsp_t *response);
	void setupDiscoveredCharacteristics(const ble_gattc_evt_char_disc_rsp_t *response);



private:

	uint8_t 	serviceIndex;
	uint8_t 	numServices;
	uint8_t		serviceMax;
	service_t	service[CFG_DISCOVERED_SERVICE_NUM];
	
	ServiceUUIDDiscoveryQueue   serviceUUIDDiscoveryQueue;
    CharUUIDDiscoveryQueue      charUUIDDiscoveryQueue;
};


#endif

