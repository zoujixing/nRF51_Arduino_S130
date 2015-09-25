
#ifndef BLE_API_H_
#define BLE_API_H_


#include "ble_api/blecommon.h"
#include "ble_api/BLEDeviceInstanceBase.h"
#include "ble_api/BLEDevice.h"
#include "ble_api/CallChainOfFunctionPointersWithContext.h"
#include "ble_api/FunctionPointerWithContext.h"
#include "ble_api/Gap.h"
#include "ble_api/GapAdvertisingData.h"
#include "ble_api/GapAdvertisingParams.h"
#include "ble_api/GapEvents.h"
#include "ble_api/GapScanningParams.h"
#include "ble_api/GattAttribute.h"
#include "ble_api/GattCharacteristic.h"
#include "ble_api/GattCharacteristicCallbackParams.h"
#include "ble_api/GattServer.h"
#include "ble_api/GattServerEvents.h"
#include "ble_api/GattService.h"
#include "ble_api/UUID.h"

#include "ble_api/GattClient.h"
//#include "ble_api/GattClientEvents.h"
#include "ble_api/DiscoveredCharacteristic.h"
#include "ble_api/DiscoveredService.h"
#include "ble_api/DiscoveredDevice.h"


#include "ble_service/Service_Battery.h"
#include "ble_service/Service_DeviceInformation.h"
#include "ble_service/Service_HealthThermometer.h"
#include "ble_service/Service_HeartRate.h"
#include "ble_service/Service_iBeacon.h"
#include "ble_service/Service_LinkLoss.h"

#endif

