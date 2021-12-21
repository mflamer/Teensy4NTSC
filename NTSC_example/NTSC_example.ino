
#include "Teensy4NTSC.h"


Teensy4NTSC ntsc;




// int line = 0;


void setup() {

   ntsc = Teensy4NTSC(3, 10);   
   //ntsc.clear(0x80000000);
      
   ntsc.line(128, 108, 255, 215);
   ntsc.line(128, 108, 255, 0);
   ntsc.line(128, 108, 0, 215);
   ntsc.line(128, 108, 0, 0);
   ntsc.line(128, 108, 148, 215);
   ntsc.line(128, 108, 148, 0);
   ntsc.line(128, 108, 108, 215);
   ntsc.line(128, 108, 108, 0);

   ntsc.line(128, 108, 128, 215);
   ntsc.line(128, 108, 128, 0);
   ntsc.line(128, 108, 0, 108);
   ntsc.line(128, 108, 255, 108);

   //ntsc.rectangle(0, 0, 255, 215);
   ntsc.rectangle(0, 0, 20, 20, true);
   ntsc.rectangle(5, 5, 15, 15, true, true);
   ntsc.rectangle(255, 215, 235, 195, true);
   ntsc.rectangle(250, 210, 240, 200, true, true);

   
   
}











void loop() {
  

 
   
     
   
   
}


