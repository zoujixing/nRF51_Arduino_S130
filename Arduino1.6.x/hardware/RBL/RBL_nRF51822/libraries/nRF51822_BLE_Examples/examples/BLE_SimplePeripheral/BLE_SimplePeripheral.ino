
#include <nRF51822_API.h>
#include <BLE_API.h>

#define BLE_UUID_TXRX_SERVICE            0x0000 /**< The UUID of the Nordic UART Service. */
#define BLE_UUID_TX_CHARACTERISTIC       0x0002 /**< The UUID of the TX Characteristic. */
#define BLE_UUIDS_RX_CHARACTERISTIC      0x0003 /**< The UUID of the RX Characteristic. */

#define TXRX_BUF_LEN                     20

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

static void connectCallBack(const Gap::ConnectionCallbackParams_t *params)
{
    Serial.println("connected  ");
    Serial.print("connect handle : ");
    Serial.println(params->handle, HEX);
    Serial.print("connect role : ");
    Serial.println(params->role, DEC);   //1:peripheral, 2:cenral
}

static void disconnectionCallBack(Gap::Handle_t handle, Gap::DisconnectionReason_t reason)
{
    Serial.println("Disconnected ");
    Serial.println("Restart advertising ");
    ble.startAdvertising();
}

static void timeoutCallBack()
{
    Serial.println("Advertising Timeout ");
}

// GATT call back handle
void writtenHandle(const GattCharacteristicWriteCBParams *Handler)
{
    uint8_t buf[TXRX_BUF_LEN];
    uint16_t bytesRead, index;

    Serial.println("Write Handle : ");
    if (Handler->handle == txCharacteristic.getValueAttribute().getHandle())
    {
        ble.readCharacteristicValue(txCharacteristic.getValueAttribute().getHandle(), buf, &bytesRead);
        memset(txPayload, 0, TXRX_BUF_LEN);
        memcpy(txPayload, buf, TXRX_BUF_LEN);
        for(byte index=0; index<bytesRead; index++)
        {
            Serial.print(buf[index], HEX);
        }
        Serial.println(" ");
    }
}

// Task handle
void m_1s_handle(void * p_context)
{
    Serial.println("1s Loop ");
    value++;
    if(value >= 100)
    {
        value = 0;
    }   
    ble.updateCharacteristicValue(rxCharacteristic.getValueAttribute().getHandle(), &value, 1);
}

void setup() {
    
    // put your setup code here, to run once
    Serial.begin(9600);
    pinMode(13, OUTPUT);
    
    app_error_handler_register(error_handler);

    Serial.println("Start ");
    ble.init();

    ble.onConnection(connectCallBack);
    ble.onDisconnection(disconnectionCallBack);
    ble.onDataWritten(writtenHandle);
    ble.onTimeout(timeoutCallBack);
      
    // setup adv_data and srp_data
    ble.accumulateAdvertisingPayload(GapAdvertisingData::BREDR_NOT_SUPPORTED | GapAdvertisingData::LE_GENERAL_DISCOVERABLE);
    
    ble.accumulateAdvertisingPayload(GapAdvertisingData::SHORTENED_LOCAL_NAME,
                                     (const uint8_t *)"Biscuit", sizeof("Biscuit") - 1);
    ble.accumulateAdvertisingPayload(GapAdvertisingData::COMPLETE_LIST_128BIT_SERVICE_IDS,
                                     (const uint8_t *)uart_base_uuid_rev, sizeof(uart_base_uuid));                               
    // add service
    ble.addService(uartService);
    // set device name
    ble.setDeviceName((const uint8_t *)"Serial UART");
    // set appearance
    ble.setAppearance(BLE_APPEARANCE_UNKNOWN);
    // set tx power,valid values are -40, -20, -16, -12, -8, -4, 0, 4
    ble.setTxPower(4);
    // set adv_type
    ble.setAdvertisingType(GapAdvertisingParams::ADV_CONNECTABLE_UNDIRECTED);
    // set adv_interval, 100ms in multiples of 0.625ms.
    ble.setAdvertisingInterval(160);
    // set adv_timeout, in seconds
    ble.setAdvertisingTimeout(0);
    // ger BLE stack version
    Serial.println( ble.getVersion() );
    // start advertising
    ble.startAdvertising();

    Serial.println("start advertising ");
  
    APP_ERROR_CHECK( app_timer_create(&m_1s_id,APP_TIMER_MODE_REPEATED, m_1s_handle) );
    APP_ERROR_CHECK( app_timer_start(m_1s_id, APP_TIMER_TICKS(1000, 0), NULL) );
}

void loop() {
    ble.waitForEvent();
}
