#include <Arduino.h>
#include <FastLED.h>
#include "LEDHandler.hpp"
#include "USBHandler.hpp"
#include "ESP_NOW.hpp"
#include "ESP_BLUETOOTH.hpp"

static void sendESP32Log(const String& message) {
  Serial.print(message); // Print message to Serial
}


void setup() {
  Serial.begin(115200);
  Serial.println("Booting up!");



  //ESP_NOW Adress Setup
 
 

  // Serial.print("[DEFAULT] ESP32 Board MAC Address: ");
  // readMacAddress();
  initESP_NOW();
  
  // Bluetooth setup 

  bluetoothSetup();

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

  // sendESP32Log();
  // Serial.println(message);



}
