
#include "Teensy4NTSC.h"


Teensy4NTSC ntsc;


void setup() {

	// Create NTSC display system and set pin selections

	// Teensy4NTSC(byte pinSync, byte pinPixels, int v_res = 256);

	// pins =  6|7|8|9|10|11|12|13|35|36|37|39
	// v_res = Display vertical resolution. Optimal value depends on display device. Max = 256.

   	ntsc = Teensy4NTSC(6, 7, 216);   
   	ntsc.clear();

    ntsc.line(ntsc.h_res/2, ntsc.v_res/2, ntsc.h_res-1, 0);
   	ntsc.line(ntsc.h_res/2, ntsc.v_res/2, 0, ntsc.v_res-1);
   	ntsc.line(ntsc.h_res/2, ntsc.v_res/2, 0, 0);
   	ntsc.line(ntsc.h_res/2, ntsc.v_res/2, 200, ntsc.v_res-1);
   	ntsc.line(ntsc.h_res/2, ntsc.v_res/2, 200, 0);
   	ntsc.line(ntsc.h_res/2, ntsc.v_res/2, 120, ntsc.v_res-1);
   	ntsc.line(ntsc.h_res/2, ntsc.v_res/2, 120, 0);
	
   	ntsc.line(ntsc.h_res/2, 0, ntsc.h_res/2, ntsc.v_res-1);
   	ntsc.line(0, ntsc.v_res/2, ntsc.h_res-1, ntsc.v_res/2);
   	ntsc.line(0, 0, ntsc.h_res-1, ntsc.v_res-1);
   	ntsc.line(0, ntsc.v_res-1, ntsc.h_res-1, 0);

   	ntsc.rectangle(0, 0, ntsc.h_res-1, ntsc.v_res-1);
   	ntsc.rectangle(0, 18, ntsc.h_res-1, ntsc.v_res-19);

   	ntsc.rectangle(0, 0, 20, 20, true);
   	ntsc.rectangle(5, 5, 15, 15, true, BLACK);

   	ntsc.rectangle(ntsc.h_res-1, ntsc.v_res-1, ntsc.h_res-21, ntsc.v_res-21, true);
   	ntsc.rectangle(ntsc.h_res-6, ntsc.v_res-6, ntsc.h_res-16, ntsc.v_res-16, true, BLACK);	
   	
   	ntsc.circle(ntsc.h_res/2, ntsc.v_res/2, 20, true);
   	ntsc.circle(ntsc.h_res/2, ntsc.v_res/2, 10, true, BLACK);
   	ntsc.circle(ntsc.h_res/2, ntsc.v_res/2, 25);

   	ntsc.text("Hello World! \x1 \x12", 4, 180);
   	ntsc.text("This is NTSC from RAM -> DMA -> FlexIO", 4, 50, BLACK);
   	
}


int cnt = 0;
int y_ = 0;
int x = 0;
int y = 108;
int t = 0;


void loop() {

  ntsc.circle(x++ % (ntsc.h_res-1), y % (ntsc.v_res-1), 3, true, BLACK);	
	y += y_;
	ntsc.circle(x % (ntsc.h_res-1), y % (ntsc.v_res-1), 3, true, WHITE); 
	   
  // randomly set y delta
  if(cnt == 10){
		y_ = random(-1, 2);
		cnt = 0;
	}
	cnt++;  

  //fizz
  if(t == 40)
  {
  	int x = random(ntsc.h_res);
  	int y = random(ntsc.v_res);
  	int r = random(10);
  	bool fill = random(0, 2); 
  	bool color = random(0, 2);
    ntsc.circle(x, y, r, fill, color);
    t = 0;
  }

  t++; 

  delay(15);

}








