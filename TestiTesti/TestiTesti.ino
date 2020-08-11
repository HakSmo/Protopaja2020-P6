
/* WIRING:

    T3.6      | DEVICE  PIN
    -----------------------------
    0 (RX1)   | CAMERA  6 (TX)
    1 (TX1)   | CAMERA  7 (RX)
    8         | DISPLAY 9 (DC)
    9         | DISPLAY 8 (RST)
    10(CS0)   | DISPLAY 7 (TFTCS)
    11(MOSI0) | DISPLAY 6 (MOSI)
    13(SCK0)  | DISPLAY 4 (SCK)

*/

#include "epc635_commands.h"
#include "crc_calc.hpp"
#include <SPI.h>
#include <ST7735_t3.h>
#include "colorLut.h"
#include <vector>
#define CAMSERIAL Serial1
#define TFT_MISO  12
#define TFT_MOSI  11  //a12
#define TFT_SCK   13  //a13
#define TFT_DC   8
#define TFT_CS   10
#define TFT_RST  9
ST7735_t3 tft = ST7735_t3(TFT_CS, TFT_DC, TFT_RST);
const int ledPin = 13;
unsigned char single_frame[14] =  {SND, GET_DIST, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x62, 0xAC, 0xA8, 0xCC};
unsigned char disable_median_filter[14] = {SND, SET_MEDIAN_FILTER, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
unsigned char enable_median_filter[14] = {SND, SET_MEDIAN_FILTER, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
unsigned char set_amplitude_limit[14] = {SND, SET_AMPLITUDE_LIMIT, 0x00, 15, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}; // Edit the 4th element to set the limit.
uint16_t target_pixel; // Used for storing displays pixel data in camera data handling.
uint16_t data_buf = 0;// Buffer for camera pixel data.
void setup() {

  Serial.begin(9600);
  pinMode(ledPin, OUTPUT);
  // initializes the display
  Serial.println("Init display");
  tft.initR(INITR_MINI160x80);  // Init ST7735S mini display


  tft.fillScreen(ST77XX_BLACK);
  tft.setRotation(3);
  Serial.println("Display initialized");

  delay(1000);
  CAMSERIAL.begin(10000000);
  
  /*Use the send_command_settings(unsigned char *command); function to set camera settings. You only need to send unsigned char command[14] with the command and data, crc is automatically calculated.
  NOTE!: This command is not to be used with GET type commands, only SET commands are viable.*/
  
  Serial.println("Setting amplitude limit");
  send_command_settings(set_amplitude_limit);
  Serial.println("Setting amplitude limit: DONE!");

  Serial.println("Setting median filter"); // Two options, disable_median_filter and enable_median_filter
  send_command_settings(enable_median_filter);
  Serial.println("Setting median filter: DONE!");
}
// define the 16 bit distance data array NOTE!: This is no longer needed if we can just write the data straight to display buffer.
uint16_t pixelarray[160][60];

void loop() {
  uint8_t ret[SINGLE_PIC_SIZE];
  CAMSERIAL.write(single_frame, 14);
  for (int i = 0; i < SINGLE_PIC_SIZE; i++) {
    for (; !CAMSERIAL.available(););
    ret[i] = CAMSERIAL.read();
  }
  int crc = calcCrc32_32(ret, SINGLE_PIC_SIZE - 4);

  /* Now the screen is operated using Teensy's own st7735_t3 library.
     It is way more efficient and better optimized than adafruit's one and now
     the screen works more smoothly.
  */
  tft.useFrameBuffer(true);   // Start the framebuffer, so we can place the camera data straight to the corresponding pixel.
  int camdata_hdr_size = 84;//pixel data starts
  data_buf = 0;// Buffer for camera pixel data.
  for (int j = 0; j < 60; j++) {
    for (int i = 0; i < 160; i++) {
      data_buf = ret[camdata_hdr_size + 1];
      data_buf <<= 8;
      data_buf |= ret[camdata_hdr_size];
      data_buf <<= 2;
      data_buf >>= 2;
      target_pixel = tft.readPixel(i, j); // Read the target pixel being handled, save for later use.
      if (data_buf == 16001) { //Case for low TOF amplitude.
        tft.drawPixel(i, j, 0xFFFF);
      }
      else if (data_buf == 16002) { //Case for A/D conversion limit exceeded.
        tft.drawPixel(i, j, 0xE7E0);
      }
      else if (data_buf == 16003) { //Case for pixel saturation.
        tft.drawPixel(i, j, 0x5FE0);
      }
      else if (data_buf == 16007) { //Case for motion blur.
        tft.drawPixel(i, j, 0xF81E);
      }
      else if (data_buf == 16008) { //Case for edge detection.
        tft.drawPixel(i, j, 0xFFFF);
      }
      else if (data_buf <= 7500) { //Case for basic distances.
          tft.drawPixel(i, j, colorlut[data_buf / 214]);
      }
      else {
        tft.drawPixel(i, j, 0x0000);
      }
      camdata_hdr_size += 2 ;
    }
  }

  /*Extra information printed on the screen*/
  tft.setCursor(2, 62);
  tft.setTextColor(0xFFFF, 0x0000);
  tft.setTextWrap(true);
  tft.print("Distance: ");
  tft.print(pixelarray[80][30]);
  tft.print(" mm");
  tft.setCursor(2, 71);
  tft.print(millis() / 10);

  
  tft.updateScreen();         //update screen
  tft.freeFrameBuffer();      //Free framebuffer
  //CRC checksum:
  Serial.println("CRC:");
  for (int i = SINGLE_PIC_SIZE - 4; i < SINGLE_PIC_SIZE; i++) {
    Serial.println(ret[i], HEX);
  }

  Serial.println(crc, HEX);
  Serial.print("Done!\n");

}
