#include <DMAChannel.h>

// Color constants
#define BLACK false
#define WHITE true

typedef unsigned char byte;

class Teensy4NTSC {
public:
	Teensy4NTSC(){}

	// Create object and set pin selections
	// pins =  6|7|8|9|10|11|12|13|35|36|37|39
	// v_res = Display vertical resolution. Optimal value depends on display device. Max = 256.
	Teensy4NTSC(byte pinSync, byte pinPixels, int v_res = 256);

	// Clear screen to a color
	void	clear(bool color = BLACK);

	//// Drawing functions /////////////////////////////////////////////////////////////////////
	// The follwing functions draw shapes in either white (default) or black.
	//
	// Draw a pixel at a specified (x, y) coordinate 		
	void 	pixel(int x, int y, bool color = WHITE);
	// Draw a line from (x0,y0) to (x1,y1) 
	void	line(int x0, int y0, int x1, int y1, bool color = WHITE);
	// Draw a filled or empty rectangle defined by lower left and top right coordinates
	void	rectangle(int x0, int y0, int x1, int y1, bool fill = false, bool color = WHITE);
	// Draw a filled or empty circle defined by a center coordinate and radius
	void	circle(int xc, int yc, int r, bool fill = false, bool color = WHITE);
	// Draw a single character at a specified coordinate.
	void	character(int c, int x, int y, bool color = WHITE);
	// Draw a single string of characters at a specified coordinate.
	void	text(const char* s, int x, int y, bool color = WHITE);


	// Dump the buffer to serial for debugging
	void	dump_buffer();

	static int v_res;
	static const int h_res = 320;
	

private:
	// constant parameters
	static const int v_active_lines = 256;
	static const int v_blank_lines = 12;

	static int buffer[v_active_lines + v_blank_lines][10];
	static DMAChannel dma;	
    
	
    int 	clampH(int v);
    int 	clampV(int v);
    void 	order(int* v0, int* v1);

    void	circleStep(int xc, int yc, int x, int y, bool fill, bool clear);
    
};


