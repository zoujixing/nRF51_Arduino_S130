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
 
 
#include "nRF51GattClient.h"

#include "Arduino.h"
 
extern UARTClass Serial;

ble_error_t 
nRF51GattClient::hwCallback(ble_evt_t *p_ble_evt)
{
	switch (p_ble_evt->header.evt_id) {
        case BLE_GATTC_EVT_PRIM_SRVC_DISC_RSP:
		{
			//Serial.println("BLE_GATTC_EVT_PRIM_SRVC_DISC_RSP ");
			switch(p_ble_evt->evt.gattc_evt.gatt_status)
			{
                case BLE_GATT_STATUS_SUCCESS:
                    discovery.setupDiscoveredServices(&p_ble_evt->evt.gattc_evt.params.prim_srvc_disc_rsp);
                    break;

                case BLE_GATT_STATUS_ATTERR_ATTRIBUTE_NOT_FOUND:
                default:
					//Serial.println("BLE_GATT_STATUS_ATTERR_ATTRIBUTE_NOT_FOUND");
					discovery.nextServiceIncludeDiscovery();
                    break;
			}
		}
        break;

        case BLE_GATTC_EVT_CHAR_DISC_RSP:
		{	
			//Serial.println("BLE_GATTC_EVT_CHAR_DISC_RSP ");
			switch(p_ble_evt->evt.gattc_evt.gatt_status)
			{
				case BLE_GATT_STATUS_SUCCESS:
				discovery.setupDiscoveredCharacteristics(&p_ble_evt->evt.gattc_evt.params.char_disc_rsp);
				break;
				
				case BLE_GATT_STATUS_ATTERR_ATTRIBUTE_NOT_FOUND:
				default : 
					//Serial.println("BLE_GATT_STATUS_ATTERR_ATTRIBUTE_NOT_FOUND ");
					discovery.nextServiceIndex();
					discovery.nextServiceIncludeDiscovery();
				break;
				
			}
		}
        break;

        case BLE_GATTC_EVT_CHAR_VAL_BY_UUID_READ_RSP:
		{
			//Serial.println("BLE_GATTC_EVT_CHAR_VAL_BY_UUID_READ_RSP ");
			if(discovery.isActive()) {
				discovery.progressDiscoverUUIDResponse(&p_ble_evt->evt.gattc_evt.params.char_val_by_uuid_read_rsp);
			}
		}
        break;
		
		case BLE_GATTC_EVT_DESC_DISC_RSP:
		{
			//Serial.println("BLE_GATTC_EVT_DESC_DISC_RSP ");
			switch(p_ble_evt->evt.gattc_evt.gatt_status){
				case BLE_GATT_STATUS_SUCCESS : 
					discovery.setupDiscoveredDescriptor(&p_ble_evt->evt.gattc_evt.params.desc_disc_rsp);
					break;
				case BLE_GATT_STATUS_ATTERR_ATTRIBUTE_NOT_FOUND:
                default:
					//Serial.println("BLE_GATT_STATUS_ATTERR_ATTRIBUTE_NOT_FOUND ");
					discovery.setDescriptorState(2);
                    break;					
			}
		}
		break;

        case BLE_GATTC_EVT_READ_RSP: 
		{
			GattCharacteristicReadCBParams response = {
				.connHandle = p_ble_evt->evt.gattc_evt.conn_handle,
				.handle     = p_ble_evt->evt.gattc_evt.params.read_rsp.handle,
				.offset     = p_ble_evt->evt.gattc_evt.params.read_rsp.offset,
				.len        = p_ble_evt->evt.gattc_evt.params.read_rsp.len,
				.data       = p_ble_evt->evt.gattc_evt.params.read_rsp.data,
			};
			processReadResponse(&response);
        }
        break;

        case BLE_GATTC_EVT_WRITE_RSP: 
		{
			GattCharacteristicWriteCBParams response = {
				.connHandle = p_ble_evt->evt.gattc_evt.conn_handle,
				.handle     = p_ble_evt->evt.gattc_evt.params.write_rsp.handle,
				.writeOp    = (GattCharacteristicWriteCBParams::WriteOp_t)(p_ble_evt->evt.gattc_evt.params.write_rsp.write_op),
				.offset     = p_ble_evt->evt.gattc_evt.params.write_rsp.offset,
				.len        = p_ble_evt->evt.gattc_evt.params.write_rsp.len,
				.data       = p_ble_evt->evt.gattc_evt.params.write_rsp.data,
			};
			
			processWriteResponse(&response);
        }
        break;

        case BLE_GATTC_EVT_HVX: 
		{
			GattCharacteristicHVXCallbackParams params;
			params.connHandle = p_ble_evt->evt.gattc_evt.conn_handle;
			params.handle     = p_ble_evt->evt.gattc_evt.params.hvx.handle;
			params.type       = static_cast<HVXType_t>(p_ble_evt->evt.gattc_evt.params.hvx.type);
			params.len        = p_ble_evt->evt.gattc_evt.params.hvx.len;
			params.data       = p_ble_evt->evt.gattc_evt.params.hvx.data;
			
			processHVXEvent(&params);
        }
        break;
    }
	
	discovery.progressDescriptorDiscovery();
	discovery.progressCharacteristicDiscovery();
	discovery.progressServiceDiscovery();
}
 
ble_error_t 
nRF51GattClient::launchServiceDiscovery(Gap::Handle_t connectionHandle, DiscoveredCallback_t callback)
{
    uint32_t rc;
	
	if (discovery.isActive()) {
            return BLE_ERROR_INVALID_STATE;
    }
	
	discovery.setDeviceCallback(callback);
	discovery.discoveryStarted(connectionHandle);
	discovery.serviceDiscoveryStart();
	
    if ((rc = sd_ble_gattc_primary_services_discover(connectionHandle, SRV_DISC_START_HANDLE, NULL)) != NRF_SUCCESS) {
        Serial.println("sd_ble_gattc_primary_services_discover fail ");
		switch (rc) {
            case BLE_ERROR_INVALID_CONN_HANDLE:
            case NRF_ERROR_INVALID_ADDR:
                return BLE_ERROR_INVALID_PARAM;
            case NRF_ERROR_BUSY:
                return BLE_STACK_BUSY;
            default:
            case NRF_ERROR_INVALID_STATE:
                return BLE_ERROR_INVALID_STATE;
        }
    }
    return BLE_ERROR_NONE;
}

void 
nRF51GattClient::terminateServiceDiscovery(TerminationCallback_t callback)
{
	discovery.setTerminateDiscoveryCallback(callback);
}

