//MQTT

#include "MQTTHandler.hpp"
#include "MIDIHandler.hpp"
#include "ESP_NOW.hpp"
#include "LEDHandler.hpp"

const char* ssid = "MyOptimum 744650";
const char* password = "1935-silver-84";
const char* mqttServer = "broker.hivemq.com";
const int mqttPort = 1883;
char clientID[30];

WiFiClient espClient;
PubSubClient client(espClient);

  struct_MQTT midiReadingMqtt; 

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

    uint32_t chipId = ESP.getEfuseMac() & 0xFFFFFF;  // Get part of the MAC address
    snprintf(clientID, sizeof(clientID), "ESP32_Client_%06X", chipId);

    if (client.connect(clientID)) {
      Serial.println("Connected!");
       //client.subscribe("midi/topic",0);  
         client.subscribe("test/topic",0); // Subscribe to test topic
    } else {
      Serial.print("Failed, rc=");
      Serial.print(client.state());
      delay(1000);
    }
  }
}

// Publish Simple Message 
void publishMIDI(String MidiMessage) {
  
  client.publish("midi/topic", MidiMessage.c_str(),false);
   //client.publish("test/topic", MidiMessage.c_str(),false);
  Serial.println("Published: " + String(MidiMessage));
 
}

void callback(char* topic, byte* payload, unsigned int length) {
 
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("]: ");

  // Use a fixed-size buffer instead of String
  char midiBuffer[50];  
  memcpy(midiBuffer, payload, length);
  midiBuffer[length] = '\0';  // Null-terminate

  // Serial.println(midiBuffer);  // Debugging output

  // Variables to store extracted values
  int midiChannel = 0, noteNumber = 0, velocity = 0;
  char noteStatus[10];  // Holds "On" or "Off"

  // Corrected sscanf format
  if (sscanf(midiBuffer, "Ch%d Note %s Channel: %d Value: %d", &midiChannel, noteStatus, &noteNumber, &velocity) == 4) {
      Serial.printf(" Parsed Successfully - MIDI Channel: %d, Note: %d, Value: %d, Status: %s\n",
                    midiChannel, noteNumber, velocity, noteStatus);

      // Update struct
      midiReadingMqtt.Channel = midiChannel;
      midiReadingMqtt.Value = velocity;
      midiReadingMqtt.StatusByte = (strcmp(noteStatus, "On") == 0) ? 0x90 : 0x80;

      // Process LEDs
      if (strcmp(noteStatus, "On") == 0 && velocity > 0) {
          lightUpLED(noteNumber, velocity);
      } else if (strcmp(noteStatus, "Off") == 0 || (strcmp(noteStatus, "On") == 0 && velocity == 0)) {
          turnOffLED(noteNumber);
      }
  } else {
      Serial.println(" Parsing failed! Check message format.");
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

