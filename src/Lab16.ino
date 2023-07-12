#include "oled-wing-adafruit.h"

SYSTEM_MODE(MANUAL);
SYSTEM_THREAD(ENABLED);

#define yellowLED D2
#define blueLED D3

OledWingAdafruit display;

const size_t UART_TX_BUF_SIZE = 20;

void onDataReceived(const uint8_t *data, size_t len, const BlePeerDevice &peer, void *context);

const BleUuid serviceUuid("6E400001-B5A3-F393-E0A9-E50E24DCCA9E");
const BleUuid rxUuid("6E400002-B5A3-F393-E0A9-E50E24DCCA9E");
BleCharacteristic rxCharacteristic("rx", BleCharacteristicProperty::WRITE_WO_RSP, rxUuid, serviceUuid, onDataReceived, NULL);
const BleUuid txUuid("6E400003-B5A3-F393-E0A9-E50E24DCCA9E");
BleCharacteristic txCharacteristic("tx", BleCharacteristicProperty::NOTIFY, txUuid, serviceUuid);

void setup()
{
  BLE.on();

  pinMode(yellowLED, OUTPUT);
  pinMode(blueLED, OUTPUT);

  BLE.addCharacteristic(txCharacteristic);
  BLE.addCharacteristic(rxCharacteristic);

  BleAdvertisingData data;
  data.appendServiceUUID(serviceUuid);
  BLE.advertise(&data);
  BLE.setDeviceName("AshleyLibasciArduino");

  uint8_t txBuf[10];                    // an array of 10 bytes we can send
  txCharacteristic.setValue(txBuf, 10); // here we are sending all 10 bytes

  display.setup();
  display.clearDisplay();
  display.display();
}

void loop()
{
  if (BLE.connected())
  {
    analogWrite(redLED, 255);
    analogWrite(greenLED, 255);
    analogWrite(blueLED, 0);

    uint8_t txBuf[UART_TX_BUF_SIZE];
    String message = "Hello!";
    message.toCharArray((char *)txBuf, message.length() + 1);
    txCharacteristic.setValue(txBuf, message.length() + 1);
  }
  else
  {
    analogWrite(redLED, 0);
    analogWrite(greenLED, 0);
    analogWrite(blueLED, 255);
  }
}

void onDataReceived(const uint8_t *data, size_t len, const BlePeerDevice &peer, void *context)
{
  for (size_t i = 0; i < len; i++)
  {
    Serial.write(data[i]);

    if (data[i] == 1)
    {
      display.loop();
      display.clearDisplay();
      display.setTextSize(1);
      display.setTextColor(WHITE);
      display.setCursor(0, 0);
      display.println("iPhone sent '1' ");
      display.display();
    }
  }
}
