#pragma once
#include "epc635_commands.h"

class Epc635{
public:
  begin(HardwareSerial *port) : port_(port){}
  uint8_t* chipInfo(){
    unsigned char chip_info[14] = {SND, GET_CHIP_INFORMATION,0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,0x94,0x8B,0x2E,0xD5};
    port_.write(chip_info), 12);
    uint8_t response[12];
    for(uint8_t i = 0; i<12; i++){
      for(;!CAMSERIAL.available(););
      response[i]=CAMSERIAL.read();
    }
    uint32_t crc = calcCrc32_32(response, 12);
    uint32_t compare;
    for(uint8_t i = 11; i >= 8; i--){
      compare |= response[i];
      compare <<= 8*i-8;
    }
    if(compare != crc){
      return NULL;
    }else{
      return response;
    }
  }
  
private:
  HardwareSerial* port_
  
};
