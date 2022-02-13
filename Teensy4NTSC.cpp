#include "Teensy4NTSC.h"
#include <Arduino.h>
#include <imxrt.h>
#include "font8x12.h"

// NTSC horiz sync timing (for reference, not used in code anymore)
// H_SYNC 4.7
// H_ACTIVE 58.8
// H_BACK 10

#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))


DMAChannel Teensy4NTSC::dma = DMAChannel(false);
int Teensy4NTSC::buffer[v_active_lines + v_blank_lines][HRES] = {0}; 
int Teensy4NTSC::v_res = 256;

Teensy4NTSC::Teensy4NTSC(int v_res){
	
	 this->v_res = MAX(0, MIN(v_res, v_active_lines));  	

   	//DMA Setup
   	dma.begin();
   	dma.disable();
   	dma.sourceBuffer((int*)buffer, HRES * 4 * (v_active_lines + v_blank_lines));
   	dma.transferSize(4);
   	dma.destination(FLEXIO2_SHIFTBUFBIS0);
   	dma.triggerAtHardwareEvent(DMAMUX_SOURCE_FLEXIO2_REQUEST0);
   	dma.TCD->CSR	&= ~(DMA_TCD_CSR_DREQ);				// do not disable the channel after it completes - so it just keeps going 
   	dma.enable();

	
   	// Setup FlexIO
   	// Fast FlexIO CLK (120MHz)
   	CCM_CS1CDR &= ~( CCM_CS1CDR_FLEXIO2_CLK_PODF( 7 ) );
   	CCM_CS1CDR |= CCM_CS1CDR_FLEXIO2_CLK_PODF( 1 );
   	// Enable CLK
   	CCM_CCGR3 |= CCM_CCGR3_FLEXIO2(CCM_CCGR_ON);
   	// Enable FlexIO module
   	FLEXIO2_CTRL |= 1;   
   	
   	// Pin Mux for FlexIO signals
   	int FLEXIO2PIN_SYNC = 0;
   	IOMUXC_SW_MUX_CTL_PAD_GPIO_B0_00 = 4; // FlexIO pin 0 - teensy pin 10  SYNC

   	int FLEXIO2PIN_PIXELS = 10;
   	IOMUXC_SW_MUX_CTL_PAD_GPIO_B0_10 = 4; // FlexIO pin 10 - teensy pin 6  MSB
	IOMUXC_SW_MUX_CTL_PAD_GPIO_B0_11 = 4; // FlexIO pin 11 - teensy pin 9
	IOMUXC_SW_MUX_CTL_PAD_GPIO_B0_12 = 4; // FlexIO pin 12 - teensy pin 32	 
	IOMUXC_SW_MUX_CTL_PAD_GPIO_B1_00 = 4; // FlexIO pin 16 - teensy pin 8  
	IOMUXC_SW_MUX_CTL_PAD_GPIO_B1_01 = 4; // FlexIO pin 17 - teensy pin 7
	IOMUXC_SW_MUX_CTL_PAD_GPIO_B1_02 = 4; // FlexIO pin 18 - teensy pin 36
	IOMUXC_SW_MUX_CTL_PAD_GPIO_B1_03 = 4; // FlexIO pin 19 - teensy pin 37 
	IOMUXC_SW_MUX_CTL_PAD_GPIO_B1_12 = 4; // FlexIO pin 28 - teensy pin 35 LSB

	IOMUXC_SW_MUX_CTL_PAD_GPIO_B0_01 = 4; // FlexIO pin 1 - teensy pin 12
	IOMUXC_SW_MUX_CTL_PAD_GPIO_B0_02 = 4; // FlexIO pin 2 - teensy pin 11
	
	
   	//--------------------------------------------------------------------------------
   	// Pixels
   	//--------------------------------------------------------------------------------
   	// Enable Shifter7 DMA signal
   	FLEXIO2_SHIFTSDEN = 0x01;

   	FLEXIO2_SHIFTCFG0 = 0x001F0000; // shift 32b | set stop bit 0 otherwise line stays high 0x001F0020 
   	FLEXIO2_SHIFTCTL0 = 0x01030002 | (FLEXIO2PIN_PIXELS << 8);
   	// Pixel timer - controls the pixel timing
   	FLEXIO2_TIMCMP1 = 0x0007; // (t * 2) - 1      
   	FLEXIO2_TIMCFG1 = 0x00001100; //0x00001120
   	FLEXIO2_TIMCTL1 = 0x03400003; 
   	// Pixel line counter - controls the number of 32b words sent per line
   	// enabled by AND gate below (shifter 1)
    FLEXIO2_TIMCMP0 = 0x13FF; // load 10 words
	FLEXIO2_TIMCFG0 = 0x00002400;
	FLEXIO2_TIMCTL0 = 0x00000503; //0x0A400501
	// Shifter as AND gate from timers 4 & 6 (shifter 1 pins = 1,2,3,4) 5 out
   	FLEXIO2_SHIFTCFG1 = 0x00000030; // mask pins 3 & 4
   	FLEXIO2_SHIFTCTL1 = 0x00030507;
   	FLEXIO2_SHIFTBUF1 = 0x00000008;   	


 //   	// Shifter for serial pixels out from DMA in 32b chunks   
 //   	FLEXIO2_SHIFTCFG0 = 0x001F0000; // shift 32b | set stop bit 0 otherwise line stays high 0x001F0020
 //   	FLEXIO2_SHIFTCTL0 = 0x00030000 | (FLEXIO2PIN_PIXELS << 8);   	
 //   	// Pixel line counter - controls the number of pixels sent per line   
 //   	// enabled by AND gate below (shifter 1)	
 //    FLEXIO2_TIMCMP0 = 0x027F; // (HRES * 2) - 1 
	// FLEXIO2_TIMCFG0 = 0x00302600;
	// FLEXIO2_TIMCTL0 = 0x07400003;
	// // Pixel timer - controls the pixel timing	
 //   	FLEXIO2_TIMCMP1 = 0x0007; // (t * 2) - 1     
 //   	FLEXIO2_TIMCFG1 = 0x00001600;
 //   	FLEXIO2_TIMCTL1 = 0x0A400003; 
	// Shifter as AND gate from timers 4 & 6 (shifter 1 pins = 1,2,3,4) 5 out
   	// FLEXIO2_SHIFTCFG1 = 0x00000030; // mask pins 3 & 4
   	// FLEXIO2_SHIFTCTL1 = 0x00030507;
   	// FLEXIO2_SHIFTBUF1 = 0x00000008;
   	//--------------------------------------------------------------------------------
   	// H_Sync
   	//--------------------------------------------------------------------------------   
   	// Scale CLK for H line PWMs (next 2 timers 2 & 4)  
   	FLEXIO2_TIMCMP3 = 0x0000001B; // (H_ACTIVE / 2) - 1   	
   	FLEXIO2_TIMCFG3 = 0x00000000;
   	FLEXIO2_TIMCTL3 = 0x00000003;
   	// H Sync Timer - Drives the H_Sync signal pin (PWM)
   	FLEXIO2_TIMCMP2 = 0xFC13; // high|low 
   	FLEXIO2_TIMCFG2 = 0x00100000;
   	FLEXIO2_TIMCTL2 = 0x0F420082 | (FLEXIO2PIN_SYNC << 8);  
   	// H Active Line Timer = sets the delay to start pixels from H sync (PWM)
   	FLEXIO2_TIMCMP4 = 0xD23D; // high|low   
   	FLEXIO2_TIMCFG4 = 0x00100000;
   	FLEXIO2_TIMCTL4 = 0x0F430182; //output on pin 1
   	//--------------------------------------------------------------------------------
   	// V_Sync
   	//--------------------------------------------------------------------------------
   	// Scale H Active edges for line count used in next 2 PWMs  
   	FLEXIO2_TIMCMP7 = 0x0003; // divider
   	FLEXIO2_TIMCFG7 = 0x01100000;
   	FLEXIO2_TIMCTL7 = 0x13400003;
   	// V Sync signal - Drives the V Sync signal pin
   	FLEXIO2_TIMCMP5 = 0x9400; //0x9400; // high|low    	
   	FLEXIO2_TIMCFG5 = 0x00100000;
   	FLEXIO2_TIMCTL5 = 0x1F410082 | (FLEXIO2PIN_SYNC << 8);
   	// V Sync active lines - disables pixels during V Sync
   	FLEXIO2_TIMCMP6 = 0x850F; //0x850F; // high|low    	
   	FLEXIO2_TIMCFG6 = 0x00100000;
   	FLEXIO2_TIMCTL6 = 0x1F430282; //output on pin 2
   	
   	   	
   	  	
   	
}



int Teensy4NTSC::clampH(int v){ 
	return MIN(h_res-1, MAX(0, v));
}
    
int Teensy4NTSC::clampV(int v){ 
	int floor_y = (v_active_lines - v_res) >> 1;
	int ceil_y = floor_y + v_res;
	v += floor_y; 
	return MIN(ceil_y, MAX(floor_y, v));
}

void Teensy4NTSC::order(int* v0, int* v1){
	if(*v0 > *v1){
		int t = *v0;
		*v0 = *v1;
		*v1 = t; 
	}
}

// Shuffle for kludgy bit storage due to FlexIO -> Teensy pin layout
int shuffle(char c){
	return ((c & 0xE0) << 24) | ((c & 0x1E) << 21) | ((c & 0x01) << 13);
}

void Teensy4NTSC::clear(char color){    
	int floor_y = (v_active_lines - v_res) >> 1;
	int ceil_y = floor_y + v_res;
	for (int j = floor_y; j <= ceil_y; j++) {
      	for (int i = 0; i < HRES; i++) {
           	buffer[j][i] = 0xFFFFFFFF;//shuffle(color);
      	}
   	}
}

void Teensy4NTSC::dump_buffer(){
	// for (int j = 0; j < v_res; j++) {
 //      	for (int i = 0; i < HRES; i++) {
 //           	Serial.print(buffer[j][i], HEX);
 //      	}
 //      	Serial.print('\n');
 //   	}
	Serial.print(FLEXIO2_SHIFTERR);
}



void Teensy4NTSC::pixel(int x, int y, char color){
   x = clampH(x); 
   y = clampV(y); 
   int _y = (v_active_lines - 1) - y; // set origin at bottom left
   buffer[_y][x] = shuffle(color); // set pixel   	
}


void Teensy4NTSC::line(int x0, int y0, int x1, int y1, char color)
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


void Teensy4NTSC::rectangle(int x0, int y0, int x1, int y1, bool fill, char color){

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


void Teensy4NTSC::circleStep(int xc, int yc, int x, int y, bool fill, char color)
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
 

void Teensy4NTSC::circle(int xc, int yc, int r, bool fill, char color)
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


void Teensy4NTSC::character(int c, int x, int y, char color){	
	for(int j = 0; j < 12; j++){
		char row = charmap[((c >> 4) * 192) + (c & 0xF) + (j * 16)];		
		for(int i = 0; i < 8; i++){ 
			if(row & (0x80 >> i)) pixel(x + i, y - j, color);
		}
	}
}

void Teensy4NTSC::text(const char* s, int x, int y, char color){
	char c;
	while((c = *s++)){
		character(c, x, y, color);
		x += 8;
	}
}



