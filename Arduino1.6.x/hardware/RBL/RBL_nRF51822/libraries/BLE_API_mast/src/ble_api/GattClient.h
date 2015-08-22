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

#ifndef GATT_CLIENT_H__
#define GATT_CLIENT_H__

#include "Gap.h"
#include "UUID.h"
#include "GattAttribute.h"

#include "GattCharacteristicCallbackParams.h"

class GattClient {
	
public:
    
    enum WriteOp_t {
        GATT_OP_WRITE_REQ = 0x01,  /**< Write Request. */
        GATT_OP_WRITE_CMD = 0x02,  /**< Write Command. */
    };
	
	typedef void (*ReadCallback_t)(const GattCharacteristicReadAuthCBParams *params);
	
    typedef void (*WriteCallback_t)(const GattCharacteristicWriteAuthCBParams *params);

    typedef void (*HVXCallback_t)(const GattCharacteristicHVXCallbackParams *params);

    /*
     * The following functions are meant to be overridden in the platform-specific sub-class.
     */
public:

    virtual ble_error_t launchServiceDiscovery(Gap::Handle_t connectionHandle, 
											   Gap::Handle_t startHandle,
											   Gap::Handle_t endHandle) {
        /* avoid compiler warnings about unused variables */
        (void)connectionHandle;
		(void)startHandle;
		(void)endHandle;
		
        return BLE_ERROR_NOT_IMPLEMENTED; /* Requesting action from porter(s): override this API if this capability is supported. */
    }
	
	
    virtual ble_error_t launchServiceDiscovery(Gap::Handle_t connectionHandle ) {
        /* avoid compiler warnings about unused variables */
        (void)connectionHandle;
		
        return BLE_ERROR_NOT_IMPLEMENTED; /* Requesting action from porter(s): override this API if this capability is supported. */
    }
	
    /* Initiate a Gatt Client read procedure by attribute-handle. */
    virtual ble_error_t read(Gap::Handle_t connHandle, GattAttribute::Handle_t attributeHandle, uint16_t offset) const {
        /* avoid compiler warnings about unused variables */
        (void)connHandle;
        (void)attributeHandle;
        (void)offset;

        return BLE_ERROR_NOT_IMPLEMENTED; /* Requesting action from porter(s): override this API if this capability is supported. */
    }
	
	 /**
     * Initiate a GATT Client write procedure.
     *
     * @param[in] cmd
     *              Command can be either a write-request (which generates a
     *              matching response from the peripheral), or a write-command,
     *              which doesn't require the connected peer to respond.
     * @param[in] connHandle
     *              Connection handle.
     * @param[in] attributeHandle
     *              handle for the target attribtue on the remote GATT server.
     * @param[in] length
     *              length of the new value.
     * @param[in] value
     *              new value being written.
     */
    virtual ble_error_t write(GattClient::WriteOp_t    cmd,
                              Gap::Handle_t            connHandle,
                              GattAttribute::Handle_t  attributeHandle,
                              size_t                   length,
                              const uint8_t           *value) const {
        /* avoid compiler warnings about unused variables */
        (void)cmd;
        (void)connHandle;
        (void)attributeHandle;
        (void)length;
        (void)value;

        return BLE_ERROR_NOT_IMPLEMENTED; /* Requesting action from porter(s): override this API if this capability is supported. */
    }
	
/* Event callback handlers. */
public:
    /**
     * Setup a callback for read response events.
     */
    void onDataRead(ReadCallback_t callback) {
        onDataReadCallback = callback;
    }

    /**
     * Setup a callback for write response events.
     * @Note: write commands (issued using writeWoResponse) don't generate a response.
     */
    void onDataWritten(WriteCallback_t callback) {
        onDataWriteCallback = callback;
    }
 /**
     * Setup a callback for when GattClient receives an update event
     * corresponding to a change in value of a characteristic on the remote
     * GattServer.
     */
    void onHVX(HVXCallback_t callback) {
        onHVXCallback = callback;
    }

protected:
    GattClient() {
        /* empty */
    }

    /* Entry points for the underlying stack to report events back to the user. */
public:
    void processReadResponse(const GattCharacteristicReadAuthCBParams *params) {
        if (onDataReadCallback) {
            onDataReadCallback(params);
        }
    }

    void processWriteResponse(const GattCharacteristicWriteAuthCBParams *params) {
        if (onDataWriteCallback) {
            onDataWriteCallback(params);
        }
    }

    void processHVXEvent(const GattCharacteristicHVXCallbackParams *params) {
        if (onHVXCallback) {
            onHVXCallback(params);
        }
    }

protected:
    ReadCallback_t  onDataReadCallback;
    WriteCallback_t onDataWriteCallback;
    HVXCallback_t   onHVXCallback;

private:
    /* disallow copy and assignment */
    GattClient(const GattClient &);
    GattClient& operator=(const GattClient &);
};
	
#endif

