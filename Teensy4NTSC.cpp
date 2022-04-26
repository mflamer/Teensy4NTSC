#include "Teensy4NTSC.h"
#include <Arduino.h>
#include <imxrt.h>
#include "stdint.h"
#include "font8x12.h"

// NTSC horiz sync timing (for reference, not used in code anymore)
// H_SYNC 4.7
// H_ACTIVE 58.8
// H_BACK 10

#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))


DMAChannel Teensy4NTSC::dma = DMAChannel(false);
char Teensy4NTSC::buffer[v_active_lines + v_blank_lines][HRES]  __attribute__((aligned(32))) = {0}; 
int Teensy4NTSC::v_res = 256;
char (*Teensy4NTSC::active_buffer)[HRES] = Teensy4NTSC::buffer;


void Teensy4NTSC::begin(int v_res){
	this->v_res = MAX(0, MIN(v_res, v_active_lines)); 
   	this->active_buffer = buffer + ((v_active_lines - this->v_res) >> 1);
   	
	/////////////////////////////////////////////////////////////////////////////////////
	//DMA Setup
   	dma.begin();
   	dma.disable();

   	dma.TCD->NBYTES			= 32;

   	dma.TCD->SADDR			= buffer;
	dma.TCD->SOFF			= 4;
	dma.TCD->ATTR_SRC		= 2;
	dma.TCD->SLAST			= -(HRES * (v_active_lines + v_blank_lines));

	dma.TCD->DADDR			= &FLEXIO2_SHIFTBUF0;
	dma.TCD->DOFF			= 4;
	dma.TCD->ATTR_DST		= (5 << 3) | 2;	
	dma.TCD->DLASTSGA		= 0;

	dma.TCD->BITER		= ((HRES * (v_active_lines + v_blank_lines)) / 32);
	dma.TCD->CITER		= ((HRES * (v_active_lines + v_blank_lines)) / 32);

   	dma.triggerAtHardwareEvent(DMAMUX_SOURCE_FLEXIO2_REQUEST0);
   	// do not disable the channel after it completes - so it just keeps going 
   	dma.TCD->CSR	&= ~(DMA_TCD_CSR_DREQ);	  	

	/////////////////////////////////////////////////////////////////////////////////////
   	// Setup FlexIO

   	// Enable CLK
   	CCM_CCGR3 |= CCM_CCGR3_FLEXIO2(CCM_CCGR_ON);
   	// Fast FlexIO CLK (120MHz)
   	CCM_CS1CDR &= ~( CCM_CS1CDR_FLEXIO2_CLK_PODF( 7 ) );
   	CCM_CS1CDR |= CCM_CS1CDR_FLEXIO2_CLK_PODF( 1 ); 
   	
   	// Pin Mux for FlexIO signals
   	int FLEXIO2PIN_SYNC = 11;
   	int FLEXIO2PIN_PIXELS_L = 0;
   	int FLEXIO2PIN_PIXELS_H = 16;

 	IOMUXC_SW_MUX_CTL_PAD_GPIO_B0_00 = 4; // FlexIO pin 0 - teensy pin 10 LSB
	IOMUXC_SW_MUX_CTL_PAD_GPIO_B0_01 = 4; // FlexIO pin 1 - teensy pin 12 
	IOMUXC_SW_MUX_CTL_PAD_GPIO_B0_02 = 4; // FlexIO pin 2 - teensy pin 11
	IOMUXC_SW_MUX_CTL_PAD_GPIO_B0_03 = 4; // FlexIO pin 3 - teensy pin 13

	IOMUXC_SW_MUX_CTL_PAD_GPIO_B0_11 = 4; // FlexIO pin 11 - teensy pin 9 		

	IOMUXC_SW_MUX_CTL_PAD_GPIO_B1_00 = 4; // FlexIO pin 16 - teensy pin 8  
	IOMUXC_SW_MUX_CTL_PAD_GPIO_B1_01 = 4; // FlexIO pin 17 - teensy pin 7
	IOMUXC_SW_MUX_CTL_PAD_GPIO_B1_02 = 4; // FlexIO pin 18 - teensy pin 36
	IOMUXC_SW_MUX_CTL_PAD_GPIO_B1_03 = 4; // FlexIO pin 19 - teensy pin 37 MSB

	//IOMUXC_SW_MUX_CTL_PAD_GPIO_B0_12 = 4; // FlexIO pin 12 - teensy pin 32
	//IOMUXC_SW_MUX_CTL_PAD_GPIO_B1_12 = 4; // FlexIO pin 28 - teensy pin 35 
	//IOMUXC_SW_MUX_CTL_PAD_GPIO_B1_13 = 4; // FlexIO pin 29 - teensy pin 34 	
	
   	//--------------------------------------------------------------------------------
   	// Pixels
   	//--------------------------------------------------------------------------------
   	// Enable Shifter0 DMA signal
   	FLEXIO2_SHIFTSDEN = 0x01;
   	// shifter0 is the output to the pins for the low nibble
   	FLEXIO2_SHIFTCFG0 = 0x00030120; // shift 32b | set stop bit 0 otherwise line stays high
   	FLEXIO2_SHIFTCTL0 = 0x01030002 | (FLEXIO2PIN_PIXELS_L << 8);
  	FLEXIO2_SHIFTCFG1 = 0x00030100;
	FLEXIO2_SHIFTCTL1 = 0x01030002;
	FLEXIO2_SHIFTCFG2 = 0x00030100;
	FLEXIO2_SHIFTCTL2 = 0x01030002;
	FLEXIO2_SHIFTCFG3 = 0x00030000;
	FLEXIO2_SHIFTCTL3 = 0x01030002;

   	// shifter04 is the output to the pins for the high nibble
   	FLEXIO2_SHIFTCFG4 = 0x00030120; // shift 32b | set stop bit 0 otherwise line stays high 
   	FLEXIO2_SHIFTCTL4 = 0x01030002 | (FLEXIO2PIN_PIXELS_H << 8);
   	FLEXIO2_SHIFTCFG5 = 0x00030100;
	FLEXIO2_SHIFTCTL5 = 0x01030002;
	FLEXIO2_SHIFTCFG6 = 0x00030100;
	FLEXIO2_SHIFTCTL6 = 0x01030002;
	FLEXIO2_SHIFTCFG7 = 0x00030000;
	FLEXIO2_SHIFTCTL7 = 0x01030002;

   	// Pixel timer - controls the pixel timing
   	FLEXIO2_TIMCMP1 = 0x3F07; // (t * 2) - 1    send 32 then reload from DMA
   	FLEXIO2_TIMCFG1 = 0x00001120; 
   	FLEXIO2_TIMCTL1 = 0x03400001; 
   	// Pixel line counter - controls the number of 32b words sent per line
   	// enabled by timers 4 & 6
    FLEXIO2_TIMCMP0 = 0x13FF; // send 320 pixels then disable
	FLEXIO2_TIMCFG0 = 0x00002500; //0x00002500
	FLEXIO2_TIMCTL0 = 0x1BC01F03; //0x1BC00403

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
   	FLEXIO2_TIMCTL4 = 0x0F431F82; // output pin 4 0x0F430482
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
   	FLEXIO2_TIMCTL6 = 0x1F400002; 

	dma.enable();
	// Enable FlexIO module
   	FLEXIO2_CTRL |= 1;
}



int Teensy4NTSC::clampH(int v){ 
	return MIN(h_res-1, MAX(0, v));
}
    
int Teensy4NTSC::clampV(int v){ 
	return MIN(v_res - 1, MAX(0, v));
}

void Teensy4NTSC::order(int* v0, int* v1){
	if(*v0 > *v1){
		int t = *v0;
		*v0 = *v1;
		*v1 = t; 
	}
}

// Swizzle low and high nibbles for DMA -> FlexIO access 
// |L,x| 15 Bytes |H,x| 15 Bytes |  
void swizzle(char* addr, char pix){
	int a = (int)addr;
   	char* a_ = (char*)((a & 0xFFFFFFE0) | ((a & 0x1F) >> 1));
   	if(a & 0x1){ // this is an odd pixel
	   *(a_) &= 0x0F; 			 	 // clear high nibble
	   *(a_) |= ((0x0F & pix) << 4); // set high nibble  
	   *(a_ + 16) &= 0x0F; 			 // clear high nibble
	   *(a_ + 16) |= (0xF0 & pix);  // set high nibble   	
	}
	else{ // this is an even pixel
	   *(a_) &= 0xF0; 			 	 // clear low nibble
	   *(a_) |= (0x0F & pix); // set high nibble  
	   *(a_ + 16) &= 0xF0; 			 // clear high nibble
	   *(a_ + 16) |= ((0xF0 & pix) >> 4);  // set low nibble   	
	}
}

void Teensy4NTSC::clear(char luma){  
	for (int j = 0; j < v_res; j++) {
      	for (int i = 0; i < h_res; i++) {
           	swizzle(&active_buffer[j][i], luma);
      	}
   	}
}

void Teensy4NTSC::dump_buffer(){
	for (int j = 0; j < 268; j++) {
      	for (int i = 0; i < HRES; i++) {
           	Serial.print(buffer[j][i] & 0x0F, HEX);
           	Serial.print((buffer[j][i] >> 4) & 0x0F, HEX);
      	}
      	Serial.print(" |\n");
   	}
	//Serial.print(FLEXIO2_SHIFTERR); // debug dma access issues
}

void Teensy4NTSC::pixel(int x, int y, char luma){
   x = clampH(x); 
   y = clampV(y); 
   int _y = (v_res - 1) - y; // set origin at bottom left
   swizzle(&active_buffer[_y][x], luma);
}

void Teensy4NTSC::line(int x0, int y0, int x1, int y1, char luma)
{
    int dx =  abs(x1-x0);
    int sx = x0<x1 ? 1 : -1;
    int dy = -abs(y1-y0);
    int sy = y0<y1 ? 1 : -1;
    int err = dx+dy;  /* error value e_xy */
    while (true){  /* loop */
        pixel(x0, y0, luma);
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


void Teensy4NTSC::rectangle(int x0, int y0, int x1, int y1, char luma, bool fill){

	if(fill){
		order(&y0, &y1);
		while(y0 <= y1){
			line(x0, y0, x1, y0, luma);
			y0++; 
		}
	}
	else{
		line(x0, y0, x0, y1, luma);
		line(x0, y1, x1, y1, luma);
		line(x1, y1, x1, y0, luma);
		line(x0, y0, x1, y0, luma);
	}
	
}


void Teensy4NTSC::circleStep(int xc, int yc, int x, int y, char luma, bool fill)
{
	if(fill){
		line(xc+x, yc+y, xc-x, yc+y, luma);
		line(xc-x, yc+y, xc+x, yc+y, luma);
	    line(xc+x, yc-y, xc-x, yc-y, luma);
	    line(xc-x, yc-y, xc+x, yc-y, luma);
	    line(xc+y, yc+x, xc-y, yc+x, luma);
	    line(xc-y, yc+x, xc+y, yc+x, luma);
	    line(xc+y, yc-x, xc-y, yc-x, luma);
	    line(xc-y, yc-x, xc+y, yc-x, luma);
	}
	else{
	    pixel(xc+x, yc+y, luma);
	    pixel(xc-x, yc+y, luma);
	    pixel(xc+x, yc-y, luma);
	    pixel(xc-x, yc-y, luma);
	    pixel(xc+y, yc+x, luma);
	    pixel(xc-y, yc+x, luma);
	    pixel(xc+y, yc-x, luma);
	    pixel(xc-y, yc-x, luma);
	}
}
 

void Teensy4NTSC::circle(int xc, int yc, int r, char luma, bool fill)
{
    int x = 0, y = r;
    int d = 3 - 2 * r;
    circleStep(xc, yc, x, y, luma, fill);
    while (y >= x++)
    {
        if (d > 0)
        {
            y--;
            d = d + 4 * (x - y) + 10;
        }
        else
            d = d + 4 * x + 6;
        circleStep(xc, yc, x, y, luma, fill);
    }
}


void Teensy4NTSC::character(int c, int x, int y, char luma){	
	for(int j = 0; j < 12; j++){
		char row = charmap[((c >> 4) * 192) + (c & 0xF) + (j * 16)];		
		for(int i = 0; i < 8; i++){ 
			if(row & (0x80 >> i)) pixel(x + i, y - j, luma);
		}
	}
}

void Teensy4NTSC::text(const char* s, int x, int y, char luma){
	char c;
	while((c = *s++)){
		character(c, x, y, luma);
		x += 8;
	}
}



