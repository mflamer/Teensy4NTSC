#include "Teensy4NTSC.h"
#include <Arduino.h>
#include <imxrt.h>
#include "font8x12.h"

// NTSC horiz sync timing (for reference, not used in code anymore)
#define H_SYNC 4.7
#define H_ACTIVE 58.8
#define H_BACK 10

DMAChannel Teensy4NTSC::dma = DMAChannel(false);
int Teensy4NTSC::buffer[V_RES][H_WORDS] = {0}; 

Teensy4NTSC::Teensy4NTSC(byte pinSync, byte pinPixels){
	
   	//DMA Setup
   	dma.begin();
   	dma.disable();
   	dma.sourceBuffer((int*)buffer, H_WORDS * 4 * V_RES);
   	dma.transferSize(4);
   	dma.destination(FLEXIO2_SHIFTBUFBIS0);
   	dma.triggerAtHardwareEvent(DMAMUX_SOURCE_FLEXIO2_REQUEST0);
   	dma.TCD->CSR	&= ~(DMA_TCD_CSR_DREQ);				// do not disable the channel after it completes - so it just keeps going 
   	dma.enable();

	
   	// Setup FlexIO
   	// Fast FlexIO CLK  
   	CCM_CS1CDR &= ~( CCM_CS1CDR_FLEXIO2_CLK_PODF( 7 ) );
   	CCM_CS1CDR |= CCM_CS1CDR_FLEXIO2_CLK_PODF( 1 );
   	// Enable CLK
   	CCM_CCGR3 |= CCM_CCGR3_FLEXIO2(CCM_CCGR_ON);
   	// Enable FlexIO module
   	FLEXIO2_CTRL |= 1;   
   	// Pad MUX for 2 pins
   	int FLEXIO2PIN_SYNC = 0; 	
   	switch(pinSync){
   		case 6 : IOMUXC_SW_MUX_CTL_PAD_GPIO_B0_10 = 4; FLEXIO2PIN_SYNC = 10;break;	
		case 9 : IOMUXC_SW_MUX_CTL_PAD_GPIO_B0_11 = 4; FLEXIO2PIN_SYNC = 11;break;	
		case 10: IOMUXC_SW_MUX_CTL_PAD_GPIO_B0_00 = 4; FLEXIO2PIN_SYNC = 0;break;
		case 12: IOMUXC_SW_MUX_CTL_PAD_GPIO_B0_01 = 4; FLEXIO2PIN_SYNC = 1;break;
		case 11: IOMUXC_SW_MUX_CTL_PAD_GPIO_B0_02 = 4; FLEXIO2PIN_SYNC = 2;break;
		case 13: IOMUXC_SW_MUX_CTL_PAD_GPIO_B0_03 = 4; FLEXIO2PIN_SYNC = 3;break;
		case 35: IOMUXC_SW_MUX_CTL_PAD_GPIO_B1_12 = 4; FLEXIO2PIN_SYNC = 12;break;	
		case 39: IOMUXC_SW_MUX_CTL_PAD_GPIO_B1_13 = 4; FLEXIO2PIN_SYNC = 13;break;	
		case 8 : IOMUXC_SW_MUX_CTL_PAD_GPIO_B1_00 = 4; FLEXIO2PIN_SYNC = 16;break;	
		case 7 : IOMUXC_SW_MUX_CTL_PAD_GPIO_B1_01 = 4; FLEXIO2PIN_SYNC = 17;break;	
		case 36: IOMUXC_SW_MUX_CTL_PAD_GPIO_B1_02 = 4; FLEXIO2PIN_SYNC = 18;break;	
		case 37: IOMUXC_SW_MUX_CTL_PAD_GPIO_B1_03 = 4; FLEXIO2PIN_SYNC = 19;break;	
   	}

   	int FLEXIO2PIN_PIXELS = 0; 	
   	switch(pinPixels){
   		case 6 : IOMUXC_SW_MUX_CTL_PAD_GPIO_B0_10 = 4; FLEXIO2PIN_PIXELS = 10;break;	
		case 9 : IOMUXC_SW_MUX_CTL_PAD_GPIO_B0_11 = 4; FLEXIO2PIN_PIXELS = 11;break;	
		case 10: IOMUXC_SW_MUX_CTL_PAD_GPIO_B0_00 = 4; FLEXIO2PIN_PIXELS = 0;break;
		case 12: IOMUXC_SW_MUX_CTL_PAD_GPIO_B0_01 = 4; FLEXIO2PIN_PIXELS = 1;break;
		case 11: IOMUXC_SW_MUX_CTL_PAD_GPIO_B0_02 = 4; FLEXIO2PIN_PIXELS = 2;break;
		case 13: IOMUXC_SW_MUX_CTL_PAD_GPIO_B0_03 = 4; FLEXIO2PIN_PIXELS = 3;break;
		case 35: IOMUXC_SW_MUX_CTL_PAD_GPIO_B1_12 = 4; FLEXIO2PIN_PIXELS = 12;break;	
		case 39: IOMUXC_SW_MUX_CTL_PAD_GPIO_B1_13 = 4; FLEXIO2PIN_PIXELS = 13;break;	
		case 8 : IOMUXC_SW_MUX_CTL_PAD_GPIO_B1_00 = 4; FLEXIO2PIN_PIXELS = 16;break;	
		case 7 : IOMUXC_SW_MUX_CTL_PAD_GPIO_B1_01 = 4; FLEXIO2PIN_PIXELS = 17;break;	
		case 36: IOMUXC_SW_MUX_CTL_PAD_GPIO_B1_02 = 4; FLEXIO2PIN_PIXELS = 18;break;	
		case 37: IOMUXC_SW_MUX_CTL_PAD_GPIO_B1_03 = 4; FLEXIO2PIN_PIXELS = 19;break;	
   	}


   	// Shifter for serial pixels out from DMA in 332b chunks   
   	FLEXIO2_SHIFTCFG0 = 0x00000020; // set stop bit 0 otherwise line stays high
   	FLEXIO2_SHIFTCTL0 = 0x01030002 | (FLEXIO2PIN_PIXELS << 8);
   	// Pixel timer - controls the pixel timing and count for Shifter0
   	FLEXIO2_TIMCMP1 = 0x3F07; // (32 * 2) - 1 |     
   	FLEXIO2_TIMCFG1 = 0x00001120;
   	FLEXIO2_TIMCTL1 = 0x00000001; 
   	// Pixel line counter - controls the number of 32b words sent per line
   	// enabled by AND gate below (shifter 1)
    FLEXIO2_TIMCMP0 = 0x13FF; // load 10 words
	FLEXIO2_TIMCFG0 = 0x00002600;
	FLEXIO2_TIMCTL0 = 0x0A400501;		
   	// Scale CLK for H line PWMs (next 2 timers 2 & 4)  
   	FLEXIO2_TIMCMP3 = 0x0000001A; // (H_ACTIVE / 2) - 1   	
   	FLEXIO2_TIMCFG3 = 0x00000000;
   	FLEXIO2_TIMCTL3 = 0x00000003;
   	// H Sync Timer - Drives the H_Sync signal pin (PWM)
   	FLEXIO2_TIMCMP2 = 0xFF18; // high|low 
   	FLEXIO2_TIMCFG2 = 0x00100000;
   	FLEXIO2_TIMCTL2 = 0x0F420082 | (FLEXIO2PIN_SYNC << 8);  
   	// H Active Line Timer = sets the delay to start pixels from H sync (PWM)
   	FLEXIO2_TIMCMP4 = 0xDF38; // high|low   
   	FLEXIO2_TIMCFG4 = 0x00100000;
   	FLEXIO2_TIMCTL4 = 0x0F430182;
   	// Scale H Active edges for line count used in next 2 PWMs  
   	FLEXIO2_TIMCMP7 = 0x0001; // low|high   0x15FF	
   	FLEXIO2_TIMCFG7 = 0x01100000;
   	FLEXIO2_TIMCTL7 = 0x13400003;
   	// V Sync signal - Drives the V Sync signal pin
   	FLEXIO2_TIMCMP5 = 0x1EFF; // low|high   	
   	FLEXIO2_TIMCFG5 = 0x00100000;
   	FLEXIO2_TIMCTL5 = 0x1F410002 | (FLEXIO2PIN_SYNC << 8);
   	// V Sync active lines - disables pixels during V Sync
   	FLEXIO2_TIMCMP6 = 0x2EEF; // low|high   	
   	FLEXIO2_TIMCFG6 = 0x00100000;
   	FLEXIO2_TIMCTL6 = 0x1F430202;
   	
   	// // Shifter as AND gate from timers 4 & 6 (shifter 1 pins = 1,2,3,4) 5 out
   	FLEXIO2_SHIFTCFG1 = 0x00000030; // mask pins 3 & 4
   	FLEXIO2_SHIFTCTL1 = 0x00030507;
   	FLEXIO2_SHIFTBUF1 = 0x00000008;   	
   	// Enable Shifter0 DMA signal
   	FLEXIO2_SHIFTSDEN |= 1;   	
   	
}







void Teensy4NTSC::order(int* v0, int* v1){
	if(*v0 > *v1){
		int t = *v0;
		*v0 = *v1;
		*v1 = t; 
	}
}


void Teensy4NTSC::clear(bool color){
	int v = color ? 0xFFFFFFFF : 0x00000000;
	for (int j = 0; j < V_RES; j++) {
      	for (int i = 0; i < H_WORDS; i++) {
           	buffer[j][i] = v;
      	}
   	}
}

void Teensy4NTSC::dump_buffer(){
	for (int j = 0; j < V_RES; j++) {
      	for (int i = 0; i < H_WORDS; i++) {
           	Serial.print(buffer[j][i]);
      	}
      	Serial.print('\n');
   	}
}



void Teensy4NTSC::pixel(int x, int y, bool color){
   x = clampH(x); 
   y = clampV(y);
   int p = 0x80000000 >> (x & 0x1F);
   int _y = (V_RES - 1) - y; // set origin at bottom left
   if(color == BLACK) buffer[_y][x >> 5] &= (~p);
   else buffer[_y][x >> 5] |= p;
}


void Teensy4NTSC::line(int x0, int y0, int x1, int y1, bool color)
{
    int dx =  abs(x1-x0);
    int sx = x0<x1 ? 1 : -1;
    int dy = -abs(y1-y0);
    int sy = y0<y1 ? 1 : -1;
    int err = dx+dy;  /* error value e_xy */
    while (true){  /* loop */
        pixel(x0, y0, color);
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


void Teensy4NTSC::rectangle(int x0, int y0, int x1, int y1, bool fill, bool color){

	if(fill){
		order(&y0, &y1);
		while(y0 <= y1){
			line(x0, y0, x1, y0, color);
			y0++; 
		}
	}
	else{
		line(x0, y0, x0, y1, color);
		line(x0, y1, x1, y1, color);
		line(x1, y1, x1, y0, color);
		line(x0, y0, x1, y0, color);
	}
	
}


void Teensy4NTSC::circleStep(int xc, int yc, int x, int y, bool fill, bool color)
{
	if(fill){
		line(xc+x, yc+y, xc-x, yc+y, color);
		line(xc-x, yc+y, xc+x, yc+y, color);
	    line(xc+x, yc-y, xc-x, yc-y, color);
	    line(xc-x, yc-y, xc+x, yc-y, color);
	    line(xc+y, yc+x, xc-y, yc+x, color);
	    line(xc-y, yc+x, xc+y, yc+x, color);
	    line(xc+y, yc-x, xc-y, yc-x, color);
	    line(xc-y, yc-x, xc+y, yc-x, color);
	}
	else{
	    pixel(xc+x, yc+y, color);
	    pixel(xc-x, yc+y, color);
	    pixel(xc+x, yc-y, color);
	    pixel(xc-x, yc-y, color);
	    pixel(xc+y, yc+x, color);
	    pixel(xc-y, yc+x, color);
	    pixel(xc+y, yc-x, color);
	    pixel(xc-y, yc-x, color);
	}
}
 

void Teensy4NTSC::circle(int xc, int yc, int r, bool fill, bool color)
{
    int x = 0, y = r;
    int d = 3 - 2 * r;
    circleStep(xc, yc, x, y, fill, color);
    while (y >= x++)
    {
        if (d > 0)
        {
            y--;
            d = d + 4 * (x - y) + 10;
        }
        else
            d = d + 4 * x + 6;
        circleStep(xc, yc, x, y, fill, color);
    }
}


void Teensy4NTSC::character(int c, int x, int y, bool invert){	
	for(int j = 0; j < 12; j++){
		char row = charmap[((c >> 4) * 192) + (c & 0xF) + (j * 16)];		
		for(int i = 0; i < 8; i++){
			bool p = (row & (0x80 >> i)) ? false : true;  
			p = invert ? !p : p;
			pixel(x + i, y - j, p);
		}
	}
}

void Teensy4NTSC::text(char* s, int x, int y, bool invert){
	char c;
	while((c = *s++)){
		character(c, x, y, invert);
		x += 8;
	}
}

