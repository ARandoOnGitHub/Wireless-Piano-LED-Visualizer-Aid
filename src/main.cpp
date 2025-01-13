#include <Arduino.h>
#include <FastLED.h>
#include "LEDHandler.hpp"
#include "USBHandler.hpp"

void setup() {
  Serial.begin(115200);
  Serial.println("Booting up!");

  FastLED.addLeds<WS2812B, LED_PIN, GRB>(leds, NUM_LEDS);
  FastLED.setBrightness(BRIGHTNESS);

  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = CRGB::Black;
  }
  FastLED.show();

  setupLEDMapping();
  setupUSB();
}

void loop() {
  handleUSB();
}
