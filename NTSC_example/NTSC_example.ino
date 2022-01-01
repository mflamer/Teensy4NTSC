
#include "Teensy4NTSC.h"


Teensy4NTSC ntsc;


void setup() {

	// Create object setting signal output pin selections
	// pinBlack = any digital pin
	// pinWhite = 6|7|8|9|10|11|12|13|35|36|37|39
   	ntsc = Teensy4NTSC(6, 7);   
   	ntsc.clear();

   	//ntsc.line(0, V_RES-1, H_RES-1, V_RES-1);
   	//ntsc.line(0, 1, 319, 1);
   	   
   	ntsc.line(H_RES/2, V_RES/2, H_RES-1, 0);
   	ntsc.line(H_RES/2, V_RES/2, 0, V_RES-1);
   	ntsc.line(H_RES/2, V_RES/2, 0, 0);
   	ntsc.line(H_RES/2, V_RES/2, 200, V_RES-1);
   	ntsc.line(H_RES/2, V_RES/2, 200, 0);
   	ntsc.line(H_RES/2, V_RES/2, 120, V_RES-1);
   	ntsc.line(H_RES/2, V_RES/2, 120, 0);
	
   	ntsc.line(H_RES/2, 0, H_RES/2, V_RES-1);
   	ntsc.line(0, V_RES/2, H_RES-1, V_RES/2);
   	ntsc.line(0, 0, H_RES-1, V_RES-1);
   	ntsc.line(0, V_RES-1, H_RES-1, 0);

   	ntsc.rectangle(0, 0, H_RES-1, V_RES-1);
   	ntsc.rectangle(0, 0, 20, 20, true);
   	ntsc.rectangle(5, 5, 15, 15, true, true);
   
   	ntsc.rectangle(H_RES-1, V_RES-1, H_RES-21, V_RES-21, true);
   	ntsc.rectangle(H_RES-6, V_RES-6, H_RES-16, V_RES-16, true, true);	
   	
   	ntsc.circle(H_RES/2, V_RES/2, 20, true);
   	ntsc.circle(H_RES/2, V_RES/2, 10, true, true);
   	ntsc.circle(H_RES/2, V_RES/2, 25);

   	ntsc.text("Hi Greg! \x1 \x12", 4, 180);
   	ntsc.text("This is a test", 4, 50, true);

   	//ntsc.dump_buffer();



}






int cnt = 0;
int y_ = 0;
int x = 0;
int y = 108;


void loop() {
	
  	delay(30);  	
	ntsc.circle(x++ % (H_RES-1), y % (V_RES-1), 2, true, true);	
	y += y_;
	ntsc.circle(x % (H_RES-1), y % (V_RES-1), 2, true); 
	   
    // randomly set y delta
    if(cnt == 10){
		y_ = random(-1, 2);
		cnt = 0;
	}
	cnt++;  
   
}


