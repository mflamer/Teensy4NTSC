#include <DMAChannel.h>

// Color constants
#define BLACK 0x00
#define WHITE 0xFF

typedef unsigned char byte;

class Teensy4NTSC {
public:	
	// Create object and set vertical resolution
	// v_res = Display vertical resolution. Optimal value depends on display device. Max = 256.
	Teensy4NTSC(int v_res = 256);

	void start();

	// Output pins are fixed 
	// 10 = Sync
	// 6,9,32,8,7,36,37,35 = MSB-LSB for 8 bit grayscale

	// Clear screen to a color
	void	clear(char color = BLACK);

	//// Drawing functions /////////////////////////////////////////////////////////////////////
	// The follwing functions draw shapes in either white (default) or black.
	//
	// Draw a pixel at a specified (x, y) coordinate 		
	void 	pixel(int x, int y, char color = WHITE);
	// Draw a line from (x0,y0) to (x1,y1) 
	void	line(int x0, int y0, int x1, int y1, char color = WHITE);
	// Draw a filled or empty rectangle defined by lower left and top right coordinates
	void	rectangle(int x0, int y0, int x1, int y1, bool fill = false, char color = WHITE);
	// Draw a filled or empty circle defined by a center coordinate and radius
	void	circle(int xc, int yc, int r, bool fill = false, char color = WHITE);
	// Draw a single character at a specified coordinate.
	void	character(int c, int x, int y, char color = WHITE);
	// Draw a single string of characters at a specified coordinate.
	void	text(const char* s, int x, int y, char color = WHITE);


	// Dump the buffer to serial for debugging
	void	dump_buffer();

	static int v_res;
	#define HRES 320
	static const int h_res = HRES;
	

private:
	// constant parameters
	static const int v_active_lines = 256;
	static const int v_blank_lines = 12;

	static char buffer[v_active_lines + v_blank_lines][HRES] __attribute__((aligned(32)));
	static DMAChannel dma;	
    
	
    int 	clampH(int v);
    int 	clampV(int v);
    void 	order(int* v0, int* v1);

    void	circleStep(int xc, int yc, int x, int y, bool fill, char color);
    
};


//FlexIO pin 0  - teensy pin 10 LSB
//FlexIO pin 1  - teensy pin 12
//FlexIO pin 2  - teensy pin 11	 
//FlexIO pin 3  - teensy pin 13  
//FlexIO pin 16 - teensy pin 8
//FlexIO pin 17 - teensy pin 7
//FlexIO pin 18 - teensy pin 36 
//FlexIO pin 19 - teensy pin 37 MSB

//FlexIO pin 11 - teensy pin 9 SYNC