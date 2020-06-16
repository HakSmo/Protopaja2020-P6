#include <uVGA.h>
uVGA uvga;

#define UVGA_DEFAULT_REZ
#include <uVGA_valid_settings_180MHz.h>


unsigned char tulos;
unsigned char testi;
void setup() {
  // put your setup code here, to run once:

  int ret;
  ret = uvga.begin(&modeline);

  Serial.println(ret);
  if(ret != 0){
    Serial.println("fatal error");
    while(1);
  }
  Serial.begin(9600);
  Serial1.begin(10000000);
  Serial2.begin(10000000);
  
}

void loop() {
  int fb_width, fb_height;
  int x,y;
  int my;
  uvga.get_frame_buffer_size(&fb_width, &fb_height);
  
  for(y=0; y<600; y++){
    for(x=0; x<800; x++){
      uvga.drawPixel(x,y, 0x8F);
    }
  }
  
  /*testi = 0xAB;
  // put your main code here, to run repeatedly:
  delay(1000);
  Serial1.write(testi);
  if(Serial2.available()){
    tulos = Serial2.read();
  }
  Serial.println(tulos);*/
  delay(100);
}
