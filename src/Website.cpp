#include "Website.hpp"
#include "LEDHandler.hpp"
#include "ESP_NOW.hpp"
#include "WebsiteHTML.hpp"
#include "MQTTHandler.hpp"

IPAddress local_IP(192, 168, 5, 245);
IPAddress gateway(192, 168, 5, 245);
IPAddress subnet(255, 255, 255, 0);

uint8_t currentHue=80;
uint8_t currentBrightness=100;
uint8_t bluetoothHue=80;
uint8_t bluetoothBrightness=100;
uint8_t ESPHUE=80;
uint8_t EspBrightness=100;
bool BlueBool = false;

const char* PARAM_INPUT_1 = "output";
const char* PARAM_INPUT_2 = "state";

AsyncWebServer server(80);

//

CRGB onboardLed[1];

void setupOnboardLED() {
  FastLED.addLeds<WS2812B, ONBOARD_LED_PIN, GRB>(onboardLed, 1);
}

void flashOnboardLED(uint8_t hue, uint8_t brightness) {
  onboardLed[0] = CHSV(hue, 255, brightness);
  FastLED.show();
  delay(500);
  onboardLed[0] = CRGB::Black;  // turn it off
  FastLED.show();
}

//

String scanNearbyDevices() {
  String result = "";
  int n = WiFi.scanNetworks(false, true);
  if (n == 0) {
    result = "No devices found";
  } else {
    for (int i = 0; i < n; ++i) {
      result += "SSID: " + WiFi.SSID(i);
      result += " | MAC: " + WiFi.BSSIDstr(i);
    }
  }
  return result;
}

bool stringToBool(String value) {
  value.toLowerCase();
  return value == "1" || value == "true" || value == "yes" || value == "on";
}

void setupAP() {
  WiFi.mode(WIFI_AP_STA);  // Support both hosting a web UI and connecting to a router

  // Create an AP so you can access the webpage locally
  WiFi.softAPConfig(local_IP, gateway, subnet);
  WiFi.softAP("Piano Visualizer", "piano123");

  // Connect to real Wi-Fi for MQTT to work
  WiFi.begin(ssid, password);

  WiFi.setSleep(false);

  Serial.print("AP IP address: ");
  Serial.println(WiFi.softAPIP());

} 

void handleRootRequest(AsyncWebServerRequest *request) {
  request->send(200, "text/html", index_html);
}

void handleMacRequest(AsyncWebServerRequest *request) {
  request->send(200, "text/plain", readMacAddress());
}

void handleScanRequest(AsyncWebServerRequest *request) {
  request->send(200, "text/plain", scanNearbyDevices());
}

void handleSendRequest(AsyncWebServerRequest *request) {
  int newHue = currentHue;
  int newBrightness = currentBrightness;
  int blueHue = bluetoothHue;
  int espHue = ESPHUE;
  bool bluetoothOn = BlueBool;

  if (request->hasParam("hue")) {
    newHue = constrain(request->getParam("hue")->value().toInt(), 0, 255);
  }
  if (request->hasParam("bright")) {
    newBrightness = constrain(request->getParam("bright")->value().toInt(), 0, 255);
  }
  if (request->hasParam("bluehue")) {
    blueHue = constrain(request->getParam("bluehue")->value().toInt(), 0, 255);
  }
  if (request->hasParam("incominghue")) {
    espHue = constrain(request->getParam("incominghue")->value().toInt(), 0, 255);
  }
  if (request->hasParam("bluetooth")) {
    bluetoothOn = stringToBool(request->getParam("bluetooth")->value());
  }

  // Apply updates (with concurrency safety in mind)
  //noInterrupts();
  currentHue = newHue;
  currentBrightness = newBrightness;
  //Serial.println("Received new hue/brightness from website:");
  //Serial.printf("Hue: %d, Brightness: %d\n", newHue, newBrightness);
  bluetoothHue = blueHue;
  bluetoothBrightness = newBrightness;
  ESPHUE = espHue;
  EspBrightness = newBrightness;
  BlueBool = bluetoothOn;
  MidiReading.hue = ESPHUE;
  MidiReading.brightness = EspBrightness;
  flashOnboardLED(currentHue, newBrightness);
  flashOnboardLED(ESPHUE, newBrightness);
  flashOnboardLED(bluetoothHue, newBrightness);
  //interrupts();

  esp_err_t result = esp_now_send(0, (uint8_t *)&MidiReading, sizeof(MidiReading));
  //sendMidiUpdate = true;

  String response = "{\n";
  response += "  \"hue\": " + String(currentHue) + ",\n";
  response += "  \"brightness\": " + String(currentBrightness) + ",\n";
  response += "  \"bluehue\": " + String(bluetoothHue) + ",\n";
  response += "  \"bluetooth\": " + String(bluetoothOn ? "true" : "false") + ",\n";
  response += "  \"espHue\": " + String(ESPHUE) + "\n";
  response += "}";

  request->send(200, "application/json", response);
  //lightUpLED(60, 100);  // Show the new color
  //delay(300);
  //turnOffLED(60);       // Then turn it off
  Serial.println(response);
}

void handleUpdateRequest(AsyncWebServerRequest *request) {
  String inputMessage1 = request->getParam(PARAM_INPUT_1)->value();
  String inputMessage2 = request->getParam(PARAM_INPUT_2)->value();

  digitalWrite(inputMessage1.toInt(), inputMessage2.toInt());

  int pin = inputMessage1.toInt();
  int state = inputMessage2.toInt();
  if (pin == 2 && state == 0) {
    turnOffLED(60);
  }

  Serial.printf("GPIO: %s - Set to: %s\n", inputMessage1.c_str(), inputMessage2.c_str());
  request->send(200, "text/plain", "OK");
}

void WebsiteSetup() {
  pinMode(2, OUTPUT);
  pinMode(4, OUTPUT);
  pinMode(33, OUTPUT);
  digitalWrite(2, LOW);
  digitalWrite(4, LOW);
  digitalWrite(33, LOW);

  //setupAP();
  if (MDNS.begin("PianoVisualizer")) {
    Serial.println("Access your ESP32 at: http://PianoVisualizer.local");
  }

  server.on("/", HTTP_GET, handleRootRequest);
  server.on("/mac", HTTP_GET, handleMacRequest);
  server.on("/scan", HTTP_GET, handleScanRequest);
  server.on("/send", HTTP_GET, handleSendRequest);
  server.on("/update", HTTP_GET, handleUpdateRequest);

  Serial.println("Starting web server...");
  server.begin();
  Serial.println("Web server started!");
}