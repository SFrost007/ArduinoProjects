#include <math.h>
#include <Arduino.h>
#include <SPI.h>
#include <BLEPeripheral.h>

const int pinRedLED = 3;
const int pinGreenLED = 5;
const int pinBlueLED = 6;

int loopCount;
const int blinkPIN = 7;

#define BLE_REQ   9
#define BLE_RDY   8
#define BLE_RST   9
BLEPeripheral ble = BLEPeripheral(BLE_REQ, BLE_RDY, BLE_RST);
BLEService lightbulbService("00000043-0000-1000-8000-0026BB765291");
BLELongCharacteristic lightCharacteristic("00000008-0000-1000-8000-0026BB765291", BLERead | BLEWrite | BLENotify);

void setup() {
  Serial.begin(115200);
  pinMode(pinRedLED, OUTPUT);
  pinMode(pinGreenLED, OUTPUT);  
  pinMode(pinBlueLED, OUTPUT);
  pinMode(blinkPIN, OUTPUT);

  ble.setAdvertisedServiceUuid(lightbulbService.uuid());
  
  ble.addAttribute(lightbulbService);
  ble.addAttribute(lightCharacteristic);

  lightCharacteristic.setValueLE(0);
  lightCharacteristic.setEventHandler(BLEWritten, characteristicWrite);

  ble.begin();
  Serial.println("Bluetooth on");
}

void loop() {
  ble.poll();
  if (++loopCount % 10000 == 0) {
    digitalWrite(blinkPIN, HIGH);
    loopCount = 0;
  } else if (loopCount % 5000 == 0) {
    digitalWrite(blinkPIN, LOW);
  }
}

void characteristicWrite(BLECentral& central, BLECharacteristic& characteristic) {
  Serial.print("Characteristic written | ");
  Serial.println(characteristic.uuid());

  unsigned long value = (unsigned long)lightCharacteristic.value();
  int r = value & 0xFF0000;
  int g = value & 0x00FF00;
  int b = value & 0x0000FF;

  Serial.println("Colors:");
  Serial.print("R: ");
  Serial.println(r);
  Serial.print("G: ");
  Serial.println(g);
  Serial.print("B: ");
  Serial.println(b);
}

void setLED(float red, float green, float blue) {
  analogWrite(pinRedLED, red);
  analogWrite(pinGreenLED, green);
  analogWrite(pinBlueLED, blue);
}

