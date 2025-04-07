#include <Arduino.h>
#include <FastLED.h>
#include "LEDHandler.hpp"
#include "USBHandler.hpp"
#include "ESP_NOW.hpp"
#include "ESP_BLUETOOTH.hpp"
#include "MQTTHandler.hpp"
#include "Website.hpp"


bool usbHandled = false;
bool usbSetup=false; 

static void sendESP32Log(const String& message) {
  Serial.print(message); // Print message to Serial
}


void setup() {
  Serial.begin(115200);
  Serial.println("Booting up!");

  
  setupUSB(); 

  // Give USB some time to initialize (especially if you're using TinyUSB MIDI)
  //ESP_Now setup
    initESP_NOW();
   readMacAddress();
  // Bluetooth setup 
  bluetoothSetup();
  WebsiteSetup();

  FastLED.addLeds<WS2812B, LED_PIN, GRB>(leds, NUM_LEDS);
  FastLED.setBrightness(BRIGHTNESS);

  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = CRGB::Black;
  }
  FastLED.show();

  setupLEDMapping();
  //MQTT setup
  //setUpMqtt(); 

  setupUSB();

}

void loop() {
  
    handleUSB();
 

 // loopMqtt();
  // sendESP32Log();
  // Serial.println(message);
}
