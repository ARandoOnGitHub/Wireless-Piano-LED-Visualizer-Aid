#include <Arduino.h>
#include <FastLED.h>
#include "LEDHandler.hpp"
#include "USBHandler.hpp"
#include "ESP_NOW.hpp"
#include "ESP_BLUETOOTH.hpp"
#include "MQTTHandler.hpp"
#include "Website.hpp"

bool sendMidiUpdate = false;


static void sendESP32Log(const String& message) {
  Serial.print(message); // Print message to Serial
}


void setup() {
  Serial.begin(115200);
  Serial.println("Booting up!");

  // Give USB some time to initialize (especially if you're using TinyUSB MIDI)
  //ESP_Now setup
  // initESP_NOW();
  // readMacAddress();
  // Bluetooth setup 

  bluetoothSetup();
  WiFiManager wm;
  if (!wm.autoConnect("Piano_Visualizer", "piano123")) {
    Serial.println("Failed to connect.");
    ESP.restart();
  }
  
  delay(500);  // give time for WiFi to stabilize
  Serial.println("WiFi connected. Starting web server...");
  WebsiteSetup();  // ✅ Only start after config portal exits
  Serial.print("ESP Local IP: ");
  Serial.println(WiFi.localIP());
  // FastLED.addLeds<WS2812B, ONBOARD_LED_PIN, GRB>(onboardLed, 1);
  FastLED.addLeds<WS2812B, LED_PIN, GRB>(leds, NUM_LEDS);
  FastLED.setBrightness(BRIGHTNESS); // Optional: for uniform brightness
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = CRGB::Black;
  }
  FastLED.show();
  setupLEDMapping();
  //MQTT setup
  setUpMqtt(); 
  setupUSB();
  // setupOnboardLED();
}

void loop() {
  handleUSB();
  loopMqtt();
  // sendESP32Log();
  // Serial.println(message);
  
}
