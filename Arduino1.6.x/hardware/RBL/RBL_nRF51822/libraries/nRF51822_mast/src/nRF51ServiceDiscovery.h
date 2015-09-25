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
#ifndef NRF51_SERVICE_DISCOVERY_H__
#define NRF51_SERVICE_DISCOVERY_H__

#include "ble_api/DiscoveredService.h"
#include "ble_api/DiscoveredCharacteristic.h"
#include "ble_api/DiscoveredDevice.h"

#include "ble_api/GattClient.h"

#include "projectconfig.h"

#include "ble.h"
#include "ble_gattc.h"

// class ServiceUUIDDiscoveryQueue {
// public:
	// ServiceUUIDDiscoveryQueue() :
		// numIndices(0),
		// serviceIndices() {
		// /* empty */
	// }

// public:

	// void reset(void) {
		// numIndices = 0;
		// for (unsigned i = 0; i < CFG_DISCOVERED_SERVICE_NUM; i++) {
			// serviceIndices[i] = INVALID_INDEX;
		// }
	// }
	
	// void enqueue(int serviceIndex) {
		// serviceIndices[numIndices++] = serviceIndex;
	// }
	
	// int dequeue(void) {
		// if (numIndices == 0) {
			// return INVALID_INDEX;
		// }

		// unsigned valueToReturn = serviceIndices[0];
		// numIndices--;
		// for (unsigned i = 0; i < numIndices; i++) {
			// serviceIndices[i] = serviceIndices[i + 1];
		// }

		// return valueToReturn;
	// }
	
	// unsigned getFirst(void) const {
		// return serviceIndices[0];
	// }
	
	// size_t getCount(void) const {
		// return numIndices;
	// }

	// /**
	 // * Trigger UUID discovery for the first of the enqueued ServiceIndices.
	 // */
	// void triggerFirst(void);

// private:
	// static const int INVALID_INDEX = -1;

// private:
	// size_t numIndices;
	// int    serviceIndices[CFG_DISCOVERED_SERVICE_NUM];
// };


// /**
 // * A datatype to contain characteristic-indices for which long UUIDs need to
 // * be discovered using read_val_by_uuid().
 // */
// class CharUUIDDiscoveryQueue {
// public:
	// CharUUIDDiscoveryQueue() :
		// numIndices(0),
		// charIndices() {
		// /* empty */
	// }

// public:
	// void reset(void) {
		// numIndices = 0;
		// for (unsigned i = 0; i < CFG_DISCOVERED_CHARS_NUM; i++) {
			// charIndices[i] = INVALID_INDEX;
		// }
	// }
	// void enqueue(int serviceIndex) {
		// charIndices[numIndices++] = serviceIndex;
	// }
	// int dequeue(void) {
		// if (numIndices == 0) {
			// return INVALID_INDEX;
		// }

		// unsigned valueToReturn = charIndices[0];
		// numIndices--;
		// for (unsigned i = 0; i < numIndices; i++) {
			// charIndices[i] = charIndices[i + 1];
		// }

		// return valueToReturn;
	// }
	// unsigned getFirst(void) const {
		// return charIndices[0];
	// }
	// size_t getCount(void) const {
		// return numIndices;
	// }

	// /**
	 // * Trigger UUID discovery for the first of the enqueued charIndices.
	 // */
	// void triggerFirst(void);

// private:
	// static const int INVALID_INDEX = -1;

// private:
	// size_t numIndices;
	// int    charIndices[CFG_DISCOVERED_CHARS_NUM];
// };



class nRF51ServiceDiscovery 
{
public:
	
	typedef enum{
		INACTIVE,
        SERVICE_DISCOVERY_ACTIVE,
        CHARACTERISTIC_DISCOVERY_ACTIVE,
		DESCRIPTOR_DISCOVERY_ACTIVE,
        DISCOVER_SERVICE_UUIDS,
        DISCOVER_CHARACTERISTIC_UUIDS,
	}discoveredState_t;
	
	
private:

	class ServiceUUIDDiscoveryQueue {
		public:
			ServiceUUIDDiscoveryQueue(nRF51ServiceDiscovery *parent) :
				numIndices(0),
				serviceIndices(),
				parentDiscoveryObject(parent) {
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
			
			nRF51ServiceDiscovery *parentDiscoveryObject;
	};
	friend class ServiceUUIDDiscoveryQueue;
	
	/**
	 * A datatype to contain characteristic-indices for which long UUIDs need to
	 * be discovered using read_val_by_uuid().
	 */
	class CharUUIDDiscoveryQueue {
		public:
			CharUUIDDiscoveryQueue(nRF51ServiceDiscovery *parent) :
				numIndices(0),
				charIndices(),
				parentDiscoveryObject(parent) 	{
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
			
			nRF51ServiceDiscovery *parentDiscoveryObject;	
	};
	friend class CharUUIDDiscoveryQueue;
	
public:

	nRF51ServiceDiscovery() : connHandle(0xFFFF),
							  endHandleDescriptor(0xFFFF),
							  descriptorState(0),
							  state(INACTIVE),
							  serviceUUIDDiscoveryQueue(this),
							  charUUIDDiscoveryQueue(this)
	{
			
	}
	
	bool isActive(void) const {
        return state != INACTIVE;
    }
	
	void setDeviceCallback(GattClient::DiscoveredCallback_t Callback) {
		discoveryCallback = Callback;
	}
	
	void setTerminateDiscoveryCallback(GattClient::TerminationCallback_t Callback) {
		terminateDiscoveryCallback = Callback;
	}
	
	void discoveryStarted(Gap::Handle_t connectionHandle) {
		connHandle = connectionHandle;
		DiscoveredInfo.resetDiscoveredDevice(connectionHandle);
	}
	
	void serviceDiscoveryStart(void){
		state = SERVICE_DISCOVERY_ACTIVE;
	}

	void characteristicsDiscoveryStart(void) {
		state = CHARACTERISTIC_DISCOVERY_ACTIVE;
	}
	
	void descriptorDiscoveryStart(void) {
		state = DESCRIPTOR_DISCOVERY_ACTIVE;
	}	
	
	void nextServiceIndex(void) {
		DiscoveredInfo.serviceIndex++;
	}
	
	void setDescriptorState(uint8_t state){
		descriptorState = state;
	}
	
public:

	uint32_t startDiscoveryCharacteristic(Gap::Handle_t connectionHandle,
										  Gap::Handle_t startHandle,
										  Gap::Handle_t endHandle);
	uint32_t startDiscoveryDescriptor(Gap::Handle_t connectionHandle,
									  Gap::Handle_t startHandle,
									  Gap::Handle_t endHandle);
										  
	void setupDiscoveredServices(const ble_gattc_evt_prim_srvc_disc_rsp_t *response);
	void setupDiscoveredCharacteristics(const ble_gattc_evt_char_disc_rsp_t *response);
	void setupDiscoveredDescriptor(const ble_gattc_evt_desc_disc_rsp_t *respone);
	
	void progressServiceDiscovery(void);
	void progressCharacteristicDiscovery(void);
	void progressDescriptorDiscovery(void);
	
	void progressDiscoverUUIDResponse(const ble_gattc_evt_char_val_by_uuid_read_rsp_t *response);
	
	void nextServiceIncludeDiscovery(void);
	void startCharsDescriptorDiscovery(void);
	
	
private:
	
	uint8_t 					descriptorState;
	GattAttribute::Handle_t		endHandleDescriptor;
	
	Gap::Handle_t            	connHandle; 

	discoveredState_t			state;
	DiscoveredDevice			DiscoveredInfo;
	ServiceUUIDDiscoveryQueue   serviceUUIDDiscoveryQueue;
    CharUUIDDiscoveryQueue      charUUIDDiscoveryQueue;	
	
	GattClient::DiscoveredCallback_t 	discoveryCallback;
	GattClient::TerminationCallback_t	terminateDiscoveryCallback;
};


#endif

