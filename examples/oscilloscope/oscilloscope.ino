//Super-simple (my fault) oscilloscope using Mark Flamer's FlexIO NTSC video for Teensy 4
//G. Kovacs, 12/26/21

#include "Teensy4NTSC.h"
#include <IntervalTimer.h>

Teensy4NTSC ntsc;
IntervalTimer timer;

volatile int x;

static const int H_RES = 320;
static const int V_RES = 216;
int xMax = H_RES - 1;
int yMax = V_RES - 1;


volatile byte readings[H_RES];

int analogResolution = 8;
uint8_t ADC_Max = 255;
uint8_t ADC_Center = ADC_Max >> 1;

void setup() {
    // Create object and set pin selections
    // pins = 6|7|8|9|10|11|12|13|35|36|37|39
    ntsc = Teensy4NTSC(6, 7, V_RES);  
    analogReadResolution(analogResolution);
    timer.begin(sample, 15); //Last variable is time between events in microseconds. In previous tests went well above 100kHz, but here maybe 67kSPS (15 us) max?
    //Testing done at default 600 MHz clock speed.
}

FASTRUN void loop() {
    //Stall until triggered
    //Super-crude trigger. Need hysteresis and slope control
    //while (analogRead(A0) < ADC_Center && analogRead(A0) < ADC_Center)
/*
    while (readings[0]<ADC_Center && readings[1]<ADC_Center && readings[2]<ADC_Center)
    {       
        ntsc.text("Not triggered", 4, 50, WHITE);
    }   
    //TODO - import good trigger code
*/
    // Erase previous trace
    ntsc.clear();

    // Plot new points
    for (int x = 0; x < xMax; x++)
    {
        int y0 = map(readings[x], 0, ADC_Max, 0, yMax);
        int y1 = map(readings[(x+1) % xMax], 0, ADC_Max, 0, yMax);
        ntsc.line(x, y0, x+1, y1);              
    }
  
    delay(10);
}

void sample(){  
    readings[x] = analogRead(A0);
    x = (x+1) % xMax; 
}

