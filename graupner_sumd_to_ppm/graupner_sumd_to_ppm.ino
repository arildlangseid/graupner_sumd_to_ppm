#include "Wire.h"
#include "Adafruit_GFX.h"
#include "Adafruit_SSD1306.h"

Adafruit_SSD1306 display(-1);

#define DIS_WIDTH 128 // OLED display width, in pixels
#define DIS_HEIGHT 64 // OLED display height, in pixels

void setup() {
  Serial.begin(115200);
  long start = millis();
  while (!Serial) {
    ; // wait for native USB CDC to be ready
  }
  long end = millis();
  Serial.print(F("USB delay:"));
  Serial.println(end-start);

  start = millis();
  Serial1.begin(115200);
  while (!Serial1) {
    ; // wait for native USB CDC to be ready
  }
  end = millis();
  Serial.print(F("UART delay:"));
  Serial.println(end-start);

  //Initialize display by providing the display type and its I2C address.
  /*
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;); // Don't proceed, loop forever
  }
  */
  //delay(2000); // Pause for 2 seconds

  //String string;
  
//  display.clearDisplay();
}

#define NUM_CHANNELS 8
int i, channel[NUM_CHANNELS], rcValue[NUM_CHANNELS];

#define SUMD_MAXCHAN NUM_CHANNELS
#define SUMD_BUFFERSIZE SUMD_MAXCHAN*2+5

static uint8_t sumdIndex=0;
static uint8_t sumdSize=0;
static uint8_t sumd[SUMD_BUFFERSIZE]={0};

void loop() {
  // put your main code here, to run repeatedly:

  while (Serial1.available()) {
    int val = Serial1.read();
    if (sumdIndex == 0 && val !=0xA8) {continue;}
    if (sumdIndex == 2) { sumdSize = val; }
    if (sumdIndex < SUMD_BUFFERSIZE ) {sumd[sumdIndex] = val;}
    sumdIndex++;

    if (sumdIndex == sumdSize*2+5) {
      if (sumdSize > SUMD_MAXCHAN) sumdSize = SUMD_MAXCHAN;
      for (uint8_t b = 0; b < sumdSize; b++) {
        rcValue[b] = ((sumd[2*b+3]<<8 | sumd[2*b+4]))>>3;
        if (rcValue[b]>750 && rcValue[b]<2250) { channel[b] = rcValue[b];}
      }
    }
    //if (val==0xA8) Serial.println("SUMD-frame Found.");
  }

  Serial.print(channel[0]);
  for (uint8_t i=1;i<NUM_CHANNELS;i++) {
    Serial.print("\t");
    Serial.print(channel[i]);
  }
  Serial.println("");
  delay(50);

  return;

/*
  display.clearDisplay();
  display.display();
  delay(3000);
  display.setTextSize(1);             // The fontsize
  display.setTextColor(WHITE);        // Draw white text
  display.setCursor(0, 5);           // Start at top-left corner
  display.print("HelloWorld");
  display.display();
  delay(3000);
*/
}
