
/* WIRING:
 *  
 *  T3.6      | DEVICE  PIN
 *  -----------------------------
 *  0 (RX1)   | CAMERA  6 (TX)
 *  1 (TX1)   | CAMERA  7 (RX)
 *  8         | DISPLAY 9 (DC)
 *  9         | DISPLAY 8 (RST)
 *  10(CS0)   | DISPLAY 7 (TFTCS)
 *  11(MOSI0) | DISPLAY 6 (MOSI)
 *  13(SCK0)  | DISPLAY 4 (SCK)
 *  
 */


#include "epc635_commands.h"
#include "crc_calc.hpp"
#include <SPI.h>
#include <ST7735_t3.h>
#include "colorLut.h"
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
unsigned char chip_info[14] =     {SND, GET_CHIP_INFORMATION,0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,0x94,0x8B,0x2E,0xD5};
unsigned char test_response[4] =  {0xFA, 0x03, 0x50, 0x4B};
unsigned char disable_median_filter[14] = {SND, SET_MEDIAN_FILTER, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xB7, 0xE8, 0xAA, 0x4B};

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

  //int crcmed = calcCrc32_32(median_filter, 10);
  //Serial.println(crcmed, HEX);
  /*Serial.println("Enable median filter:");
  
  CAMSERIAL.write(median_filter, 14);
  for(int i = 0; i<8; i++){
    for(;!CAMSERIAL.available(););
    if(CAMSERIAL.read() == 0x00 && i == 2){
      Serial.println("Command acknowledged");
      break;
    }else{
      Serial.println("Command not acknowledged");
      break;
    }
  }*/
  
}
// define the 16 bit distance data array
uint16_t pixelarray[160][60];

void loop() {
  /*for(unsigned int i = 0; i < 4; i++){
    digitalWrite(ledPin, HIGH);   // set the LED on
    delay(150);                  // wait for a second
    digitalWrite(ledPin, LOW);    // set the LED off
    delay(150);
  }*/
  uint8_t ret[SINGLE_PIC_SIZE];
  
  /*int a_bytes = CAMSERIAL.availableForWrite();
  
  Serial.print("Can print: ");
  Serial.print(a_bytes);
  Serial.print(" bytes\n");
  Serial.flush();*/
  CAMSERIAL.write(single_frame, 14);
  for(int i = 0; i<SINGLE_PIC_SIZE; i++){
    for(;!CAMSERIAL.available(););
    ret[i]=CAMSERIAL.read();
  }
  int crc = calcCrc32_32(ret, SINGLE_PIC_SIZE-4);
  /*for(int i = 0; i<4; i++){
    Serial.println(ret[i],HEX);
  }*/
  
 /* for(int i = 4; i< 84; i++){
    Serial.print("byteindex: ");
    Serial.print(i-4);
    Serial.print(" ");
    Serial.print(ret[i], HEX);
    Serial.print(", ");
    Serial.println(ret[i]);
  }*/
  int k = 84;//pixel data starts
  uint16_t buf = 0;//buffer
  for(int j = 0; j<60;j++){
    for(int i = 0; i<160; i++){
      buf = ret[k+1];
      buf <<= 8;
      buf |= ret[k];
      buf <<=2;
      buf >>=2;
      pixelarray[i][j] = buf;
      k +=2 ;
    }
  }
  /* Now the screen is operated using Teensy's own st7735_t3 library.
   * It is way more efficient and better optimized than adafruit's one and now
   * the screen works more smoothly.
   */
  
  tft.useFrameBuffer(true);   // starts framebuffer
  /*for(int j = 0; j<60; j++){
    for(int i = 1; i<160; i++){
      if(pixelarray[i][j]==16008){
        if(tft.readPixel(i,j) != 0xFFFF){
          tft.drawPixel(i,j, 0xFFFF);
        }
      }else if(pixelarray[i][j] == 16002){
        if(tft.readPixel(i,j) != 0xFFFF){
          tft.drawPixel(i,j, 0xFFFF);
        }
      }else if(pixelarray[i][j]>7500){
        if(tft.readPixel(i,j) != 0x0000){
          tft.drawPixel(i,j, 0x0000);
        }
      }else if(pixelarray[i][j]>3000){
        if(tft.readPixel(i,j) != 0xA800){
          tft.drawPixel(i,j, 0xA800);
        }
      }else if(pixelarray[i][j]>2000){
        if(tft.readPixel(i,j) != 0xF800){
          tft.drawPixel(i,j, 0xF800);
        }
      }else if(pixelarray[i][j]>1500){
        if(tft.readPixel(i,j) != 0xFC60){
          tft.drawPixel(i,j, 0xFC60);
        }
      }else if(pixelarray[i][j]>1100){
        if(tft.readPixel(i,j) != 0xFF80){
          tft.drawPixel(i,j, 0xFF80);
        }
      }else if(pixelarray[i][j]>800){
        if(tft.readPixel(i,j) != 0x07E0){
          tft.drawPixel(i,j, 0x07E0);
        }
      }else if(pixelarray[i][j]>500){
        if(tft.readPixel(i,j) != 0x07DF){
          tft.drawPixel(i,j, 0x07DF);
        }
      }else if(pixelarray[i][j]<=500){
        if(tft.readPixel(i,j) != 0x007F){
          tft.drawPixel(i,j, 0x007F);
        }
      }else{
        tft.drawPixel(i,j, 0x0000);
      }
    }
  }*/
  
  // Not tested. It basically calls the 35 member lookup table from colorLut.h
  for(int j = 0; j<60; j++){
    for(int i = 0; i<160; i++){
      
      if(pixelarray[i][j]==16008){
        if(tft.readPixel(i,j) != 0xFFFF){
          tft.drawPixel(i,j, 0xFFFF);
        }
      }else if(pixelarray[i][j] == 16002){
        if(tft.readPixel(i,j) != 0xFFFF){
          tft.drawPixel(i,j, 0xFFFF);
        }
      }else if(pixelarrray[i][j]<=7500){
        if(tft.readPixel(i,j) != cololut[(int)pixelarray[i][j]/214]){
          tft.drawPixel(i,j, cololut[(int)pixelarray[i][j]/214]);
        }
      }else{
        tft.drawPixel(i,j, 0x0000);
      }
      
    }
  }
  
  for(int j = 0; j<60; j++){
    for(int i = 0; i<160; i++){
      if(pixelarray[i][j]==16008){
        tft.drawPixel(i,j, 0xFFFF);
      }else if(pixelarray[i][j] == 16002){
        tft.drawPixel(i,j, 0xFFFF);
      }else if(pixelarray[i][j]>7500){
        tft.drawPixel(i,j, 0x0000);
      }else if(pixelarray[i][j]>3000){
        tft.drawPixel(i,j, 0xA800);
      }else if(pixelarray[i][j]>2000){
        tft.drawPixel(i,j, 0xF800);
      }else if(pixelarray[i][j]>1500){
        tft.drawPixel(i,j, 0xFC60);
      }else if(pixelarray[i][j]>1100){
        tft.drawPixel(i,j, 0xFF80);
      }else if(pixelarray[i][j]>800){
        tft.drawPixel(i,j, 0x07E0);
      }else if(pixelarray[i][j]>500){
        tft.drawPixel(i,j, 0x07DF);
      }else if(pixelarray[i][j]<=500){
        tft.drawPixel(i,j, 0x007F);
      }else{
        tft.drawPixel(i,j, 0x0000);
      }
    }
  }
  
  tft.setCursor(2, 62);
  tft.setTextColor(0xFFFF, 0x0000);
  tft.setTextWrap(true);
  tft.print("Distance: ");
  tft.print(pixelarray[80][30]);
  tft.print(" mm");
  tft.setCursor(2, 71);
  tft.print(millis()/10);
  // I don't want to print anything else to serial since it slows things down
 /* for(int i = 0; i<60;i++){
    for(int j = 0; j<160;j++){
       print(pixelarray[j][i])
    }
    println("");
  }*/
  tft.updateScreen();         //update screen
  tft.freeFrameBuffer();      //Free framebuffer
  //CRC checksum:
  Serial.println("CRC:");
  for(int i = SINGLE_PIC_SIZE-4; i<SINGLE_PIC_SIZE; i++){
    Serial.println(ret[i], HEX);
  }

  Serial.println(crc,HEX);
  Serial.print("Done!\n");

}
