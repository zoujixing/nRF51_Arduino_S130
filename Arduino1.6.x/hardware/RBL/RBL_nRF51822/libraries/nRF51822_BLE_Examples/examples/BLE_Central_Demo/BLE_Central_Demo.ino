
#include <nRF51822_API.h>
#include <BLE_API.h>

BLEDevice ble;
//DiscoveredDevice HRM;
uint16_t conn_handle;

uint8_t  falg = 0;
uint8_t  state = 0;
uint32_t ble_advdata_decode(uint8_t type, uint8_t advdata_len, uint8_t *p_advdata, uint8_t *len, uint8_t *p_field_data)
{
    uint8_t index=0;
    uint8_t field_length, field_type;
    
    while(index<advdata_len)
    {
        field_length = p_advdata[index];
        field_type   = p_advdata[index+1];
        if(field_type == type)
        {
            memcpy(p_field_data, &p_advdata[index+2], (field_length-1));
            *len = field_length - 1;
            return NRF_SUCCESS;
        }
        index += field_length + 1;
    }
    return NRF_ERROR_NOT_FOUND;
}

void scanCallBack(const Gap::AdvertisementCallbackParams_t *params)
{ 
    Serial.println("Check");
    Serial.println("Scan Device CallBack Handle ");

    Serial.print("  The peerAddr : ");
    for(uint8_t index=0; index<6; index++)
    {
        Serial.print(params->peerAddr[index], HEX);
        Serial.print(" ");  
    }
    Serial.println(" ");  

    Serial.print("  The Rssi : ");
    Serial.println(params->rssi, DEC);

    uint8_t len;
    uint8_t adv_name[31];
    if( NRF_SUCCESS == ble_advdata_decode(BLE_GAP_AD_TYPE_SHORT_LOCAL_NAME, params->advertisingDataLen, (uint8_t *)params->advertisingData, &len, adv_name) )
    {
        Serial.print("  The length of Short Local Name : ");
        Serial.println(len, HEX);
        Serial.print("  The Short Local Name is        : ");
        Serial.println((const char *)adv_name);
        if(0x00 == memcmp(adv_name, "Biscuit", min(7, len)))
        {
          ble.stopScan();
          ble.connect(params->peerAddr, Gap::ADDR_TYPE_RANDOM_STATIC, NULL, NULL);         
        }
    }
    else if( NRF_SUCCESS == ble_advdata_decode(BLE_GAP_AD_TYPE_COMPLETE_LOCAL_NAME, params->advertisingDataLen, (uint8_t *)params->advertisingData, &len, adv_name) )
    {
        Serial.print("  The length of Complete Local Name : ");
        Serial.println(len, HEX);
        Serial.print("  The Complete Local Name is        : ");
        Serial.println((const char *)adv_name);
        if(0x00 == memcmp(adv_name, "HRM1", min(4, len)))
        {
          ble.stopScan();
          ble.connect(params->peerAddr, Gap::ADDR_TYPE_RANDOM_STATIC, NULL, NULL);         
        }
    }
    Serial.println(" ");  
    Serial.println(" ");  
}

void discoveredCallback(const DiscoveredDevice *device)
{
    Serial.println("----------------discoveredCallback----------------");
    Serial.print("Device connHandle : ");
    Serial.println(device->connHandle, HEX);
    Serial.print("service number : ");
    Serial.println(device->currNumServices, DEC);  

    for(uint8_t i=0; i<device->currNumServices; i++)
    {
        Serial.print("Service uuid : ");
        Serial.println(device->service[i].service.getUUID().getShortUUID(), HEX);
        Serial.print("Service start handle : ");
        Serial.println(device->service[i].service.getStartHandle(), HEX);
        Serial.print("Service end handle : ");
        Serial.println(device->service[i].service.getEndHandle(), HEX);
        Serial.print("chars number of this service: ");
        Serial.println(device->service[i].currNumChars, DEC);
        for(uint8_t n=0; n<device->service[i].currNumChars; n++)
        {
            Serial.print("chars uuid : ");
            Serial.println(device->service[i].chars[n].getUUID().getShortUUID(), HEX);   
            Serial.print("char declaration handle : ");
            Serial.println(device->service[i].chars[n].getDeclHandle(), HEX);       
            Serial.print("char value handle : ");
            Serial.println(device->service[i].chars[n].getValueHandle(), HEX);        
            Serial.print("char descriptor handle : ");
            Serial.println(device->service[i].chars[n].getDescHandle(), HEX);     
            Serial.print("char cccd handle : ");
            Serial.println(device->service[i].chars[n].getCCCDHndle(), HEX);  
            Serial.print("char propertis : ");
            Serial.println(device->service[i].chars[n].getProperties()._broadcast, DEC);   
            Serial.println(device->service[i].chars[n].getProperties()._read, DEC);    
            Serial.println(device->service[i].chars[n].getProperties()._writeWoResp, DEC);    
            Serial.println(device->service[i].chars[n].getProperties()._write, DEC);    
            Serial.println(device->service[i].chars[n].getProperties()._notify, DEC);    
            Serial.println(device->service[i].chars[n].getProperties()._indicate, DEC);    
            Serial.println(device->service[i].chars[n].getProperties()._authSignedWrite, DEC);                                                        
        }
    }
  
    uint16_t value = 0x2356;
    //ble.gattClient().read(device->connHandle, device->service[2].chars[1].getValueHandle(), 0);
    //ble.gattClient().write(GattClient::GATT_OP_WRITE_CMD,device->connHandle,device->service[2].chars[0].getValueHandle(),2,(uint8_t *)&value);
    //ble.gattClient().write(GattClient::GATT_OP_WRITE_REQ, device->connHandle, device->service[2].chars[1].getCCCDHndle(),2,(uint8_t *)&value);
}

void terminateCallback(uint8_t reason)
{
    Serial.print("terminateCallback reason : ");
    Serial.println(reason, HEX);
}
 
// GAP call back handle
void connectionCallBack(const Gap::ConnectionCallbackParams_t *params)
{
    Serial.println("GAP_EVT_CONNECTED");    
    Serial.print("The conn handle : ");
    Serial.println(params->handle, HEX);
    Serial.print("The role : ");
    Serial.println(params->role, HEX);
    Serial.print("PeerAddr type is : ");
    Serial.println(params->peerAddrType, HEX);
    Serial.print("PeerAddr is : ");
    for(uint8_t index=0; index<6; index++)
    {
        Serial.print(params->peerAddr[index], HEX);
        Serial.print(" ");
    }
    Serial.println(" ");

    ble.gattClient().launchServiceDiscovery(params->handle, discoveredCallback);
}

void disconnectionCallBack(Gap::Handle_t handle, Gap::DisconnectionReason_t reason)
{
    Serial.println("Disconnected ");
}

void onDataWrite(const GattCharacteristicWriteCBParams *params)
{
    Serial.println("GattClient write call back ");    
}

void onDataRead(const GattCharacteristicReadCBParams *params)
{
    Serial.println("GattClient read call back ");      
    Serial.print("The handle : ");
    Serial.println(params->handle, HEX);  
    Serial.print("The offset : ");
    Serial.println(params->offset, DEC); 
    Serial.print("The len : ");
    Serial.println(params->len, DEC); 
    Serial.print("The data : ");
    for(uint8_t index=0; index<params->len; index++)
    {
        Serial.print( params->data[index], HEX);    
    }
    Serial.println("");
}

void hvxCallBack(const GattCharacteristicHVXCallbackParams *params)
{
    Serial.println("GattClient notify call back \r\n");  
    Serial.print("The len : ");
    Serial.println(params->len, DEC); 
    for(unsigned char index=0; index<params->len; index++)
    {
        Serial.print(params->data[index], HEX);  
    }
    Serial.println("");
}
 
void setup() 
{
    // put your setup code here, to run once:
    Serial.begin(9600);
    delay(8000);
    Serial.println("Start..."); 
    
    ble.init();
    ble.onConnection(connectionCallBack);
    ble.onDisconnection(disconnectionCallBack);
    ble.gattClient().terminateServiceDiscovery(terminateCallback);
    ble.gattClient().onHVX(hvxCallBack);
    ble.gattClient().onDataWritten(onDataWrite);
    ble.gattClient().onDataRead(onDataRead);
   
     //Note : take care of scheduler, prevent ram leak.See projectconfig.h
    ble.startScan(scanCallBack);
}

void loop() 
{
    // put your main code here, to run repeatedly:
    ble.waitForEvent();

//    if(state == 0xAA)
//    { state = 0;
//      Serial.println("---------------11111111111111111----------------");
//      Serial.print("Device connHandle : ");
//      Serial.println(HRM.connHandle, HEX);
//      Serial.print("service number : ");
//      Serial.println(HRM.currNumServices, DEC);  
//  
//      for(uint8_t i=0; i<HRM.currNumServices; i++)
//      {
//          Serial.print("Service uuid : ");
//          Serial.println(HRM.service[i].service.getUUID().getShortUUID(), HEX);
//          Serial.print("Service start handle : ");
//          Serial.println(HRM.service[i].service.getStartHandle(), HEX);
//          Serial.print("Service end handle : ");
//          Serial.println(HRM.service[i].service.getEndHandle(), HEX);
//          Serial.print("chars number of this service: ");
//          Serial.println(HRM.service[i].currNumChars, DEC);
//          for(uint8_t n=0; n<HRM.service[i].currNumChars; n++)
//          {
//              Serial.print("chars uuid : ");
//              Serial.println(HRM.service[i].chars[n].getUUID().getShortUUID(), HEX);   
//              Serial.print("char declaration handle : ");
//              Serial.println(HRM.service[i].chars[n].getDeclHandle(), HEX);       
//              Serial.print("char value handle : ");
//              Serial.println(HRM.service[i].chars[n].getValueHandle(), HEX);        
//              Serial.print("char descriptor handle : ");
//              Serial.println(HRM.service[i].chars[n].getDescHandle(), HEX);     
//              Serial.print("char cccd handle : ");
//              Serial.println(HRM.service[i].chars[n].getCCCDHndle(), HEX);  
//              Serial.print("char propertis : ");
//              Serial.println(HRM.service[i].chars[n].getProperties()._broadcast, DEC);   
//              Serial.println(HRM.service[i].chars[n].getProperties()._read, DEC);    
//              Serial.println(HRM.service[i].chars[n].getProperties()._writeWoResp, DEC);    
//              Serial.println(HRM.service[i].chars[n].getProperties()._write, DEC);    
//              Serial.println(HRM.service[i].chars[n].getProperties()._notify, DEC);    
//              Serial.println(HRM.service[i].chars[n].getProperties()._indicate, DEC);    
//              Serial.println(HRM.service[i].chars[n].getProperties()._authSignedWrite, DEC);                                                        
//          }
//      }
//    }
}
