
#include "Teensy4NTSC.h"

// Global instance of NTSC system
Teensy4NTSC ntsc;


void setup() {

	// Start the system and begin sending the NTSC signal. 
    // v_res = The desired display vertical resolution. Optimal value depends on display device. Max = 256.
    // Note: h_res is fixed at 320.
    ntsc.begin(256);
    ntsc.clear();

    // for(int y = 0; y < ntsc.v_res; y++){
    //     for(int x = 0; x < ntsc.h_res; x++){
    //         ntsc.pixel(x, y, x & 0xFF);
    //     }
    // }  	
    
    ntsc.circle(100, 100, 20, 0xFF);

   	ntsc.rectangle(0, 0, ntsc.h_res-1, ntsc.v_res-1, 0xFF, false);

    ntsc.dump_buffer(); 
   	
}

int r = 0;
int x = 0;
int y = 108;
int t = 0;
byte luma = 0;
bool filled = 0;
float k = 0;
int samples[512];
float intensity[512];


void loop() {

     
  //lumaBars
  ntsc.clear();
  for(byte x = 0; x < 16; x++)
    {
      ntsc.rectangle(x*(ntsc.h_res/15), 0, (x+1)*(ntsc.h_res/15), ntsc.v_res-1, x, x);
    }
  ntsc.text("Luminance bars", 4, 15);
  

  delay(2000);

  //Zooming
  ntsc.clear();
  k = 0.1;

  for (t = 0; t<100; t++)
    {
      for(x = 0; x < ntsc.h_res; x++)
        {
          for(y = 0; y < ntsc.v_res; y++)
          {
            luma = int(7.5 + 7.5*sin((x*y/(k*3.14*ntsc.v_res)))+0.5);
            ntsc.pixel(x, y, luma);
          }
        }
      k = k + 0.1;
      
    }

  for (t = 0; t<100; t++)
    {
      for(x = 0; x < ntsc.h_res; x++)
        {
          for(y = 0; y < ntsc.v_res; y++)
          {
            luma = int(7.5 + 7.5*sin((x*y/(k*3.14*ntsc.v_res)))+0.5);
            ntsc.pixel(x, y, luma);
          }
        }
      k = k - 0.1;
      
    }

  delay(1000);

  //Zooming Sombrero
  ntsc.clear();
  k = 1.0;

  for (t = 0; t<100; t++)
    {
      for(x = 0; x < ntsc.h_res; x++)
        {
          for(y = 0; y < ntsc.v_res; y++)
          {
            luma = int(7.5 + 7.5*cos(((x-ntsc.h_res/2)*(x-ntsc.h_res/2)+(y-ntsc.v_res/2)*(y-ntsc.v_res/2))/(k*3.14*ntsc.v_res))+0.5);
            ntsc.pixel(x, y, luma);
          }
        }
      k = k + 0.1;
     
    }

  delay(1000);

  //Zooming Thingy
  ntsc.clear();
  k = 1.0;

    for (t = 0; t<500; t++)
      {
        for(x = 0; x < ntsc.h_res; x++)
          {
            for(y = 0; y < ntsc.v_res; y++)
            {
              luma = int(7.5 + 7.5*cos(((x-ntsc.h_res/2)*(x-ntsc.h_res/2)*(y-ntsc.v_res/2)*(y-ntsc.v_res/2))/(k*3.14*ntsc.v_res))+0.5);
              ntsc.pixel(x, y, luma);
            }
          }
        k = k + 0.5;
       
      }
    delay(1000);

    //grayFizz
    ntsc.clear();
    ntsc.text("Fizz, fizz, fizz...", 4, 15);
    for (t = 0; t < 1000; t++)
    {
        x = random(ntsc.h_res);
        y = random(ntsc.v_res);
        r = random(10);
        luma = int(random(15));
        filled = bool(random(2));
        ntsc.circle(x, y, r, luma, filled);
        delay(5);
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
        delay(50); //delay must be longer than than NTSC frame time
    }

  }


  
}








