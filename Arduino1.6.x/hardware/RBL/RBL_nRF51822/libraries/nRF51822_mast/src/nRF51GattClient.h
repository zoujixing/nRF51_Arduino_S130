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
 
#ifndef NRF51_GATT_CLIENT_H__
#define NRF51_GATT_CLIENT_H__
 
#include "ble_api/GattClient.h"
#include "ble_api/GattCharacteristicCallbackParams.h"

#include "nRF51ServiceDiscovery.h"

#include "ble.h"
#include "ble_gattc.h"

class nRF51GattClient : public GattClient
{
public:
    static nRF51GattClient &getInstance() {
        static nRF51GattClient m_instance;
        return m_instance;
    }
	 
    static const uint16_t SRV_DISC_START_HANDLE             = 0x0001; /**< The start handle value used during service discovery. */
    static const uint16_t SRV_DISC_END_HANDLE               = 0xFFFF; /**< The end handle value used during service discovery. */	 
	 
    virtual ble_error_t launchServiceDiscovery(Gap::Handle_t connectionHandle, DiscoveredCallback_t callback);

	virtual void terminateServiceDiscovery(TerminationCallback_t callback);
	
    virtual ble_error_t read(Gap::Handle_t connHandle, GattAttribute::Handle_t attributeHandle, uint16_t offset) const {
        uint32_t rc = sd_ble_gattc_read(connHandle, attributeHandle, offset);
        if (rc == NRF_SUCCESS) {
            return BLE_ERROR_NONE;
        }
        switch (rc) {
            case NRF_ERROR_BUSY:
                return BLE_STACK_BUSY;
            case BLE_ERROR_INVALID_CONN_HANDLE:
            case NRF_ERROR_INVALID_STATE:
            case NRF_ERROR_INVALID_ADDR:
            default:
                return BLE_ERROR_INVALID_STATE;
        }
    }

    virtual ble_error_t write(GattClient::WriteOp_t cmd, Gap::Handle_t connHandle, GattAttribute::Handle_t attributeHandle, size_t length, const uint8_t *value) const {
        ble_gattc_write_params_t writeParams = { };
        writeParams.write_op = cmd;
        writeParams.handle   = attributeHandle;
        writeParams.len      = length;
        writeParams.p_value  = const_cast<uint8_t *>(value);

        uint32_t rc = sd_ble_gattc_write(connHandle, &writeParams);
        if (rc == NRF_SUCCESS) {
            return BLE_ERROR_NONE;
        }
        switch (rc) {
            case NRF_ERROR_BUSY:
                return BLE_STACK_BUSY;
            case BLE_ERROR_NO_TX_BUFFERS:
                return BLE_ERROR_NO_MEM;
            case BLE_ERROR_INVALID_CONN_HANDLE:
            case NRF_ERROR_INVALID_STATE:
            case NRF_ERROR_INVALID_ADDR:
            default:
                return BLE_ERROR_INVALID_STATE;
        }
    }
	 
	 
public:
    nRF51GattClient() {
        /* empty */
    }

	ble_error_t hwCallback(ble_evt_t *p_ble_evt);

private:
    nRF51GattClient(const nRF51GattClient &);
    const nRF51GattClient& operator=(const nRF51GattClient &);	 

private:
    nRF51ServiceDiscovery 	discovery;
};
 
 #endif
 
 