#include "ESP_NOW.hpp"
#include "LEDHandler.hpp"
void readMacAddress(){

uint8_t baseMac[6];
  esp_err_t ret = esp_wifi_get_mac(WIFI_IF_STA, baseMac);
  if (ret == ESP_OK) {
    Serial.printf("%02x:%02x:%02x:%02x:%02x:%02x\n",
                  baseMac[0], baseMac[1], baseMac[2],
                  baseMac[3], baseMac[4], baseMac[5]);
  } else {
    Serial.println("Failed to read MAC adadress");
  } 

}

uint8_t broadcastAddress[] = {0xa0, 0x85, 0xe3, 0xe7, 0x4b, 0x68};
 struct_message MidiReading;
// typedef struct struct_message{

// // char midiString[50];
// uint8_t Channel; 
// uint8_t Value; 
// uint8_t StatusByte; 
// uint8_t CableNumber; 

// } struct_message;

// struct_message MidiReading; 

 esp_now_peer_info_t peerInfo;

// void sendMidiData() {
//     esp_err_t result = esp_now_send(broadcastAddress, (uint8_t*)&MidiReading, sizeof(MidiReading));
    
//     // if (result == ESP_OK) {
//     //     Serial.println("MIDI data sent successfully!");
//     // } else {
//     //     Serial.println("Error sending MIDI data.");
//     // }
// }


void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("\r\nLast Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}

void onDataReceive(const uint8_t *mac_addr, const uint8_t *data, int len) {


    // Copy received data into MidiReading struct
    memcpy(&MidiReading, data, sizeof(MidiReading));

    // Extract actual status (message type) and MIDI channel
   
   

    switch ( MidiReading.statusByte & 0xF0) {
        case 0x80: // Note Off
            Serial.println("USB MIDI IN: NOTE OFF Pitch: " + String(MidiReading.channel) + " Velocity: " + String(MidiReading.value));
            turnOffLED(MidiReading.channel); // Handle Note Off logic here
            break;

        case 0x90: // Note On
            if (MidiReading.value == 0) {
                // MIDI standard treats Note On with velocity 0 as Note Off
                Serial.println("USB MIDI IN: NOTE OFF Pitch: " + String(MidiReading.channel) + " Velocity: " + String(MidiReading.value));
                 turnOffLED(MidiReading.channel);
            } else {
                 Serial.println("USB MIDI IN: NOTE ON Pitch: " + String(MidiReading.channel) + " Velocity: " + String(MidiReading.value));
                lightUpLED(MidiReading.channel, MidiReading.value);
            }
            break;

        case 0xB0: // Control Change
            switch (MidiReading.channel) {
                case 64: // Sustain Pedal
                     Serial.println("USB MIDI IN: Sustain Pedal CC " + String(MidiReading.channel) + " Velocity: " + String(MidiReading.value));
                    break;
                case 67: // Soft Pedal
                    Serial.println("USB MIDI IN: Soft Pedal CC " + String(MidiReading.channel) + " Velocity: " + String(MidiReading.value));
                    break;
                case 66: // Sostenuto Pedal
                     Serial.println("USB MIDI IN: Sostenuto Pedal CC " + String(MidiReading.channel) + " Velocity: " + String(MidiReading.value));
                    break;
               
            }
            break;
        default:  Serial.println("UNKOWN MIDI DATA " + String(MidiReading.channel) + " Velocity: " + String(MidiReading.value));
        break;
      
    }
}


void initESP_NOW(){
  
    WiFi.mode(WIFI_STA);

     if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
   esp_now_register_recv_cb(esp_now_recv_cb_t(onDataReceive));
  esp_now_register_send_cb(OnDataSent);

  // Set up a peer device (receiver's MAC address required)
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);  // You'll need to set receiver's MAC address
  peerInfo.channel = 0;  // Default channel
  peerInfo.encrypt = false;

  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Failed to add peer");
    return;
  }

   
}