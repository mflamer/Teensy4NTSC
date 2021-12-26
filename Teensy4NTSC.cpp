#include "Teensy4NTSC.h"
#include <Arduino.h>
#include <imxrt.h>
#include "font8x12.h"

IntervalTimer Teensy4NTSC::timer = IntervalTimer();
DMAChannel Teensy4NTSC::dma = DMAChannel(false);
int Teensy4NTSC::buffer[V_RES][H_WORDS] = {0}; 

Teensy4NTSC::Teensy4NTSC(byte pinBlack, byte pinWhite){
	// Setup black pin
	pinMode(pinBlack, OUTPUT);
   	digitalWriteFast(pinBlack, HIGH);  	

   	//start timer to interrupt at each line  	
   	timer.begin(sendLine, 63.5);

   //DMA Setup
   dma.begin();
   dma.disable();
   dma.sourceBuffer((int*)buffer, H_WORDS);
   dma.transferSize(1);
   dma.destination(FLEXIO2_SHIFTBUFBIS0);
   dma.triggerAtHardwareEvent(DMAMUX_SOURCE_FLEXIO2_REQUEST0);
   //dma1.triggerContinuously();
   dma.enable();



	
   	// Setup FlexIO (white pin)
   	// Fast FlexIO CLK  
   	CCM_CS1CDR &= ~( CCM_CS1CDR_FLEXIO2_CLK_PODF( 7 ) );
   	CCM_CS1CDR |= CCM_CS1CDR_FLEXIO2_CLK_PODF( 1 );
   	// Enable CLK
   	CCM_CCGR3 |= CCM_CCGR3_FLEXIO2(CCM_CCGR_ON);
   	// Enable FlexIO module
   	FLEXIO2_CTRL |= 1;   
   	// Pad MUX  
   	int FLEXIO2PIN = 0; 	
   	switch(pinWhite){
   		case 6 : IOMUXC_SW_MUX_CTL_PAD_GPIO_B0_10 = 4; FLEXIO2PIN = 10;break;	
		case 9 : IOMUXC_SW_MUX_CTL_PAD_GPIO_B0_11 = 4; FLEXIO2PIN = 11;break;	
		case 10: IOMUXC_SW_MUX_CTL_PAD_GPIO_B0_00 = 4; FLEXIO2PIN = 0;break;
		case 12: IOMUXC_SW_MUX_CTL_PAD_GPIO_B0_01 = 4; FLEXIO2PIN = 1;break;
		case 11: IOMUXC_SW_MUX_CTL_PAD_GPIO_B0_02 = 4; FLEXIO2PIN = 2;break;
		case 13: IOMUXC_SW_MUX_CTL_PAD_GPIO_B0_03 = 4; FLEXIO2PIN = 3;break;
		case 35: IOMUXC_SW_MUX_CTL_PAD_GPIO_B1_12 = 4; FLEXIO2PIN = 12;break;	
		case 39: IOMUXC_SW_MUX_CTL_PAD_GPIO_B1_13 = 4; FLEXIO2PIN = 13;break;	
		case 8 : IOMUXC_SW_MUX_CTL_PAD_GPIO_B1_00 = 4; FLEXIO2PIN = 16;break;	
		case 7 : IOMUXC_SW_MUX_CTL_PAD_GPIO_B1_01 = 4; FLEXIO2PIN = 17;break;	
		case 36: IOMUXC_SW_MUX_CTL_PAD_GPIO_B1_02 = 4; FLEXIO2PIN = 18;break;	
		case 37: IOMUXC_SW_MUX_CTL_PAD_GPIO_B1_03 = 4; FLEXIO2PIN = 19;break;	
   	}

   	// Shifter to pin
   	FLEXIO2_SHIFTCFG0 = 0x00000120; // set stop bit 0 otherwise line stays high
   	FLEXIO2_SHIFTCTL0 = 0x00030002 | (FLEXIO2PIN << 8);
   	// Shift counter
   	FLEXIO2_TIMCMP0 = 0x000001FF; // (H_RES * 2) - 1 
   	FLEXIO2_TIMCTL0 = 0x07400003;
   	FLEXIO2_TIMCFG0 = 0x00302630;
   	// Baud timer
   	FLEXIO2_TIMCMP1 = 0x00000009; // (PixelRateDiv / 2) - 1
   	FLEXIO2_TIMCTL1 = 0x01C00003;
   	FLEXIO2_TIMCFG1 = 0x00001200;
   	// Shifter DMA
   	FLEXIO2_SHIFTSDEN |= 1;
   	
   	
}

void Teensy4NTSC::sendLine(){
	static int line = 0;
	if((line < (V_RES + V_START)) && (line >= V_START)){
      digitalWriteFast(3, LOW);
      delayMicroseconds(H_SYNC);
      digitalWriteFast(3, HIGH);
      delayMicroseconds(H_BACK);       
      
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


void Teensy4NTSC::circleStep(int xc, int yc, int x, int y, bool fill, bool clear)
{
	if(fill){
		line(xc+x, yc+y, xc-x, yc+y, clear);
		line(xc-x, yc+y, xc+x, yc+y, clear);
	    line(xc+x, yc-y, xc-x, yc-y, clear);
	    line(xc-x, yc-y, xc+x, yc-y, clear);
	    line(xc+y, yc+x, xc-y, yc+x, clear);
	    line(xc-y, yc+x, xc+y, yc+x, clear);
	    line(xc+y, yc-x, xc-y, yc-x, clear);
	    line(xc-y, yc-x, xc+y, yc-x, clear);
	}
	else{
	    pixel(xc+x, yc+y, clear);
	    pixel(xc-x, yc+y, clear);
	    pixel(xc+x, yc-y, clear);
	    pixel(xc-x, yc-y, clear);
	    pixel(xc+y, yc+x, clear);
	    pixel(xc-y, yc+x, clear);
	    pixel(xc+y, yc-x, clear);
	    pixel(xc-y, yc-x, clear);
	}
}
 

void Teensy4NTSC::circle(int xc, int yc, int r, bool fill, bool clear)
{
    int x = 0, y = r;
    int d = 3 - 2 * r;
    circleStep(xc, yc, x, y, fill, clear);
    while (y >= x++)
    {
        if (d > 0)
        {
            y--;
            d = d + 4 * (x - y) + 10;
        }
        else
            d = d + 4 * x + 6;
        circleStep(xc, yc, x, y, fill, clear);
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

