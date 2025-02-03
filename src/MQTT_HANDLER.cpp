//MQTT

#include "MQTTHandler.hpp"
#include "MIDIHandler.hpp"
#include "ESP_NOW.hpp"
#include "LEDHandler.hpp"

const char* ssid = "WIFI";
const char* password = "weoriuewi";
const char* mqttServer = "broker.hivemq.com";
const int mqttPort = 1883;

WiFiClient espClient;
PubSubClient client(espClient);


void setupWiFi() {
  delay(10);
  Serial.println("Connecting to WiFi...");
  WiFi.begin(ssid, password);
  Serial.println(ssid);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting...");
  }
  Serial.println("WiFi connected!");
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Connecting to MQTT broker...");
    if (client.connect("ESP32_Test_Client")) {
      Serial.println("Connected!");
      client.subscribe("test/topic");  // Subscribe to test topic
    } else {
      Serial.print("Failed, rc=");
      Serial.print(client.state());
      delay(5000);
    }
  }
}

// Publish Simple Message 
void publishMessage() {
  String message = "Bread is Nice with butter";
  client.publish("test/topic", message.c_str());
  Serial.println("Published: " + message);
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("]: ");

  String midiString = "";
  for (int i = 0; i < length; i++) {
    midiString += (char)payload[i];
  }


  // Parse midiString
  uint8_t cableNumber = midiString.substring(2, 3).toInt();  // Extract cable number
  uint8_t channel = midiString.substring(midiString.indexOf("channel: ") + 9, midiString.indexOf("value:") - 1).toInt();
  uint8_t value = midiString.substring(midiString.indexOf("value: ") + 7).toInt();

  String status = "";
  if (midiString.indexOf("Note On") != -1) {
    status = "Note On";
  } else if (midiString.indexOf("Note Off") != -1) {
    status = "Note Off";
  } else if (midiString.indexOf("Control Change") != -1) {
    status = "Control Change";
  } else {
    status = "Other";
  }

  // Update struct and process
  MidiReading.channel = channel;
  MidiReading.value = value;
  MidiReading.statusByte = (status == "Note On") ? 0x90 : (status == "Note Off") ? 0x80 : 0xB0;
  MidiReading.cableNumber = cableNumber;

  // Print parsed data
  Serial.print("Parsed - channel: ");
  Serial.print(channel);
  Serial.print(", value: ");
  Serial.print(value);
  Serial.print(", Status: ");
  Serial.println(status);

  // Process LEDs based on note on/off
  if (status == "Note On" && value > 0) {
    lightUpLED(channel, value);
  } else if (status == "Note Off" || (status == "Note On" && value == 0)) {
    turnOffLED(channel);
  }
}



void setUpMqtt(){

 setupWiFi();
  client.setServer(mqttServer, mqttPort);
  client.setCallback(callback);

}

void loopMqtt(){

 if (!client.connected()) {
   reconnect();
  }
  client.loop();

}

