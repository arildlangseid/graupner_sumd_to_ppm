#include "PPMEncoder.h"

// SUMD in is connected to Rx-pin
// Code at the moment for Arduino Leonardo

// Pin for PPM out
#define PPM_OUTPUT_PIN 10

// Uncomment for debug-printing
//#define PRINT_DEBUG

#define NUM_CHANNELS 8
#define SUMD_MAXCHAN NUM_CHANNELS
#define SUMD_BUFFERSIZE SUMD_MAXCHAN*2+5

#ifdef PRINT_DEBUG
static int channel[NUM_CHANNELS];
#endif
static uint8_t sumd[SUMD_BUFFERSIZE]={0};

void setup() {
  ppmEncoder.begin(PPM_OUTPUT_PIN);

#ifdef PRINT_DEBUG
  Serial.begin(115200);
  while (!Serial) {
    ; // wait for native USB CDC to be ready
  }
#endif

  // SUMD serial params 115200 baud, 8 bit, none parity, 1 stop bit
  Serial1.begin(115200);
}

static uint8_t sumdIndex=0;
static uint8_t sumdSize=0;
static uint8_t channelCounter=0;
void loop() {
  if (Serial1.available()) {
    int val = Serial1.read();
    if (sumdIndex == 0 && val !=0xA8) { return; }
    if (sumdIndex == 1 && val !=0x01) { sumdIndex = 0; return; }
    if (sumdIndex == 2) { sumdSize = val; }
    if (sumdIndex < SUMD_BUFFERSIZE ) { sumd[sumdIndex] = val; }

    if (sumdIndex > 2 && sumdIndex % 2 == 0 && channelCounter < NUM_CHANNELS) {
      /*sumdIndex is even*/
      int rcValue = ((sumd[sumdIndex-1]<<8 | sumd[sumdIndex]))>>3;
      if (rcValue>750 && rcValue<2250) {
        ppmEncoder.setChannel(channelCounter, rcValue);
        #ifdef PRINT_DEBUG
          channel[channelCounter] = rcValue;
        #endif
        channelCounter++;
      }
    }
    sumdIndex++;

    if (sumdIndex == sumdSize*2+5) {
      sumdIndex = 0;
      channelCounter = 0;
#ifdef PRINT_DEBUG
      debug();
#endif
    }
  }
}

#ifdef PRINT_DEBUG
void debug() {
  Serial.print(channel[0]);
  for (uint8_t i=1;i<NUM_CHANNELS;i++) {
    Serial.print("\t");
    Serial.print(channel[i]);
  }
  Serial.println("");
}
#endif