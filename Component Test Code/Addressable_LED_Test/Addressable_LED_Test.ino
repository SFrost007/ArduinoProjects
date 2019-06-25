#include <Adafruit_NeoPixel.h>

#define PIN 4
#define LED_COUNT 102

Adafruit_NeoPixel leds = Adafruit_NeoPixel(LED_COUNT, PIN, NEO_GRB + NEO_KHZ800);

void setup() {
  leds.begin();
  clearLEDs();
  leds.show();
}

void loop() {
  for (int i=0; i<LED_COUNT; i++) {
    clearLEDs();
    leds.setPixelColor(i, 0xFF0000);
    leds.show();
    delay(100);
  }
}

void clearLEDs() {
  for (int i=0; i<LED_COUNT; i++) {
    leds.setPixelColor(i, 0);
  }
}

