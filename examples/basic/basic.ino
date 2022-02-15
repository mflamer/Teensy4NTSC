
#include "Teensy4NTSC.h"


Teensy4NTSC ntsc;


void setup() {

	// Create NTSC display system and set pin selections

	// Teensy4NTSC(byte pinSync, byte pinPixels, int v_res = 256);

	// pins =  6|7|8|9|10|11|12|13|35|36|37|39
	// v_res = Display vertical resolution. Optimal value depends on display device. Max = 256.

   	ntsc = Teensy4NTSC(240);  
    ntsc.start();


   	ntsc.clear();
    for(int x = 0; x < ntsc.h_res; x++){
        for(int y = 0; y < ntsc.v_res; y++){
            ntsc.pixel(x, y, x & 0xF0);
        }
    }
   	// ntsc.pixel(3, ntsc.v_res-1, 0x00);
    // ntsc.pixel(ntsc.h_res-3, ntsc.v_res-1, 0x00);
   	ntsc.dump_buffer(); 

   	//pinMode(36, OUTPUT);
   	//digitalWrite(36, HIGH);

    // ntsc.line(ntsc.h_res/2, ntsc.v_res/2, ntsc.h_res-1, 0);
   	// ntsc.line(ntsc.h_res/2, ntsc.v_res/2, 0, ntsc.v_res-1);
   	// ntsc.line(ntsc.h_res/2, ntsc.v_res/2, 0, 0);
   	// ntsc.line(ntsc.h_res/2, ntsc.v_res/2, 200, ntsc.v_res-1);
   	// ntsc.line(ntsc.h_res/2, ntsc.v_res/2, 200, 0);
   	// ntsc.line(ntsc.h_res/2, ntsc.v_res/2, 120, ntsc.v_res-1);
   	// ntsc.line(ntsc.h_res/2, ntsc.v_res/2, 120, 0);
	
   	// ntsc.line(ntsc.h_res/2, 0, ntsc.h_res/2, ntsc.v_res-1);
   	// ntsc.line(0, ntsc.v_res/2, ntsc.h_res-1, ntsc.v_res/2);
   	// ntsc.line(0, 0, ntsc.h_res-1, ntsc.v_res-1);
   	// ntsc.line(0, ntsc.v_res-1, ntsc.h_res-1, 0);

   	// ntsc.rectangle(0, 0, ntsc.h_res-1, ntsc.v_res-1);
   	// ntsc.rectangle(0, 18, ntsc.h_res-1, ntsc.v_res-19);

   	// ntsc.rectangle(0, 0, 20, 20, true);
   	// ntsc.rectangle(5, 5, 15, 15, true, BLACK);

   	// ntsc.rectangle(ntsc.h_res-1, ntsc.v_res-1, ntsc.h_res-21, ntsc.v_res-21, true);
   	// ntsc.rectangle(ntsc.h_res-6, ntsc.v_res-6, ntsc.h_res-16, ntsc.v_res-16, true, BLACK);	
   	
   	// ntsc.circle(ntsc.h_res/2, ntsc.v_res/2, 20, true);
   	// ntsc.circle(ntsc.h_res/2, ntsc.v_res/2, 10, true, BLACK);
   	// ntsc.circle(ntsc.h_res/2, ntsc.v_res/2, 25);

   	// ntsc.text("Hello World! \x1 \x12", 4, 180);
   	// ntsc.text("This is NTSC from RAM -> DMA -> FlexIO", 4, 50, BLACK);
   	
}


// int cnt = 0;
// int y_ = 0;
// int x = 0;
// int y = 108;
// int t = 0;

int r = 0;
int x = 0;
int y = 108;
int t = 0;
byte fill = 0;
byte line = 0;
float k = 0;
int samples[512];
float intensity[512];


void loop() {

 //  ntsc.circle(x++ % (ntsc.h_res-1), y % (ntsc.v_res-1), 3, true, BLACK);	
	// y += y_;
	// ntsc.circle(x % (ntsc.h_res-1), y % (ntsc.v_res-1), 3, true, WHITE); 
	   
 //  // randomly set y delta
 //  if(cnt == 10){
	// 	y_ = random(-1, 2);
	// 	cnt = 0;
	// }
	// cnt++;  

 //  //fizz
 //  if(t == 40)
 //  {
 //  	int x = random(ntsc.h_res);
 //  	int y = random(ntsc.v_res);
 //  	int r = random(10);
 //  	bool fill = random(0, 2); 
 //  	bool color = random(0, 2);
 //    ntsc.circle(x, y, r, fill, color);
 //    t = 0;
 //  }

 //  t++; 

 //delay(15);
    
 //lumaBars
  // ntsc.clear();
  // for(byte x = 0; x < 16; x++)
  //   {
  //     ntsc.rectangle(x*(ntsc.h_res/15), 0, (x+1)*(ntsc.h_res/15), ntsc.v_res-1, x, x);
  //   }
  // ntsc.text("Luminance bars", 4, 15);
  // //ntsc.dump_buffer();

  // delay(2000);

  // //Zooming
  // ntsc.clear();
  // k = 0.1;

  // for (t = 0; t<100; t++)
  //   {
  //     for(x = 0; x < ntsc.h_res; x++)
  //       {
  //         for(y = 0; y < ntsc.v_res; y++)
  //         {
  //           fill = int(7.5 + 7.5*sin((x*y/(k*3.14*ntsc.v_res)))+0.5);
  //           ntsc.pixel(x, y, fill);
  //         }
  //       }
  //     k = k + 0.1;
  //     //ntsc.dump_buffer();
  //   }

  // for (t = 0; t<100; t++)
  //   {
  //     for(x = 0; x < ntsc.h_res; x++)
  //       {
  //         for(y = 0; y < ntsc.v_res; y++)
  //         {
  //           fill = int(7.5 + 7.5*sin((x*y/(k*3.14*ntsc.v_res)))+0.5);
  //           ntsc.pixel(x, y, fill);
  //         }
  //       }
  //     k = k - 0.1;
  //     //ntsc.dump_buffer();
  //   }

  // delay(1000);

  // //Zooming Sombrero
  // ntsc.clear();
  // k = 1.0;

  // for (t = 0; t<100; t++)
  //   {
  //     for(x = 0; x < ntsc.h_res; x++)
  //       {
  //         for(y = 0; y < ntsc.v_res; y++)
  //         {
  //           fill = int(7.5 + 7.5*cos(((x-ntsc.h_res/2)*(x-ntsc.h_res/2)+(y-ntsc.v_res/2)*(y-ntsc.v_res/2))/(k*3.14*ntsc.v_res))+0.5);
  //           ntsc.pixel(x, y, fill);
  //         }
  //       }
  //     k = k + 0.1;
  //     //ntsc.dump_buffer();
  //   }

  // delay(1000);

  // //Zooming Thingy
  // ntsc.clear();
  // k = 1.0;

  // for (t = 0; t<500; t++)
  //   {
  //     for(x = 0; x < ntsc.h_res; x++)
  //       {
  //         for(y = 0; y < ntsc.v_res; y++)
  //         {
  //           fill = int(7.5 + 7.5*cos(((x-ntsc.h_res/2)*(x-ntsc.h_res/2)*(y-ntsc.v_res/2)*(y-ntsc.v_res/2))/(k*3.14*ntsc.v_res))+0.5);
  //           ntsc.pixel(x, y, fill);
  //         }
  //       }
  //     k = k + 0.5;
  //     //ntsc.dump_buffer();
  //   }

  // delay(1000);
/*
 //grayFizz
 ntsc.clear();
 for (t = 0; t < 1000; t++)
   {
        x = random(ntsc.h_res);
      y = random(ntsc.v_res);
      r = random(10);
      fill = int(random(0, 15));
      line = int(random(0, 15));
      ntsc.circle(x, y, r, fill, line);
      ntsc.text("Fizz, fizz, fizz...", 4, 15);
      ntsc.dump_buffer();
    }

 delay(1000);

  //plotFade
  int fadeLength = 50; //number of frames over which to fade a trace to black

  ntsc.clear();

  for (int loopCount = 0; loopCount < 5; loopCount++)
  {
    k = random(0,64)/16.0;
    for (x = 0; x <= ntsc.h_res; x++)
        {
          samples[x] = int(ntsc.v_res/2 + (ntsc.v_res/2)*sin(k*6.28*x/ntsc.h_res)+0.5);
          intensity[x] = 15;
        }

    for (t = 0; t < fadeLength; t++)
      {
         for (x = 0; x < ntsc.h_res; x++)
            {
              intensity[x] = int(float(intensity[x])*(1.0-1/float(fadeLength)));
              ntsc.line(x, samples[x], x+1, samples[x+1], intensity[x]);
              //ntsc.pixel(x, samples[x], intensity[x]);
            }

          ntsc.text("Fading trace ", 4, 15);
          ntsc.dump_buffer();
          delay(50); //delay must be longer than than NTSC frame time
      }

  }

 */
  


}








