#include "USBHandler.hpp"
#include "MIDIHandler.hpp"

#if BOARD_TYPE == ESP32S3 || BOARD_TYPE == ESP32S2
#include <usb/usb_host.h>
#include "usbhhelp.hpp"

// static void sendESP32Log(const String& message) {
//   Serial.print(message); // Print message to Serial
// }

bool isMIDI = false;
bool isMIDIReady = false;

const size_t MIDI_IN_BUFFERS = 8;
usb_transfer_t *MIDIIn[MIDI_IN_BUFFERS] = { NULL };

static void midi_transfer_cb(usb_transfer_t *transfer) {
  if (Device_Handle == transfer->device_handle) {
    if ((transfer->status == 0) && (transfer->bEndpointAddress & USB_B_ENDPOINT_ADDRESS_EP_DIR_MASK)) {
      // uint8_t *const p = transfer->data_buffer;
      // for (int i = 0; i < transfer->actual_num_bytes; i += 4) {
      //   if ((p[i] + p[i + 1] + p[i + 2] + p[i + 3]) == 0) break;
      //   Serial.printf("MIDI: %02x %02x %02x %02x\n", p[i], p[i + 1], p[i + 2], p[i + 3]);
      }

      //From OG Code
      uint8_t *const p = transfer->data_buffer;
      for (int i = 0; i < transfer->actual_num_bytes; i += 4) {
        if ((p[i] + p[i + 1] + p[i + 2] + p[i + 3]) == 0) break;
        ESP_LOGI("", "midi: %02x %02x %02x %02x",
                 p[i], p[i + 1], p[i + 2], p[i + 3]);
                 

      
      processMIDI(transfer->data_buffer, transfer->actual_num_bytes);
      usb_host_transfer_submit(transfer);
    }
  }
}

void check_interface_desc_MIDI(const void *p) {
  const usb_intf_desc_t *intf = (const usb_intf_desc_t *)p;
  if ((intf->bInterfaceClass == USB_CLASS_AUDIO) && (intf->bInterfaceSubClass == 3) && (intf->bInterfaceProtocol == 0)) {
    isMIDI = true;
    usb_host_interface_claim(Client_Handle, Device_Handle, intf->bInterfaceNumber, intf->bAlternateSetting);
  }
}

void prepare_endpoints(const void *p) {
  const usb_ep_desc_t *endpoint = (const usb_ep_desc_t *)p;
  if ((endpoint->bmAttributes & USB_BM_ATTRIBUTES_XFERTYPE_MASK) != USB_BM_ATTRIBUTES_XFER_BULK) return;

  if (endpoint->bEndpointAddress & USB_B_ENDPOINT_ADDRESS_EP_DIR_MASK) {
    for (int i = 0; i < MIDI_IN_BUFFERS; i++) {
      usb_host_transfer_alloc(endpoint->wMaxPacketSize, 0, &MIDIIn[i]);
      MIDIIn[i]->device_handle = Device_Handle;
      MIDIIn[i]->bEndpointAddress = endpoint->bEndpointAddress;
      MIDIIn[i]->callback = midi_transfer_cb;
      MIDIIn[i]->context = (void *)i;
      MIDIIn[i]->num_bytes = endpoint->wMaxPacketSize;
      usb_host_transfer_submit(MIDIIn[i]);
    }
  }
}

void show_config_desc_full(const usb_config_desc_t *config_desc) {
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

void setupUSB() {
  usbh_setup(show_config_desc_full);
}

void handleUSB() {
  usbh_task();
}
#endif