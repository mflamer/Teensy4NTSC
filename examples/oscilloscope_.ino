//Super-simple (my fault) oscilloscope using Mark Flamer's FlexIO NTSC video for Teensy 4
//G. Kovacs, 12/26/21

#include "Teensy4NTSC.h"
#include <IntervalTimer.h>

#define MAX(a,b) (((a)>(b))?(a):(b))

#define FADE 0x10

Teensy4NTSC ntsc;
IntervalTimer timer;

volatile int x;

static const int H_RES = 320;
static const int V_RES = 240;
int xMax = H_RES - 1;
int yMax = V_RES - 1;


byte readings[H_RES];
byte buffer[V_RES][H_RES];

int analogResolution = 8;
uint8_t ADC_Max = 255;
uint8_t ADC_Center = ADC_Max >> 1;

void setup() {

    // Start the system and begin sending the NTSC signal. 
    // v_res = The desired display vertical resolution. Optimal value depends on display device. Max = 256.
    // Note: h_res is fixed at 320.
    ntsc.begin(V_RES);
    
    analogReadResolution(analogResolution);
    //timer.begin(sample, 15); //Last variable is time between events in microseconds. In previous tests went well above 100kHz, but here maybe 67kSPS (15 us) max?
    //Testing done at default 600 MHz clock speed.
}


// temp hack 
void line(int x0, int y0, int x1, int y1)
{
    int dx =  abs(x1-x0);
    int sx = x0<x1 ? 1 : -1;
    int dy = -abs(y1-y0);
    int sy = y0<y1 ? 1 : -1;
    int err = dx+dy;  /* error value e_xy */
    while (true){  /* loop */
        buffer[y0][x0] = 0xFF; 
        if (x0 == x1 && y0 == y1) break;
        int e2 = 2 * err;
        if (e2 >= dy){/* e_xy+e_x > 0 */
            err += dy;
            x0 += sx;
        }
        if (e2 <= dx){ /* e_xy+e_y < 0 */
            err += dx;
            y0 += sy;
        }
    }    
}

FASTRUN void loop() {
    //Stall until triggered
    //Super-crude trigger. Need hysteresis and slope control
    //while (analogRead(A0) < ADC_Center && analogRead(A0) < ADC_Center)

    while(analogRead(A0) < ADC_Center && analogRead(A0) < ADC_Center)
    {       
        ntsc.text("Not triggered", 4, 50);
    }   
    //TODO - import good trigger code


    // sample next set
    for(int x = 0; x < H_RES; x++){
        readings[x] = map(analogRead(A0), 0, ADC_Max, 0, yMax);
    }

    // fade buffer
    for (int j = 0; j < V_RES; j++) {
        for (int i = 0; i < H_RES; i++) {
            buffer[j][i] = MAX(0, buffer[j][i] - FADE); 
        }
    }

    // Plot new points
    for (int x = 0; x < xMax; x++)
    {
        //buffer[readings[x]][x] = 0xFF;  
        line(x, readings[x], x+1, readings[x+1]);            
    }


    //copy to display buffer
    ntsc.copy(&buffer[0][0]);    
  
    delay(50);
}

void sample(){  
    readings[x] = analogRead(A0);
    x = (x+1) % xMax; 
}

