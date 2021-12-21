#include "Teensy4NTSC.h"
#include <Arduino.h>
#include "imxrt.h"

TeensyTimerTool::PeriodicTimer Teensy4NTSC::timer = TeensyTimerTool::PeriodicTimer(TeensyTimerTool::GPT1);
int Teensy4NTSC::buffer[V_RES][H_WORDS] = {0}; 

Teensy4NTSC::Teensy4NTSC(byte pinBlack, byte pinWhite){
	// Setup black pin
	pinMode(3, OUTPUT);
   	digitalWriteFast(3, HIGH);  	

   	//start timer to interrupt at each line  	
   	timer.begin(sendLine, 63.5);
	
   	// Setup FlexIO (white pin)
   	// Fast FlexIO CLK  
   	CCM_CS1CDR &= ~( CCM_CS1CDR_FLEXIO2_CLK_PODF( 7 ) );
   	CCM_CS1CDR |= CCM_CS1CDR_FLEXIO2_CLK_PODF( 1 );
   	// Enable CLK
   	CCM_CCGR3 |= CCM_CCGR3_FLEXIO2(CCM_CCGR_ON);
   	// Enable FlexIO module
   	FLEXIO2_CTRL |= 1;   
   	// Pad MUX 
   	IOMUXC_SW_MUX_CTL_PAD_GPIO_B0_00 = 4;
   	// Shifter to pin
   	FLEXIO2_SHIFTCFG0 = 0x00000120; // set stop bit 0 otherwise line stays high
   	FLEXIO2_SHIFTCTL0 = 0x00030002;
   	// Shift counter
   	FLEXIO2_TIMCMP0 = 0x000001FF; // (H_RES * 2) - 1 
   	FLEXIO2_TIMCTL0 = 0x07400003;
   	FLEXIO2_TIMCFG0 = 0x00302630;
   	// Baud timer
   	FLEXIO2_TIMCMP1 = 0x00000009; // (PixelRateDiv / 2) - 1
   	FLEXIO2_TIMCTL1 = 0x01C00003;
   	FLEXIO2_TIMCFG1 = 0x00001200;
   	// Shifter chain
   	FLEXIO2_SHIFTCFG1 = 0x00000100;
   	FLEXIO2_SHIFTCFG2 = 0x00000100;
   	FLEXIO2_SHIFTCFG3 = 0x00000100;
   	FLEXIO2_SHIFTCFG4 = 0x00000100;
   	FLEXIO2_SHIFTCFG5 = 0x00000100;
   	FLEXIO2_SHIFTCFG6 = 0x00000100;
   	FLEXIO2_SHIFTCFG7 = 0x00000100;
   	FLEXIO2_SHIFTCTL1 = 0x00000002;
   	FLEXIO2_SHIFTCTL2 = 0x00000002;
   	FLEXIO2_SHIFTCTL3 = 0x00000002;
   	FLEXIO2_SHIFTCTL4 = 0x00000002;
   	FLEXIO2_SHIFTCTL5 = 0x00000002;
   	FLEXIO2_SHIFTCTL6 = 0x00000002;
   	FLEXIO2_SHIFTCTL7 = 0x00000002;
   	
}

void Teensy4NTSC::sendLine(){
	static int line = 0;
	if((line < (V_RES + V_START)) && (line >= V_START)){
      digitalWriteFast(3, LOW);
      delayMicroseconds(H_SYNC);
      digitalWriteFast(3, HIGH);
      delayMicroseconds(H_BACK);       
      FLEXIO2_SHIFTBUFBIS1 = buffer[line - V_START][1];
      FLEXIO2_SHIFTBUFBIS2 = buffer[line - V_START][2];
      FLEXIO2_SHIFTBUFBIS3 = buffer[line - V_START][3];
      FLEXIO2_SHIFTBUFBIS4 = buffer[line - V_START][4];      
      FLEXIO2_SHIFTBUFBIS5 = buffer[line - V_START][5];
      FLEXIO2_SHIFTBUFBIS6 = buffer[line - V_START][6];      
      FLEXIO2_SHIFTBUFBIS7 = buffer[line - V_START][7];
      FLEXIO2_SHIFTBUFBIS0 = buffer[line - V_START][0];             
   }
   else if(line == V_SYNC){        

      digitalWriteFast(3, LOW); 
      delayMicroseconds(58.86);
      digitalWriteFast(3, HIGH); 
   }
   else{
      digitalWriteFast(3, LOW);
      delayMicroseconds(H_SYNC);
      digitalWriteFast(3, HIGH);
      delayMicroseconds(H_BACK);       
      FLEXIO2_SHIFTBUFBIS1 = 0;
      FLEXIO2_SHIFTBUFBIS2 = 0;
      FLEXIO2_SHIFTBUFBIS3 = 0;
      FLEXIO2_SHIFTBUFBIS4 = 0;
      FLEXIO2_SHIFTBUFBIS5 = 0;
      FLEXIO2_SHIFTBUFBIS6 = 0;
      FLEXIO2_SHIFTBUFBIS7 = 0;
      FLEXIO2_SHIFTBUFBIS0 = 0;   
   }

   line++;
   if(line == V_TOTAL_LINES) line = 0;
  
}

void Teensy4NTSC::order(int* v0, int* v1){
	if(*v0 > *v1){
		int t = *v0;
		*v0 = *v1;
		*v1 = t; 
	}
}


void Teensy4NTSC::clear(int v){
	for (int j = 0; j < V_RES; j++) {
      	for (int i = 0; i < H_WORDS; i++) {
           	buffer[j][i] = v;
      	}
   	}
}



void Teensy4NTSC::pixel(int x, int y, bool clear){
   x = clampH(x); 
   y = clampV(y);
   int p = 0x80000000 >> (x & 0x1F);
   byte _y = (V_RES - 1) - y; // set origin at bottom left
   if(clear) buffer[_y][x >> 5] &= (~p);
   else buffer[_y][x >> 5] |= p;
}


void Teensy4NTSC::line(int x0, int y0, int x1, int y1, bool clear)
{
    int dx =  abs(x1-x0);
    int sx = x0<x1 ? 1 : -1;
    int dy = -abs(y1-y0);
    int sy = y0<y1 ? 1 : -1;
    int err = dx+dy;  /* error value e_xy */
    while (true){  /* loop */
        pixel(x0, y0, clear);
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


void Teensy4NTSC::rectangle(int x0, int y0, int x1, int y1, bool fill, bool clear){

	if(fill){
		order(&y0, &y1);
		while(y0 <= y1){
			line(x0, y0, x1, y0, clear);
			y0++; 
		}
	}
	else{
		line(x0, y0, x0, y1, clear);
		line(x0, y1, x1, y1, clear);
		line(x1, y1, x1, y0, clear);
		line(x0, y0, x1, y0, clear);
	}

	
}
