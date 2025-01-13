#ifndef LEDHANDLER_HPP
#define LEDHANDLER_HPP

#include <Arduino.h>
#include <FastLED.h>

// LED Strip Configurations
#define LED_PIN     3
#define NUM_LEDS    144
#define BRIGHTNESS  255
#define NUM_KEYS    88
#define OCTAVE_GAP  1

extern CRGB leds[NUM_LEDS];
extern int keyLEDStart[NUM_KEYS];
extern uint8_t noteToColor[88];

void setupLEDMapping();
void lightUpLED(uint8_t note, uint8_t velocity);
void turnOffLED(uint8_t note);

#endif // LEDHANDLER_HPP
