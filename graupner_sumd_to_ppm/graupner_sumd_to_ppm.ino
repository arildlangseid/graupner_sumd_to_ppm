#include "PPMEncoder.h"

#define PPM_OUTPUT_PIN 10

//#define PRINT_DEBUG

void setup() {
  ppmEncoder.begin(PPM_OUTPUT_PIN);

#ifdef PRINT_DEBUG
  Serial.begin(115200);
  while (!Serial && !stopWaitForUSB) {
    ; // wait for native USB CDC to be ready
  }
#endif

  Serial1.begin(115200);
  while (!Serial1) {
    ; // wait for native USB CDC to be ready
  }

}

#define NUM_CHANNELS 8
int channel[NUM_CHANNELS], rcValue[NUM_CHANNELS];

#define SUMD_MAXCHAN NUM_CHANNELS
#define SUMD_BUFFERSIZE SUMD_MAXCHAN*2+5

static uint8_t sumdIndex=0;
static uint8_t sumdSize=0;
static uint8_t sumd[SUMD_BUFFERSIZE]={0};

void loop() {
  while (Serial1.available()) {
    int val = Serial1.read();
    if (sumdIndex == 0 && val !=0xA8) { continue; }
    if (sumdIndex == 2) { sumdSize = val; }
    if (sumdIndex < SUMD_BUFFERSIZE ) { sumd[sumdIndex] = val; }
    sumdIndex++;

    if (sumdIndex == sumdSize*2+5) {
      if (sumdSize > SUMD_MAXCHAN) sumdSize = SUMD_MAXCHAN;
      for (uint8_t b = 0; b < sumdSize; b++) {
        rcValue[b] = ((sumd[2*b+3]<<8 | sumd[2*b+4]))>>3;
        if (rcValue[b]>750 && rcValue[b]<2250) {
#ifdef PRINT_DEBUG
          channel[b] = rcValue[b];
#endif
          ppmEncoder.setChannel(b, rcValue[b]);
        }
      }
    }
  }

#ifdef PRINT_DEBUG
  debug();
#endif

}

void debug() {
  Serial.print(channel[0]);
  for (uint8_t i=1;i<NUM_CHANNELS;i++) {
    Serial.print("\t");
    Serial.print(channel[i]);
  }
  Serial.println("");
}
