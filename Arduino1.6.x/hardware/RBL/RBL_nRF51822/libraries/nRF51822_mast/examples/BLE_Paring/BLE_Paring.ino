
#include <nRF51822_API.h>
#include <BLE_API.h>

#define BLE_UUID_TXRX_SERVICE            0x0000 /**< The UUID of the Nordic UART Service. */
#define BLE_UUID_TX_CHARACTERISTIC       0x0002 /**< The UUID of the TX Characteristic. */
#define BLE_UUIDS_RX_CHARACTERISTIC      0x0003 /**< The UUID of the RX Characteristic. */

#define TXRX_BUF_LEN                     20

#define BLE_SEC_PARAM_BOND		       0			/**< Perform bonding. */
#define BLE_SEC_PARAM_MITM                     1                        /**< Man In The Middle protection not required. */

static uint8_t passkey[] = "123456";


static app_timer_id_t                    m_1s_id; 
static uint8_t value=0;

BLEDevice       ble;

// The Nordic UART Service
static const uint8_t uart_base_uuid[]       = {0x71, 0x3D, 0, 0, 0x50, 0x3E, 0x4C, 0x75, 0xBA, 0x94, 0x31, 0x48, 0xF1, 0x8D, 0x94, 0x1E};
static const uint8_t uart_tx_uuid[]         = {0x71, 0x3D, 0, 3, 0x50, 0x3E, 0x4C, 0x75, 0xBA, 0x94, 0x31, 0x48, 0xF1, 0x8D, 0x94, 0x1E};
static const uint8_t uart_rx_uuid[]         = {0x71, 0x3D, 0, 2, 0x50, 0x3E, 0x4C, 0x75, 0xBA, 0x94, 0x31, 0x48, 0xF1, 0x8D, 0x94, 0x1E};
static const uint8_t uart_base_uuid_rev[]   = {0x1E, 0x94, 0x8D, 0xF1, 0x48, 0x31, 0x94, 0xBA, 0x75, 0x4C, 0x3E, 0x50, 0, 0, 0x3D, 0x71};

uint8_t txPayload[TXRX_BUF_LEN] = {0,};
uint8_t rxPayload[TXRX_BUF_LEN] = {0,};


GattCharacteristic  txCharacteristic (uart_tx_uuid, txPayload, 1, TXRX_BUF_LEN, GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_WRITE | GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_WRITE_WITHOUT_RESPONSE );

GattCharacteristic  rxCharacteristic (uart_rx_uuid, rxPayload, 1, TXRX_BUF_LEN, GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_NOTIFY);

GattCharacteristic *uartChars[] = {&txCharacteristic, &rxCharacteristic};

GattService         uartService(uart_base_uuid, uartChars, sizeof(uartChars) / sizeof(GattCharacteristic *));


void error_handler(uint32_t error_code, uint32_t line_num, const uint8_t * p_file_name)
{
    Serial.println("##---------------------Error Code-------------------##");
    Serial.print("The file name : ");
    Serial.println((const char *)p_file_name);
    Serial.print("The error line : ");
    Serial.println(line_num, DEC);
    Serial.print("The error code : ");
    Serial.println(error_code, HEX);
    Serial.println("##--------------------------------------------------##");
}

void connectionCallback(Gap::Handle_t handle,                                                    \
                        Gap::addr_type_t peerAddrType, const Gap::Address_t peerAddr,            \
                        Gap::addr_type_t ownAddrType,  const Gap::Address_t ownAddr,             \
                        const Gap::ConnectionParams_t *params)
{
    ble_gap_sec_params_t sec_params;
    
    Serial.println("GAP_EVT_CONNECTED");    
    sec_params.bond    = BLE_SEC_PARAM_BOND;
    sec_params.mitm    = BLE_SEC_PARAM_MITM;
    sec_params.io_caps = Gap::IO_CAPS_DISPLAY_ONLY;
    sec_params.oob     = 0;
    sec_params.min_key_size = 16;
    sec_params.max_key_size = 16;
    sec_params.kdist_periph.enc = 1;     
    sec_params.kdist_periph.id = 1;  
    sec_params.kdist_periph.sign = 1;    
    APP_ERROR_CHECK( sd_ble_gap_authenticate(handle, &sec_params) );
}

void disconnectionCallback(Gap::Handle_t handle, Gap::DisconnectionReason_t reason)
{
    Serial.println("Disconnected ");
    Serial.println("Restart advertising ");
    ble.startAdvertising();
}

void SecurityInitiatedCB(Gap::Handle_t handle, bool allowBonding, bool requireMITM, Gap::SecurityIOCapabilities_t iocaps)
{
    Serial.println("Security:Initiate CB");
}

void SecurityCompletedCB(Gap::Handle_t handle, Gap::SecurityCompletionStatus_t status)
{
    Serial.println("Security:Completed CB");
    switch(status)
    {
        case Gap::SEC_STATUS_PASSKEY_ENTRY_FAILED:
        {
            Serial.println("Status:SEC_STATUS_PASSKEY_ENTRY_FAILED");
            ble.disconnect(Gap::REMOTE_USER_TERMINATED_CONNECTION);
        }break;
        
        case Gap::SEC_STATUS_CONFIRM_VALUE:
        {
            Serial.println("Status:SEC_STATUS_CONFIRM_VALUE");
            ble.disconnect(Gap::REMOTE_USER_TERMINATED_CONNECTION);
        }break;
        
        case Gap::SEC_STATUS_TIMEOUT:
        {
            Serial.println("Status:SEC_STATUS_TIMEOUT");
            ble.disconnect(Gap::REMOTE_USER_TERMINATED_CONNECTION);
        }break;
      
        default:
            Serial.println("Status:Other");
        break;
    
    }
}

void SecurityPasskeyDisplayCB(Gap::Handle_t handle, const Gap::Passkey_t passkey)
{
    Serial.println("Security:Display Passkey");
    Serial.print("The Passkey : ");
    Serial.println((const char *)passkey);
}

void WrittenHandler(const GattCharacteristicWriteCBParams *Handler)
{
    uint8_t buf[TXRX_BUF_LEN];
    uint16_t bytesRead, index;

    Serial.println("Write Handle : ");
    if (Handler->charHandle == txCharacteristic.getValueAttribute().getHandle())
    {
        ble.readCharacteristicValue(txCharacteristic.getValueAttribute().getHandle(), buf, &bytesRead);
        memset(txPayload, 0, TXRX_BUF_LEN);
        memcpy(txPayload, buf, TXRX_BUF_LEN);
        for(byte index=0; index<bytesRead; index++)
        {
            Serial.print(buf[index], HEX);
        }
        Serial.println("");
    }
}

void m_1s_handle(void * p_context)
{
    Serial.println("1S timeout ");
    value++;
    if(value >= 100)
    {
        value = 0;
    }   
    ble.updateCharacteristicValue(rxCharacteristic.getValueAttribute().getHandle(), &value, 1);
}

void setup() 
{
    uint32_t err_code = NRF_SUCCESS;
    // put your setup code here, to run once
    Serial.begin(9600);
    Serial.println("Start ");
    pinMode(13, OUTPUT);
    pinMode(7, INPUT_PULLUP);
 
    app_error_handler_register(error_handler);
    
    ble.init();
    ble.onDisconnection(disconnectionCallback);
    ble.onDataWritten(WrittenHandler);
    ble.onConnection(connectionCallback);
    
    // Set security params 
    if(digitalRead(7) == LOW)
        ble.initializeSecurity(true, BLE_SEC_PARAM_BOND, BLE_SEC_PARAM_MITM, Gap::IO_CAPS_DISPLAY_ONLY, passkey);
    else
        ble.initializeSecurity(false, BLE_SEC_PARAM_BOND, BLE_SEC_PARAM_MITM, Gap::IO_CAPS_DISPLAY_ONLY, passkey);
    //ble.initializeSecurity(false, BLE_SEC_PARAM_BOND, BLE_SEC_PARAM_MITM, Gap::IO_CAPS_DISPLAY_ONLY, NULL);
    ble.onSecuritySetupInitiated(SecurityInitiatedCB);
    ble.onSecuritySetupCompleted(SecurityCompletedCB);
    ble.onPasskeyDisplay(SecurityPasskeyDisplayCB);
  
    // setup advertising
    ble.accumulateAdvertisingPayload(GapAdvertisingData::BREDR_NOT_SUPPORTED);
    ble.setAdvertisingType(GapAdvertisingParams::ADV_CONNECTABLE_UNDIRECTED);
    ble.accumulateAdvertisingPayload(GapAdvertisingData::SHORTENED_LOCAL_NAME,
                                   (const uint8_t *)"Biscuit", sizeof("Biscuit") - 1);
    ble.accumulateAdvertisingPayload(GapAdvertisingData::COMPLETE_LIST_128BIT_SERVICE_IDS,
                                   (const uint8_t *)uart_base_uuid_rev, sizeof(uart_base_uuid));
    // 100ms; in multiples of 0.625ms.
    ble.setAdvertisingInterval(160);

    //txCharacteristic.requireSecurity(Gap::SECURITY_MODE_ENCRYPTION_NO_MITM);
    ble.addService(uartService);
  
    ble.setDeviceName((const uint8_t *)"Serial UART");

    ble.startAdvertising();

    Serial.println("start advertising ");
  
    err_code = app_timer_create(&m_1s_id,APP_TIMER_MODE_REPEATED, m_1s_handle);
    APP_ERROR_CHECK(err_code);	
  
    //err_code = app_timer_start(m_1s_id, APP_TIMER_TICKS(1000, 0), NULL);
    //APP_ERROR_CHECK(err_code);  
}

void loop() 
{
    ble.waitForEvent();
}
