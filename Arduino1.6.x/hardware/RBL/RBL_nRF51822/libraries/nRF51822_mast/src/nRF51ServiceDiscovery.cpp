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
 
#include "nRF51ServiceDiscovery.h"
#include "Arduino.h"

//extern UARTClass Serial;
 
uint32_t 
nRF51ServiceDiscovery::startDiscoveryCharacteristic(Gap::Handle_t connectionHandle,
													Gap::Handle_t startHandle,
													Gap::Handle_t endHandle)
{
    ble_gattc_handle_range_t handleRange = {
        .start_handle = startHandle,
        .end_handle   = endHandle
    };	
	uint32_t rc;
	if ((rc = sd_ble_gattc_characteristics_discover(connectionHandle, &handleRange)) != NRF_SUCCESS) {
		if(terminateDiscoveryCallback)
			terminateDiscoveryCallback(0xB0);
    }

    return rc;
}

uint32_t 
nRF51ServiceDiscovery::startDiscoveryDescriptor(Gap::Handle_t connectionHandle,
													Gap::Handle_t startHandle,
													Gap::Handle_t endHandle)
{
    ble_gattc_handle_range_t handleRange = {
        .start_handle = startHandle,
        .end_handle   = endHandle
    };	
	uint32_t rc;
	if ((rc = sd_ble_gattc_descriptors_discover(connectionHandle, &handleRange)) != NRF_SUCCESS) {
		if(terminateDiscoveryCallback)
			terminateDiscoveryCallback(0xC0);
    }

    return rc;	
}

void 
nRF51ServiceDiscovery::setupDiscoveredServices(const ble_gattc_evt_prim_srvc_disc_rsp_t *response)
{
	uint8_t index = 0; 
	uint8_t responseIndex=0;
	// Add count to numServices
	//Serial.println("setupDiscoveredServices ");
	DiscoveredInfo.preNumService = DiscoveredInfo.currNumServices;
	DiscoveredInfo.currNumServices += response->count; 
	// Make sure not overflow
	if(DiscoveredInfo.currNumServices > DEVICE_SERVICE_MAX_NUM) 
		DiscoveredInfo.currNumServices = DEVICE_SERVICE_MAX_NUM;
	
	serviceUUIDDiscoveryQueue.reset();
	for(index=DiscoveredInfo.preNumService; index<DiscoveredInfo.currNumServices;index++,responseIndex++) {
		if (response->services[responseIndex].uuid.type == BLE_UUID_TYPE_UNKNOWN) {	
			//Serial.println("BLE_UUID_TYPE_UNKNOWN ");
			serviceUUIDDiscoveryQueue.enqueue(index);
			DiscoveredInfo.service[index].service.setup(response->services[responseIndex].handle_range.start_handle,
														response->services[responseIndex].handle_range.end_handle);
		}
		else {	
			//Serial.println("BLE_UUID_TYPE_KNOWN ");
			DiscoveredInfo.service[index].service.setup(response->services[responseIndex].uuid.uuid,
														response->services[responseIndex].handle_range.start_handle,
														response->services[responseIndex].handle_range.end_handle);
		}
	}
	
	/* Trigger discovery of service UUID if necessary. */
    if (serviceUUIDDiscoveryQueue.getCount()) {
        serviceUUIDDiscoveryQueue.triggerFirst();
    }
}

void
nRF51ServiceDiscovery::progressServiceDiscovery(void)
{
	/* Relaunch discovery of new services beyond the last entry cached. */
	if((state == SERVICE_DISCOVERY_ACTIVE) && (DiscoveredInfo.preNumService < DiscoveredInfo.currNumServices)) {
		DiscoveredInfo.preNumService = DiscoveredInfo.currNumServices;
		/* Determine the ending handle of the last cached service. */
		Gap::Handle_t endHandle = DiscoveredInfo.service[DiscoveredInfo.currNumServices - 1].service.getEndHandle();
		if(endHandle == 0xFFFF) {
			//Serial.println("serviceDiscover end ");
			//for(uint8_t index=0; index<DiscoveredInfo.currNumServices; index++) {
				//Serial.print("service index : ");	
				//Serial.println(index, DEC);
				//Serial.print("service uuid : ");
				//Serial.println(DiscoveredInfo.service[index].service.getUUID().getShortUUID(), HEX);
				//Serial.print("service start handle : ");
				//Serial.println(DiscoveredInfo.service[index].service.getStartHandle(), HEX);
				//Serial.print("service end handle : ");
				//Serial.println(DiscoveredInfo.service[index].service.getEndHandle(), HEX);
			//}
			/* To the end, start discovery chars of a service */
			DiscoveredInfo.serviceIndex=0;
			nextServiceIncludeDiscovery();
		}
		else {
			if(sd_ble_gattc_primary_services_discover(connHandle, endHandle, NULL) != NRF_SUCCESS) {
				if(terminateDiscoveryCallback)
					terminateDiscoveryCallback(0xA0);
			}
		}
	}
	else if(state == SERVICE_DISCOVERY_ACTIVE && DiscoveredInfo.currNumServices !=0x00 )
	{	/* If no enough space, stop discovery service, start discovery chars */
		DiscoveredInfo.serviceIndex=0;
		nextServiceIncludeDiscovery();
	}
}

void
nRF51ServiceDiscovery::nextServiceIncludeDiscovery(void)
{	
	/*Terminate service discovery, if exist services, start discovery characteristics.*/
	if(DiscoveredInfo.currNumServices && DiscoveredInfo.serviceIndex < DiscoveredInfo.currNumServices) {
		//Serial.println("start characteristics discovery ");
		characteristicsDiscoveryStart();
		
		startDiscoveryCharacteristic(connHandle,
									 DiscoveredInfo.service[DiscoveredInfo.serviceIndex].service.getStartHandle(),
									 DiscoveredInfo.service[DiscoveredInfo.serviceIndex].service.getEndHandle());	
	}
	else {
		/* All services is discovered */
		//Serial.println("All services is discovered ");
		//Serial.println("start discovery descriptor ");
		DiscoveredInfo.serviceIndex = 0;
		
		startCharsDescriptorDiscovery();
	}
}

void
nRF51ServiceDiscovery::setupDiscoveredCharacteristics(const ble_gattc_evt_char_disc_rsp_t *response)
{
	uint8_t index=0;
	uint8_t responsIndex=0;
	//Serial.println("setupDiscoveredCharacteristics ");
	DiscoveredInfo.service[DiscoveredInfo.serviceIndex].preNumChars = DiscoveredInfo.service[DiscoveredInfo.serviceIndex].currNumChars;
	DiscoveredInfo.service[DiscoveredInfo.serviceIndex].currNumChars += response->count;;
	
	if(DiscoveredInfo.service[DiscoveredInfo.serviceIndex].currNumChars > DEVICE_CHARS_OF_SERVICE_MAX_NUM)
		DiscoveredInfo.service[DiscoveredInfo.serviceIndex].currNumChars = DEVICE_CHARS_OF_SERVICE_MAX_NUM;
	
	charUUIDDiscoveryQueue.reset();
	for(index=DiscoveredInfo.service[DiscoveredInfo.serviceIndex].preNumChars; index<DiscoveredInfo.service[DiscoveredInfo.serviceIndex].currNumChars; index++,responsIndex++) {
		DiscoveredCharacteristic::Properties_t properties;
		properties._broadcast 		= response->chars[responsIndex].char_props.broadcast;
		properties._read 			= response->chars[responsIndex].char_props.read;
		properties._writeWoResp	 	= response->chars[responsIndex].char_props.write_wo_resp;
		properties._write 			= response->chars[responsIndex].char_props.write;
		properties._notify 			= response->chars[responsIndex].char_props.notify;
		properties._indicate 		= response->chars[responsIndex].char_props.indicate;
		properties._authSignedWrite = response->chars[responsIndex].char_props.auth_signed_wr;

		if(response->chars[responsIndex].uuid.type == BLE_UUID_TYPE_UNKNOWN) {	
			//Serial.println("BLE_UUID_TYPE_UNKNOWN ");
			charUUIDDiscoveryQueue.enqueue(index);
			DiscoveredInfo.service[DiscoveredInfo.serviceIndex].chars[index].setup(connHandle,
																				   properties,	
																				   response->chars[responsIndex].handle_decl,
																				   response->chars[responsIndex].handle_value);
		}
		else {	
			//Serial.println("BLE_UUID_TYPE_KNOWN ");
			DiscoveredInfo.service[DiscoveredInfo.serviceIndex].chars[index].setup(connHandle,
																				   response->chars[responsIndex].uuid.uuid,
																				   properties,	
																				   response->chars[responsIndex].handle_decl,
																				   response->chars[responsIndex].handle_value);			
		}
	}
	/* Trigger discovery of char UUID if necessary. */
    if (charUUIDDiscoveryQueue.getCount()) {
        charUUIDDiscoveryQueue.triggerFirst();
    }
}

void
nRF51ServiceDiscovery::progressCharacteristicDiscovery(void)
{
	if( (state == CHARACTERISTIC_DISCOVERY_ACTIVE) && 
		(DiscoveredInfo.service[DiscoveredInfo.serviceIndex].preNumChars < DiscoveredInfo.service[DiscoveredInfo.serviceIndex].currNumChars) ) {
		//Serial.println("progressCharacteristicDiscovery ");
		DiscoveredInfo.service[DiscoveredInfo.serviceIndex].preNumChars = DiscoveredInfo.service[DiscoveredInfo.serviceIndex].currNumChars;
		/* Determine the ending handle of the last cached characteristic. */
		Gap::Handle_t startHandle = DiscoveredInfo.service[DiscoveredInfo.serviceIndex].chars[DiscoveredInfo.service[DiscoveredInfo.serviceIndex].currNumChars-1].getValueHandle() + 1;
		Gap::Handle_t endHandle = DiscoveredInfo.service[DiscoveredInfo.serviceIndex].service.getEndHandle();
		if(startHandle < endHandle) {
			
			//Serial.print("service start handle : ");
			//Serial.println(startHandle, HEX);
			//Serial.print("service end handle : ");
			//Serial.println(endHandle, HEX);
			/* Restart discovery characteristics of current service. */
			startDiscoveryCharacteristic(connHandle, startHandle, endHandle);
		}
		else {
			//Serial.println("start next characteristic discovery or discovery descriptor ");
			DiscoveredInfo.serviceIndex++;
			nextServiceIncludeDiscovery();
		}
	}
	else if(state == CHARACTERISTIC_DISCOVERY_ACTIVE)
	{
		DiscoveredInfo.serviceIndex++;
		nextServiceIncludeDiscovery();
	}
}


void
nRF51ServiceDiscovery::startCharsDescriptorDiscovery(void)
{	
	uint8_t flag1=0, flag2=0;
	
	ble_gattc_handle_range_t handleRange = { };
	
	while(1)
	{	
		flag1 = 0;
		flag2 = 0;
		if(DiscoveredInfo.service[DiscoveredInfo.serviceIndex].currNumChars) {	
			if(DiscoveredInfo.service[DiscoveredInfo.serviceIndex].charsIndex < DiscoveredInfo.service[DiscoveredInfo.serviceIndex].currNumChars-1) {	/* not the last one */
				flag1 = 1;
				handleRange.start_handle = DiscoveredInfo.service[DiscoveredInfo.serviceIndex].chars[DiscoveredInfo.service[DiscoveredInfo.serviceIndex].charsIndex].getValueHandle()+1;
				handleRange.end_handle   = DiscoveredInfo.service[DiscoveredInfo.serviceIndex].chars[DiscoveredInfo.service[DiscoveredInfo.serviceIndex].charsIndex+1].getDeclHandle();			
			}
			else {	
				/* last one chars of this service */
				flag1 = 2;
				handleRange.start_handle = DiscoveredInfo.service[DiscoveredInfo.serviceIndex].chars[DiscoveredInfo.service[DiscoveredInfo.serviceIndex].charsIndex].getValueHandle();
				handleRange.end_handle   = DiscoveredInfo.service[DiscoveredInfo.serviceIndex].service.getEndHandle();
			}
			//Serial.print("descriptor start handle : ");
			//Serial.println(handleRange.start_handle, HEX);
			//Serial.print("descriptor end handle : ");
			//Serial.println(handleRange.end_handle, HEX);
			if(handleRange.start_handle >= handleRange.end_handle) {
				/* this chars no descriptor */
				if(flag1 == 1) {	
					/* look at next chars */
					//Serial.println("this chars no descriptor ");
					DiscoveredInfo.service[DiscoveredInfo.serviceIndex].charsIndex++;
				}
				else if(flag1 == 2) {
					/* look at next service */
					//Serial.println("look at next service ");
					DiscoveredInfo.serviceIndex++;
				}
			}
			else { 	
				/* exist descriptor */
				flag2 = 1;
				break;
			}
		}
		else {	
			/* no chars of this service, look at next */
			/* can't go here */
			//Serial.println(" look at next service ");
			DiscoveredInfo.serviceIndex++;
		}
		/* check if to the tail */
		if(DiscoveredInfo.serviceIndex >= DiscoveredInfo.currNumServices) {
			flag2 = 2;
			break;
		}
	}
	if(flag2 == 1) {	
		//Serial.println(" exist descriptor ");
		descriptorDiscoveryStart();
		startDiscoveryDescriptor(connHandle,handleRange.start_handle,handleRange.end_handle);
	}
	else if(flag2 == 2) {
		//Serial.println("all descriptor is discovered, end discovery");
		discoveryCallback(&DiscoveredInfo);
	}
}


void 
nRF51ServiceDiscovery::setupDiscoveredDescriptor(const ble_gattc_evt_desc_disc_rsp_t *respone)
{
	if(state == DESCRIPTOR_DISCOVERY_ACTIVE) {
		//Serial.println("DESCRIPTOR_DISCOVERY_ACTIVE ");
		descriptorState = 1;
		for(uint8_t index=0; index<respone->count; index++){	
			if(respone->descs[index].uuid.uuid ==  BLE_UUID_DESCRIPTOR_CHAR_USER_DESC) {
				//Serial.println("descriptor ");				
				DiscoveredInfo.service[DiscoveredInfo.serviceIndex].chars[DiscoveredInfo.service[DiscoveredInfo.serviceIndex].charsIndex].setDescHandle(respone->descs[index].handle);
			}
			
			if(respone->descs[index].uuid.uuid ==  BLE_UUID_DESCRIPTOR_CLIENT_CHAR_CONFIG){
				//Serial.println("cccd ");				
				DiscoveredInfo.service[DiscoveredInfo.serviceIndex].chars[DiscoveredInfo.service[DiscoveredInfo.serviceIndex].charsIndex].setCCCDHandle(respone->descs[index].handle);
			}			
		}
		endHandleDescriptor = respone->descs[respone->count-1].handle;
	}
}

void 
nRF51ServiceDiscovery::progressDescriptorDiscovery(void)
{
	uint8_t flag=0;
	
	if( (state == DESCRIPTOR_DISCOVERY_ACTIVE) && (descriptorState>0) ) {
		//Serial.println("progressDescriptorDiscovery ");					
		Gap::Handle_t startHandle;
		Gap::Handle_t endHandle;
		if(descriptorState == 1) {	
			/* start descriptor */
			startHandle = endHandleDescriptor + 1;
			if(DiscoveredInfo.service[DiscoveredInfo.serviceIndex].charsIndex < DiscoveredInfo.service[DiscoveredInfo.serviceIndex].currNumChars-1) {	/* not the last one */
				flag=1;
				endHandle   = DiscoveredInfo.service[DiscoveredInfo.serviceIndex].chars[DiscoveredInfo.service[DiscoveredInfo.serviceIndex].charsIndex+1].getDeclHandle();			
			}
			else  {	
				/* last one chars of this service */
				flag=2;
				endHandle   = DiscoveredInfo.service[DiscoveredInfo.serviceIndex].service.getEndHandle();
			}
			if(startHandle < endHandle) {	
				/* restart discovery descriptor */
				//Serial.println("restart discovery descriptor");		
				startDiscoveryDescriptor(connHandle,startHandle,endHandle);
			}
			else {
				//Serial.println("this chars finished, next chars ");	
				if(flag == 1)
					DiscoveredInfo.service[DiscoveredInfo.serviceIndex].charsIndex++;
				else if(flag == 2)
					DiscoveredInfo.serviceIndex++;
				startCharsDescriptorDiscovery();
			}
		}
		else if(descriptorState == 2) {
			if(DiscoveredInfo.service[DiscoveredInfo.serviceIndex].charsIndex < DiscoveredInfo.service[DiscoveredInfo.serviceIndex].currNumChars-1)
				DiscoveredInfo.service[DiscoveredInfo.serviceIndex].charsIndex++;
			else 
				DiscoveredInfo.serviceIndex++;
			//Serial.println("not find, next char ");	
			startCharsDescriptorDiscovery();
		}
		descriptorState = 0;
	}
}


void
nRF51ServiceDiscovery::ServiceUUIDDiscoveryQueue::triggerFirst(void)
{	
	/* loop until a call to char_value_by_uuid_read() succeeds or we run out of pending indices. */
	while (numIndices) { 
        parentDiscoveryObject->state = DISCOVER_SERVICE_UUIDS;

        unsigned serviceIndex = getFirst();
        ble_uuid_t uuid = {
            .uuid = BLE_UUID_SERVICE_PRIMARY,
            .type = BLE_UUID_TYPE_BLE,
        };
        ble_gattc_handle_range_t handleRange = {
            .start_handle = parentDiscoveryObject->DiscoveredInfo.service[serviceIndex].service.getStartHandle(),
            .end_handle   = parentDiscoveryObject->DiscoveredInfo.service[serviceIndex].service.getEndHandle(),
        };
        if (sd_ble_gattc_char_value_by_uuid_read(parentDiscoveryObject->connHandle, &uuid, &handleRange) == NRF_SUCCESS) {
            return;
        }

        /* Skip this service if we fail to launch a read for its service-declaration
         * attribute. Its UUID will remain INVALID, and it may not match any filters. */
        dequeue();
    }

    /* Switch back to service discovery upon exhausting the service-indices pending UUID discovery. */
    if (parentDiscoveryObject->state == DISCOVER_SERVICE_UUIDS) {
        parentDiscoveryObject->state = SERVICE_DISCOVERY_ACTIVE;
    }
}

void
nRF51ServiceDiscovery::CharUUIDDiscoveryQueue::triggerFirst(void)
{
    while (numIndices) { /* loop until a call to char_value_by_uuid_read() succeeds or we run out of pending indices. */
        parentDiscoveryObject->state = DISCOVER_CHARACTERISTIC_UUIDS;

        unsigned charIndex = getFirst();
        ble_uuid_t uuid = {
            .uuid = BLE_UUID_CHARACTERISTIC,
            .type = BLE_UUID_TYPE_BLE,
        };
        ble_gattc_handle_range_t handleRange = { 
		    .start_handle = parentDiscoveryObject->DiscoveredInfo.service[parentDiscoveryObject->DiscoveredInfo.serviceIndex].chars[charIndex].getDeclHandle(),
			.end_handle   = parentDiscoveryObject->DiscoveredInfo.service[parentDiscoveryObject->DiscoveredInfo.serviceIndex].chars[charIndex].getDeclHandle() + 1,
		};

        if (sd_ble_gattc_char_value_by_uuid_read(parentDiscoveryObject->connHandle, &uuid, &handleRange) == NRF_SUCCESS) {
            return;
        }

        /* Skip this service if we fail to launch a read for its service-declaration
         * attribute. Its UUID will remain INVALID, and it may not match any filters. */
        dequeue();
    }

    /* Switch back to service discovery upon exhausting the service-indices pending UUID discovery. */
    if (parentDiscoveryObject->state == DISCOVER_CHARACTERISTIC_UUIDS) {
        parentDiscoveryObject->state = CHARACTERISTIC_DISCOVERY_ACTIVE;
    }
}

void
nRF51ServiceDiscovery::progressDiscoverUUIDResponse(const ble_gattc_evt_char_val_by_uuid_read_rsp_t *response)
{
	if(state == DISCOVER_SERVICE_UUIDS) {
		//Serial.println("DISCOVER_SERVICE_UUIDS");  
		if((response->count == 1) && (response->value_len == UUID::LENGTH_OF_LONG_UUID)) {
			UUID::LongUUIDBytes_t uuid;
			/* Switch longUUID bytes to MSB */
			//Serial.print("DISCOVER_SERVICE_UUIDS is : "); 
            for (unsigned i = 0; i < UUID::LENGTH_OF_LONG_UUID; i++) {
                uuid[i] = response->handle_value[0].p_value[UUID::LENGTH_OF_LONG_UUID - 1 - i];
				//Serial.print(uuid[i], HEX); 
            }
			//Serial.println("");
			
			unsigned serviceIndex = serviceUUIDDiscoveryQueue.dequeue();
			DiscoveredInfo.service[serviceIndex].service.setupLongUUID(uuid);
			
			serviceUUIDDiscoveryQueue.triggerFirst();
		}
		else {
			serviceUUIDDiscoveryQueue.dequeue();
		}
	}
	else if(state == DISCOVER_CHARACTERISTIC_UUIDS) {
		//Serial.println("DISCOVER_CHARACTERISTIC_UUIDS");  
		if ((response->count == 1) && (response->value_len == UUID::LENGTH_OF_LONG_UUID + 1 /* props */ + 2 /* value handle */)) {
			UUID::LongUUIDBytes_t uuid;
			/* Switch longUUID bytes to MSB */
			//Serial.print("DISCOVER_CHARACTERISTIC_UUIDS is : "); 
            for (unsigned i = 0; i < UUID::LENGTH_OF_LONG_UUID; i++) {
                uuid[i] = response->handle_value[0].p_value[3 + UUID::LENGTH_OF_LONG_UUID - 1 - i];
				//Serial.print(uuid[i], HEX); 
            }
			//Serial.println("");
			
		    unsigned charIndex = charUUIDDiscoveryQueue.dequeue();
			DiscoveredInfo.service[DiscoveredInfo.serviceIndex].chars[charIndex].setupLongUUID(uuid);

            charUUIDDiscoveryQueue.triggerFirst();	
		}
		else {
			charUUIDDiscoveryQueue.dequeue();
		}
	}
}

