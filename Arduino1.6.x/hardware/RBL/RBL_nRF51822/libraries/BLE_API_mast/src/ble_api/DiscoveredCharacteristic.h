/*
 *   Copyright (c) 2014 RedBearLab, All rights reserved.
 *
 *   This library is free software; you can redistribute it and/or
 *   modify it under the terms of the GNU Lesser General Public
 *   License as published by the Free Software Foundation; either
 *   version 2.1 of the License, or (at your option) any later version.
 *
 *   This library is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *   See the GNU Lesser General Public License for more details.
 *
 *   You should have received a copy of the GNU Lesser General Public
 *   License along with this library; if not, write to the Free Software
 *   Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 *   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 *   EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 *   MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 *   IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 *   CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 *   TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 *   SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */
 
#ifndef __DISCOVERED_CHARACTERISTIC_H__
#define __DISCOVERED_CHARACTERISTIC_H__

#include "UUID.h"
#include "Gap.h"
#include "GattAttribute.h"
//#include "GattClient.h"

/**
 * Structure for holding information about the service and the characteristics
 * found during the discovery process.
 */
class DiscoveredCharacteristic {
public:
    typedef struct {
        uint8_t _broadcast       :1; /**< Broadcasting of the value permitted. */
        uint8_t _read            :1; /**< Reading the value permitted. */
        uint8_t _writeWoResp     :1; /**< Writing the value with Write Command permitted. */
        uint8_t _write           :1; /**< Writing the value with Write Request permitted. */
        uint8_t _notify          :1; /**< Notications of the value permitted. */
        uint8_t _indicate        :1; /**< Indications of the value permitted. */
        uint8_t _authSignedWrite :1; /**< Writing the value with Signed Write Command permitted. */
	}Properties_t;

	void setup(Gap::Handle_t            connectionHandleIn,
               Properties_t    			propsIn,
               GattAttribute::Handle_t  declHandleIn,
               GattAttribute::Handle_t  valueHandleIn){
				   
		connHandle  = connectionHandleIn;
		declHandle  = declHandleIn;
		valueHandle = valueHandleIn;

		props._broadcast       = propsIn._broadcast;
		props._read            = propsIn._read;
		props._writeWoResp     = propsIn._writeWoResp;
		props._write           = propsIn._write;
		props._notify          = propsIn._notify;
		props._indicate        = propsIn._indicate;
		props._authSignedWrite = propsIn._authSignedWrite;
	}
	
	void setup(Gap::Handle_t            connectionHandleIn,
               UUID::ShortUUIDBytes_t   uuidIn,
               Properties_t    			propsIn,
               GattAttribute::Handle_t  declHandleIn,
               GattAttribute::Handle_t  valueHandleIn){
				   
		connHandle  = connectionHandleIn;
		uuid        = uuidIn;
		declHandle  = declHandleIn;
		valueHandle = valueHandleIn;

		props._broadcast       = propsIn._broadcast;
		props._read            = propsIn._read;
		props._writeWoResp     = propsIn._writeWoResp;
		props._write           = propsIn._write;
		props._notify          = propsIn._notify;
		props._indicate        = propsIn._indicate;
		props._authSignedWrite = propsIn._authSignedWrite;
	}
	
	void setDescHandle(GattAttribute::Handle_t descHandleIn){
		descHandle=descHandleIn;
	}
	
	void setCCCDHandle(GattAttribute::Handle_t CCCDHandleIn) {
		cccdHandle=CCCDHandleIn;
	}
	
    /**
     * Structure for holding information about the service and the characteristics
     * found during the discovery process.
     */
    struct DiscoveredDescriptor {
        GattAttribute::Handle_t handle; /**< Descriptor Handle. */
        UUID                    uuid;   /**< Descriptor UUID. */
    };

    /**
     * Callback type for when a characteristic descriptor is found during descriptor-
     * discovery. The receiving function is passed in a pointer to a
     * DiscoveredDescriptor object which will remain valid for the lifetime
     * of the callback. Memory for this object is owned by the BLE_API eventing
     * framework. The application can safely make a persistent shallow-copy of
     * this object in order to work with the characteristic beyond the callback.
     */
    typedef void (*DescriptorCallback_t)(const DiscoveredDescriptor *);

    /**
     * Initiate (or continue) a read for the value attribute, optionally at a
     * given offset. If the Characteristic or Descriptor to be read is longer
     * than ATT_MTU - 1, this function must be called multiple times with
     * appropriate offset to read the complete value.
     *
     * @return BLE_ERROR_NONE if a read has been initiated, else
     *         BLE_ERROR_INVALID_STATE if some internal state about the connection is invalid, or
     *         BLE_STACK_BUSY if some client procedure already in progress, or
     *         BLE_ERROR_OPERATION_NOT_PERMITTED due to the characteristic's properties.
     */
    ble_error_t read(uint16_t offset = 0) const;

    /**
     * Perform a write without response procedure.
     *
     * @param  length
     *           The amount of data being written.
     * @param  value
     *           The bytes being written.
     *
     * @note   It is important to note that a write without response will generate
     *         an onDataSent() callback when the packet has been transmitted. There
     *         will be a BLE-stack specific limit to the number of pending
     *         writeWoResponse operations; the user may want to use the onDataSent()
     *         callback for flow-control.
     *
     * @retval BLE_ERROR_NONE Successfully started the Write procedure, else
     *         BLE_ERROR_INVALID_STATE if some internal state about the connection is invalid, or
     *         BLE_STACK_BUSY if some client procedure already in progress, or
     *         BLE_ERROR_NO_MEM if there are no available buffers left to process the request, or
     *         BLE_ERROR_OPERATION_NOT_PERMITTED due to the characteristic's properties.
     */
    ble_error_t writeWoResponse(uint16_t length, const uint8_t *value) const;

    /**
     * Initiate a GATT Characteristic Descriptor Discovery procedure for descriptors within this characteristic.
     *
     * @param  callback
     * @param  matchingUUID
     *           filter for descriptors. Defaults to wildcard which will discover all descriptors.
     *
     * @return  BLE_ERROR_NONE if descriptor discovery is launched successfully; else an appropriate error.
     */
    ble_error_t discoverDescriptors(DescriptorCallback_t callback, const UUID &matchingUUID = UUID::ShortUUIDBytes_t(BLE_UUID_UNKNOWN)) const;

    /**
     * Perform a write procedure.
     *
     * @param  length
     *           The amount of data being written.
     * @param  value
     *           The bytes being written.
     *
     * @note   It is important to note that a write will generate
     *         an onDataWritten() callback when the peer acknowledges the request.
     *
     * @retval BLE_ERROR_NONE Successfully started the Write procedure, else
     *         BLE_ERROR_INVALID_STATE if some internal state about the connection is invalid, or
     *         BLE_STACK_BUSY if some client procedure already in progress, or
     *         BLE_ERROR_NO_MEM if there are no available buffers left to process the request, or
     *         BLE_ERROR_OPERATION_NOT_PERMITTED due to the characteristic's properties.
     */
    ble_error_t write(uint16_t length, const uint8_t *value) const;
	
    void setupLongUUID(UUID::LongUUIDBytes_t longUUID) {
        uuid.setupLong(longUUID);
    }

public:
    const UUID &getUUID(void) const {
        return uuid;
    }
	
    UUID::ShortUUIDBytes_t getShortUUID(void) const {
        return uuid.getShortUUID();
    }

    const Properties_t& getProperties(void) const {
        return props;
    }

	const GattAttribute::Handle_t& getConnHandle(void) const {
        return connHandle;
    }
	
    const GattAttribute::Handle_t& getDeclHandle(void) const {
        return declHandle;
    }
	
    const GattAttribute::Handle_t& getValueHandle(void) const {
        return valueHandle;
    }
	
	const GattAttribute::Handle_t& getDescHandle(void) const {
        return descHandle;
    }
    const GattAttribute::Handle_t& getCCCDHndle(void) const {
        return cccdHandle;
    }

public:
    DiscoveredCharacteristic() : uuid(UUID::ShortUUIDBytes_t(0)),
                                 props(),
                                 declHandle(GattAttribute::INVALID_HANDLE),
                                 valueHandle(GattAttribute::INVALID_HANDLE),
                                 descHandle(GattAttribute::INVALID_HANDLE),
                                 cccdHandle(GattAttribute::INVALID_HANDLE)
	{
        /* empty */
    }

protected:
    UUID                     uuid;
    Properties_t             props;
    GattAttribute::Handle_t  declHandle;
    GattAttribute::Handle_t  valueHandle;
	GattAttribute::Handle_t  descHandle;
	GattAttribute::Handle_t  cccdHandle;

    Gap::Handle_t            connHandle;
};


#endif

