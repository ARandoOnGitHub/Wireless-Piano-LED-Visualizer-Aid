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

uint8_t broadcastAddress[] = {0xa0, 0x85, 0xe3, 0xe7, 0x4b, 0x68}; //Master Device
uint8_t broadcastAddress1[] = {0xa0, 0x85, 0xe3, 0xe6, 0x56, 0x1c};
uint8_t broadcastAddress2[] = {0x48, 0xca, 0x43, 0xaf, 0x28, 0x2c};
uint8_t broadcastAddress3[] = {0xa0, 0x85, 0xe3, 0xe7, 0x55, 0xe8};

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
  char macStr[18];
  Serial.print("Packet from: ");
  // Copies the sender mac address to a string
  snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
           mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
  Serial.print(macStr);
  Serial.print(" send status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}

void onDataReceive(const uint8_t *mac_addr, const uint8_t *data, int len) {


  
    memcpy(&MidiReading, data, sizeof(MidiReading));

   
   
   

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

        case 0xB0:
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


void addPeer(const uint8_t *peerAddr) {
  esp_now_peer_info_t newPeer = {};
  memcpy(newPeer.peer_addr, peerAddr, 6);
  newPeer.channel = 0;
  newPeer.encrypt = false;

  if (esp_now_add_peer(&newPeer) != ESP_OK) {
      Serial.println("Failed to add peer");
  } else {
      Serial.println("Peer added successfully");
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
  //Number 1 Peer
  
  // addPeer(broadcastAddress);
  addPeer(broadcastAddress1);
  addPeer(broadcastAddress2);
  addPeer(broadcastAddress3);

   
}