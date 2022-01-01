
#include "Teensy4NTSC.h"


Teensy4NTSC ntsc;


void setup() {

	// Create object setting signal output pin selections
	// pinBlack = any digital pin
	// pinWhite = 6|7|8|9|10|11|12|13|35|36|37|39
   	ntsc = Teensy4NTSC(6, 7);   
   	ntsc.clear();
   	   
   	//ntsc.line(0, 0, 31, 0);
   	//ntsc.line(0, 239, 319, 239);
   	// ntsc.line(128, 108, 255, 0);
   	// ntsc.line(128, 108, 0, 215);
   	// ntsc.line(128, 108, 0, 0);
   	// ntsc.line(128, 108, 148, 215);
   	// ntsc.line(128, 108, 148, 0);
   	// ntsc.line(128, 108, 108, 215);
   	// ntsc.line(128, 108, 108, 0);
	
   	// ntsc.line(128, 108, 128, 215);
   	// ntsc.line(128, 108, 128, 0);
   	// ntsc.line(128, 108, 0, 108);
   	// ntsc.line(128, 108, 255, 108);
	
   	ntsc.rectangle(0, 1, H_RES-1, V_RES-20);
   	// ntsc.rectangle(0, 0, 20, 20, true);
   	// ntsc.rectangle(5, 5, 15, 15, true, true);
   	//ntsc.rectangle(0, 0, 31, 239, true);
   	// ntsc.rectangle(250, 210, 240, 200, true, true);
	
   	
   	// ntsc.circle(128, 108, 20, true);
   	// ntsc.circle(128, 108, 10, true, true);
   	// ntsc.circle(128, 108, 25);

   	// ntsc.text("Hi Greg! \x1 \x12", 4, 180);
   	// ntsc.text("This is a test", 4, 50, true);

}






int cnt = 0;
int y_ = 0;
int x = 0;
int y = 108;


void loop() {
	
 //  	delay(30);  	
	// ntsc.circle(x++ & 0xFF, y & 0xFF, 2, true, true);	
	// y += y_;
	// ntsc.circle(x & 0xFF, y & 0xFF, 2, true); 
	   
 //    // randomly set y delta
 //    if(cnt == 10){
	// 	y_ = random(-1, 2);
	// 	cnt = 0;
	// }
	// cnt++;  
   
}


