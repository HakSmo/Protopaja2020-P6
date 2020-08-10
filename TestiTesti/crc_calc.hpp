#pragma once
#include <stdint.h>
#include <vector>
#define CAMSERIAL Serial1
uint32_t calcCrc32Uint32(uint32_t crc, uint32_t data){
  int32_t i;  
  crc = crc ^ data;  
  for(i=0; i<32; i++){
    if (crc & 0x80000000){
      crc = (crc << 1) ^ 0x04C11DB7;    
    }else    {
        crc = (crc << 1);    
    }
  }  
  return(crc);
}

uint32_t calcCrc32_32(const uint8_t *data, const uint32_t size){
  uint32_t crc = 0xFFFFFFFF;  
  for(uint32_t i = 0; i < size; i++){  
    crc = calcCrc32Uint32(crc, data[i]); 
  }  
  return crc ^ 0x00000000;
}

void send_command_settings(unsigned char *cam_command){
  Serial.println("Printing the command!"); // Print the received command
  for(unsigned int i = 0; i < 14; i++){
    Serial.print(cam_command[i], HEX);
    Serial.print(" ");
    }
  Serial.println("\nCalculating crc for cam_command"); // Calculating the crc for the command.
  uint32_t crcmed = calcCrc32_32(cam_command, 10);
  Serial.println("The crc is:");
  Serial.println(crcmed, HEX);
  Serial.println("Vectorifying the crc"); // Modify the uint32_t type crc into a vector of 4 unsigned char elements in the correct order.
  unsigned char crc_vect[4] = {
  (crcmed >> 0) & 0xFF,
  (crcmed >> 8) & 0xFF,
  (crcmed >> 16) & 0xFF,
  (crcmed >> 24) & 0xFF
  };
  Serial.println("Print the new crc vector."); // Print the new crc vector.
  for(unsigned int i = 0; i < 4; i++){
    Serial.println(crc_vect[i], HEX);
    }
  Serial.println("Applying the crc to command and print the results"); // Apply the crc vector to the 4 last elements of the command.
  for(unsigned int i = 0; i < 4; i++){
    cam_command[10+i] = crc_vect[i];
    }
  for(unsigned int i = 0; i < 14; i++){
    Serial.print(cam_command[i], HEX);
    Serial.print(" ");
    }
    
  Serial.println("\nSending the command"); // Sending the command
  CAMSERIAL.write(cam_command, 14);
  Serial.println("Waiting on response...");
  
  char incoming_response[8]; // Initialize all the structures needed in handling the response and checking its validity.
  char incoming_byte = 0xFF;
  while(incoming_byte == 0xFF){ // We wait for the cameras response. This is still WIP as it can get stuck into an infinite loop.
    incoming_byte = CAMSERIAL.read();
    };
  incoming_response[0] = incoming_byte; // Once we get a response, immediately save the new non 0xFF byte into the first slot of the response.
  Serial.print(incoming_response[0],HEX);
  Serial.print(" ");
  for (int i = 1; i < 8; i++) { // Read rest of the response.
    incoming_response[i] = CAMSERIAL.read();
    Serial.print(incoming_response[i],HEX);
    Serial.print(" ");
  }
  // Following structures will check whether the response was ACK, NACK or unidentifiable. At the end we return to main loop.
  if(incoming_response[0] == CAM_ACK[0] && incoming_response[1] == CAM_ACK[1]){ 
    for (int i = 0; i < 8; i++) {
      if(incoming_response[i] != CAM_ACK[i]){
        Serial.println("Response error!");
        break;
        }
      }
    Serial.println("Command acknowledged!");
    return;
    }
   else if(incoming_response[0] == CAM_NACK[0] && incoming_response[1] == CAM_NACK[1]){
    for (int i = 0; i < 8; i++) {
      if(incoming_response[i] != CAM_NACK[i]){
        Serial.println("Response error!");
        break;
        }
      }
    Serial.println("Command not acknowledged!");
    return;
    }
  else{
    Serial.println("Response not identified");
    return;
    }
  }
