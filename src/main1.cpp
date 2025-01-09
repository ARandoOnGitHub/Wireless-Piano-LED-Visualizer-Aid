#include <Arduino.h>
#include <FastLED.h>

// Define board types with unique values
#define ESP32    1
#define ESP32S2  2
#define ESP32S3  3

// Define the actual board type (change this based on your board)
#define BOARD_TYPE  ESP32S3 // select your board: ESP32, ESP32S2, or ESP32S3

// LED Strip Configurations
#define LED_PIN     3         // Pin where the WS2812B is connected
#define NUM_LEDS    144        // Number of LEDs (e.g., 88 keys of a piano)
#define BRIGHTNESS  255       // Max brightness

CRGB leds[NUM_LEDS];          // Array to hold the LED states

#if BOARD_TYPE == ESP32S3
#include <usb/usb_host.h>
#include "usbhhelp.hpp"
#elif BOARD_TYPE == ESP32S2
#include <usb/usb_host.h>
#include "usbhhelp.hpp"
#elif BOARD_TYPE == ESP32
#else
#error "Unsupported board type!"
#endif

#if BOARD_TYPE == ESP32S3 || BOARD_TYPE == ESP32S2
static void sendESP32Log(const String& message) {
  Serial.print(message); // Print message to Serial
}
void lightUpLED(uint8_t note, uint8_t velocity);
void turnOffLED(uint8_t note);


static void midi_transfer_cb(usb_transfer_t *transfer) {
  ESP_LOGI("", "midi_transfer_cb context: %d", transfer->context);
  if (Device_Handle == transfer->device_handle) {
    int in_xfer = transfer->bEndpointAddress & USB_B_ENDPOINT_ADDRESS_EP_DIR_MASK;
    if ((transfer->status == 0) && in_xfer) {
      uint8_t *const p = transfer->data_buffer;
      for (int i = 0; i < transfer->actual_num_bytes; i += 4) {
        if ((p[i] + p[i + 1] + p[i + 2] + p[i + 3]) == 0) break;
        ESP_LOGI("", "midi: %02x %02x %02x %02x", p[i], p[i + 1], p[i + 2], p[i + 3]);

        // Parse MIDI data
        uint8_t cableNumber = p[i] & 0x0F;
        
        uint8_t statusByte = p[i + 1];
        uint8_t channel = p[i + 2];  // For Note On/Off, this is the note number. For CC, it's the controller number.
        uint8_t value = p[i + 3];    // For Note On/Off, this is the velocity. For CC, it's the controller value.

        // Format the parsed MIDI data as a string
        char midiString[50];  // Adjust the size as needed
        snprintf(midiString, sizeof(midiString), "Ch%d %s Channel: %d Value: %d",
                 cableNumber, (statusByte >= 0x80 && statusByte < 0x90) ? "Note Off" : (statusByte >= 0x90 && statusByte < 0xA0) ? "Note On"
                 : (statusByte >= 0xB0 && statusByte < 0xC0) ? "Control Change"
                 : "Other",
                 channel, value);

        // Raw data
        Serial.println(midiString);

        // Process MIDI messages and update LEDs
        switch (statusByte & 0xF0) {
          case 0x80: // Note Off
            sendESP32Log("USB MIDI IN: NOTE OFF Pitch: " + String(channel) + " Velocity: " + String(value));
            turnOffLED(channel); // Turn off the corresponding LED for this note
            break;

          case 0x90: // Note On
            if (value == 0) {
              sendESP32Log("USB MIDI IN: NOTE OFF Pitch: " + String(channel) + " Velocity: " + String(value));
              turnOffLED(channel); // Handle Note Off as velocity 0
            } else {
              sendESP32Log("USB MIDI IN: NOTE ON Pitch: " + String(channel) + " Velocity: " + String(value));
              lightUpLED(channel, value); // Light up the LED corresponding to the note, passes channel (note) and value (velocity) into lightup function
            }
            break;

          case 0xB0: // Control Change
            // Process Control Change messages (e.g., sustain pedal)
            switch (channel) {
              case 64: // Sustain Pedal
                sendESP32Log("USB MIDI IN: Sustain Pedal CC " + String(channel) + " Value: " + String(value));
                break;

              case 67: // Soft Pedal
                sendESP32Log("USB MIDI IN: Soft Pedal CC " + String(channel) + " Value: " + String(value));
                break;

              case 66: // Sostenuto Pedal
                sendESP32Log("USB MIDI IN: Sostenuto Pedal CC " + String(channel) + " Value: " + String(value));
                break;
            }
            break;
        }
      }
      esp_err_t err = usb_host_transfer_submit(transfer);
      if (err != ESP_OK) {
        ESP_LOGI("", "usb_host_transfer_submit In fail: %x", err);
      }
    } else {
      ESP_LOGI("", "transfer->status %d", transfer->status);
    }
  }
}
#endif
// uint8_t &channel;

#if BOARD_TYPE == ESP32S3 || BOARD_TYPE == ESP32S2
bool isMIDI = false;
bool isMIDIReady = false;

const size_t MIDI_IN_BUFFERS = 8;
const size_t MIDI_OUT_BUFFERS = 8;
usb_transfer_t *MIDIOut = NULL;
usb_transfer_t *MIDIIn[MIDI_IN_BUFFERS] = { NULL };

void check_interface_desc_MIDI(const void *p) {
  const usb_intf_desc_t *intf = (const usb_intf_desc_t *)p;

  // USB MIDI
  if ((intf->bInterfaceClass == USB_CLASS_AUDIO) && (intf->bInterfaceSubClass == 3) && (intf->bInterfaceProtocol == 0)) {
    isMIDI = true;
    ESP_LOGI("", "Claiming a MIDI device!");
    esp_err_t err = usb_host_interface_claim(Client_Handle, Device_Handle,
                    intf->bInterfaceNumber, intf->bAlternateSetting);
    if (err != ESP_OK) ESP_LOGI("", "usb_host_interface_claim failed: %x", err);
  }
}

void prepare_endpoints(const void *p) {
  const usb_ep_desc_t *endpoint = (const usb_ep_desc_t *)p;
  esp_err_t err;

  // must be bulk for MIDI
  if ((endpoint->bmAttributes & USB_BM_ATTRIBUTES_XFERTYPE_MASK) != USB_BM_ATTRIBUTES_XFER_BULK) {
    ESP_LOGI("", "Not bulk endpoint: 0x%02x", endpoint->bmAttributes);
    return;
  }
  if (endpoint->bEndpointAddress & USB_B_ENDPOINT_ADDRESS_EP_DIR_MASK) {
    for (int i = 0; i < MIDI_IN_BUFFERS; i++) {
      err = usb_host_transfer_alloc(endpoint->wMaxPacketSize, 0, &MIDIIn[i]);
      if (err != ESP_OK) {
        MIDIIn[i] = NULL;
        ESP_LOGI("", "usb_host_transfer_alloc In fail: %x", err);
      } else {
        MIDIIn[i]->device_handle = Device_Handle;
        MIDIIn[i]->bEndpointAddress = endpoint->bEndpointAddress;
        MIDIIn[i]->callback = midi_transfer_cb;
        MIDIIn[i]->context = (void *)i;
        MIDIIn[i]->num_bytes = endpoint->wMaxPacketSize;
        esp_err_t err = usb_host_transfer_submit(MIDIIn[i]);
        if (err != ESP_OK) {
          ESP_LOGI("", "usb_host_transfer_submit In fail: %x", err);
        }
      }
    }
  } else {
    err = usb_host_transfer_alloc(endpoint->wMaxPacketSize, 0, &MIDIOut);
    if (err != ESP_OK) {
      MIDIOut = NULL;
      ESP_LOGI("", "usb_host_transfer_alloc Out fail: %x", err);
      return;
    }
    ESP_LOGI("", "Out data_buffer_size: %d", MIDIOut->data_buffer_size);
    MIDIOut->device_handle = Device_Handle;
    MIDIOut->bEndpointAddress = endpoint->bEndpointAddress;
    MIDIOut->callback = midi_transfer_cb;
    MIDIOut->context = NULL;
  }
  isMIDIReady = ((MIDIOut != NULL) && (MIDIIn[0] != NULL));
}

void show_config_desc_full(const usb_config_desc_t *config_desc) {
  // Full decode of config desc.
  const uint8_t *p = &config_desc->val[0];
  uint8_t bLength;
  for (int i = 0; i < config_desc->wTotalLength; i += bLength, p += bLength) {
    bLength = *p;
    if ((i + bLength) <= config_desc->wTotalLength) {
      const uint8_t bDescriptorType = *(p + 1);
      switch (bDescriptorType) {
        case USB_B_DESCRIPTOR_TYPE_CONFIGURATION:
          break;
        case USB_B_DESCRIPTOR_TYPE_INTERFACE:
          if (!isMIDI) check_interface_desc_MIDI(p);
          break;
        case USB_B_DESCRIPTOR_TYPE_ENDPOINT:
          if (isMIDI && !isMIDIReady) {
            prepare_endpoints(p);
          }
          break;
        default:
          break;
      }
    } else {
      return;
    }
  }
}
#endif

// Function to light up the corresponding LED for a note
#define NUM_LEDS 144   // Total number of LEDs in the strip
#define NUM_KEYS 88    // Total keys on an 88-key piano


// Helper function to determine if a note is a white key
bool isWhiteKey(uint8_t note) {
  // MIDI notes corresponding to white keys (C, D, E, F, G, A, B)
  int whiteNotes[] = {21, 23, 24, 26, 28, 29, 31, 33, 35, 36, 38, 40, 41, 43, 45, 47, 48, 50, 52, 53, 55, 57, 59, 60, 62, 64, 65, 67, 69, 71, 72, 74, 76, 77, 79, 81, 83, 84, 86, 88, 89, 91, 93, 95, 96, 98, 100, 101, 103, 105, 107};
  for (int i = 0; i < sizeof(whiteNotes) / sizeof(whiteNotes[0]); i++) {
    if (note == whiteNotes[i]) return true;
  }
  return false;
}
// Key LED start indices
int keyLEDStart[NUM_KEYS];  // Array to store the starting LED index for each key

// void setupLEDMapping() {
//   int ledIndex = 0;
//   int totalWhiteKeys = 0;
//   int totalBlackKeys = 0;

//   // Traverse from MIDI note 21 to 108 (88 keys total)
//   for (int note = 21; note <= 108; note++) {
//     if (isWhiteKey(note)) {
//       // Assign 2 LEDs to white keys
//       keyLEDStart[note - 21] = ledIndex;
//       ledIndex += 2;
//       totalWhiteKeys++;
//     } else {
//       // Assign 1 LED to black keys
//       keyLEDStart[note - 21] = ledIndex;
//       ledIndex += 1;
//       totalBlackKeys++;
//     }

//     // Ensure we don't go beyond the LED strip length
//     if (ledIndex >= NUM_LEDS) {
//       ledIndex = NUM_LEDS - 1; // Clamp to the last LED
//     }
//   }
// }

// void lightUpLED(uint8_t note, uint8_t velocity) {
//   if (note < 21 || note > 108) return; // Ignore notes outside the piano range

//   int ledIndex = keyLEDStart[note - 21]; // Get the starting LED index for this note

//   if (isWhiteKey(note)) {
//     // Light up 2 LEDs for white keys
//     leds[ledIndex] = CHSV(map(note, 21, 108, 0, 255), 255, map(velocity, 0, 127, 0, BRIGHTNESS));
//     leds[ledIndex + 1] = CHSV(map(note, 21, 108, 0, 255), 255, map(velocity, 0, 127, 0, BRIGHTNESS));
//   } else {
//     // Light up 1 LED for black keys
//     leds[ledIndex] = CHSV(map(note, 21, 108, 0, 255), 255, map(velocity, 0, 127, 0, BRIGHTNESS));
//   }

//   FastLED.show();
// }

// void turnOffLED(uint8_t note) {
//   if (note < 21 || note > 108) return; // Ignore notes outside the piano range

//   int ledIndex = keyLEDStart[note - 21]; // Get the starting LED index for this note

//   if (isWhiteKey(note)) {
//     // Turn off 2 LEDs for white keys
//     leds[ledIndex] = CRGB::Black;
//     leds[ledIndex + 1] = CRGB::Black;
//   } else {
//     // Turn off 1 LED for black keys
//     leds[ledIndex] = CRGB::Black;
//   }

//   FastLED.show();
// }

////////////////
void setupLEDMapping() {
  int ledIndex = 0;
  int keyGroup = 0;
  int noteInGroup = 0;

  for (int note = 21; note <= 108; note++) { // Traverse from MIDI note 21 to 108 (88 keys total)
    keyLEDStart[note - 21] = ledIndex;

    if (isWhiteKey(note)) {
      ledIndex += 2;  // Assign 2 LEDs to white keys
    } else {
      ledIndex += 1;  // Assign 1 LED to black keys
    }

    noteInGroup++;
    if (noteInGroup == 3) {
      noteInGroup = 0;
      keyGroup++;
      
      // Add a small gap between octaves for better visual separation
      ledIndex += 1;
    }

    // Ensure we don't go beyond the LED strip length
    if (ledIndex >= NUM_LEDS) {
      ledIndex = NUM_LEDS - 1;  // Clamp to the last LED
    }
  }
}


void lightUpLED(uint8_t note, uint8_t velocity) {
  if (note < 21 || note > 108) return;  // Ignore notes outside the piano range

  int ledIndex = keyLEDStart[note - 21]; // Get the starting LED index for this note
  CHSV color = CHSV(map(note, 21, 108, 0, 255), 255, map(velocity, 0, 127, 0, BRIGHTNESS));

  leds[ledIndex] = color;
  if (isWhiteKey(note) && ledIndex + 1 < NUM_LEDS) {
    leds[ledIndex + 1] = color;
  }

  FastLED.show();
}

void turnOffLED(uint8_t note) {
  if (note < 21 || note > 108) return; // Ignore notes outside the piano range

  int ledIndex = keyLEDStart[note - 21]; // Get the starting LED index for this note

  leds[ledIndex] = CRGB::Black;
  if (isWhiteKey(note) && ledIndex + 1 < NUM_LEDS) {
    leds[ledIndex + 1] = CRGB::Black;
  }

  FastLED.show();
}


/////////////////




void setup() {
  Serial.begin(115200);
  FastLED.addLeds<WS2812B, LED_PIN, GRB>(leds, NUM_LEDS);
  FastLED.setBrightness(BRIGHTNESS);

  // Initialize the LED mappings
  setupLEDMapping();

  ////////////
    Serial.begin(115200);
  Serial.println("Booting up!");

  // Initialize FastLED
  FastLED.addLeds<WS2812B, LED_PIN, GRB>(leds, NUM_LEDS);
  FastLED.setBrightness(BRIGHTNESS);

  // Initialize all LEDs to off
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = CRGB::Black;
  }
  FastLED.show();

  // USB setup
#if BOARD_TYPE == ESP32S3 || BOARD_TYPE == ESP32S2
  usbh_setup(show_config_desc_full);  // Init USB host for MIDI devices
#endif
}








void loop() {
  // Handle USB
#if BOARD_TYPE == ESP32S3 || BOARD_TYPE == ESP32S2
  usbh_task();
#endif
}
