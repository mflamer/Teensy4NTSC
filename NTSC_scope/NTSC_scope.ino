//Super-simple (my fault) oscilloscope using Mark Flamer's FlexIO NTSC video for Teensy 4
//G. Kovacs, 12/26/21

#include "Teensy4NTSC.h"
#include <IntervalTimer.h>

Teensy4NTSC ntsc;
IntervalTimer timer;

int xMax = H_RES - 1;
int yMax = V_RES - 1;

volatile byte readings[H_RES];

int analogResolution = 8;
int ADC_Max = 255;
int ADC_Center = ADC_Max >> 1;

void setup() {
    // Create object and set pin selections
    // pins = 6|7|8|9|10|11|12|13|35|36|37|39
    ntsc = Teensy4NTSC(6, 7);  
    analogReadResolution(analogResolution);
    timer.begin(sample, 80); // I'm not sure how fast the ADC is. If I set the interrupts freq much faster it hangs.....
}

FASTRUN void loop() {
    //Stall until triggered
    noInterrupts();
    while (analogRead(A0) < ADC_Center && analogRead(A0) < ADC_Center && analogRead(A0) < ADC_Center) //Super-crude trigger. Need hysteresis and slope control
    {       
        ntsc.text("Not triggered", 4, 50, WHITE);
    }    

    // Erase previous trace
    ntsc.clear();

    // Plot new points
    for (int x = 0; x < xMax; x++)
    {
        ntsc.pixel(x, readings[x], WHITE);        
    }
    interrupts();
  
    delay(20);
}



void sample(){
    static int x = 0;    
    readings[x] = map(analogRead(A0), 0, ADC_Max, 0, yMax);
    x = x == xMax ? 0 : x + 1;    
}
