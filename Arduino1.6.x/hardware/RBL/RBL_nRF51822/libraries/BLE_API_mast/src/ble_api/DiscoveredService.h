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
 
#ifndef __DISCOVERED_SERVICE_H__
#define __DISCOVERED_SERVICE_H__

#include <string.h>

#include "UUID.h"
#include "GattAttribute.h"


/**@brief Type for holding information about the service and the characteristics found during
 *        the discovery process.
 */
class DiscoveredService {
public:
     void setup(UUID uuidIn, GattAttribute::Handle_t startHandleIn, GattAttribute::Handle_t endHandleIn) {
         uuid        = uuidIn;
         startHandle = startHandleIn;
         endHandle   = endHandleIn;
     }

     void setup(GattAttribute::Handle_t startHandleIn, GattAttribute::Handle_t endHandleIn) {
         startHandle = startHandleIn;
         endHandle   = endHandleIn;
     }

    void setupLongUUID(UUID::LongUUIDBytes_t longUUID) {
         uuid.setupLong(longUUID);
    }

public:
    const UUID &getUUID(void) const {
        return uuid;
    }

    const GattAttribute::Handle_t& getStartHandle(void) const {
        return startHandle;
    }
    const GattAttribute::Handle_t& getEndHandle(void) const {
        return endHandle;
    }

public:
    DiscoveredService() : uuid(UUID::ShortUUIDBytes_t(0)),
                          startHandle(GattAttribute::INVALID_HANDLE),
                          endHandle(GattAttribute::INVALID_HANDLE) {
        /* empty */
    }

private:
    DiscoveredService(const DiscoveredService &);

private:
    UUID                    uuid;        /**< UUID of the service.  */
    GattAttribute::Handle_t startHandle; /**< Service Handle Range. */
    GattAttribute::Handle_t endHandle;   /**< Service Handle Range. */
};


#endif

