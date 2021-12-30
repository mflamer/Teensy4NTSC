#include "IntervalTimer.h"

// NTSC horiz sync timing
#define H_SYNC 4.7
#define H_BACK 10
// NTSC vert 
#define V_TOTAL_LINES 260
#define V_RES 240
#define V_SYNC 252
#define H_RES 257 
#define H_WORDS 8

typedef unsigned char byte;

class Teensy4NTSC {
public:
	Teensy4NTSC(){}

	// Create object setting signal output pin selections
	// pinBlack = any digital pin
	// pinWhite = 6|7|8|9|10|11|12|13|35|36|37|39
	Teensy4NTSC(byte pinBlack, byte pinWhite);

	// Clear screen with optional bit pattern
	void	clear(int v = 0);
	// Set or clear a pixel at a specified (x, y) coordinate 		
	void 	pixel(int x, int y, bool clear = false);
	// Draw or clear a line from (x0,y0) to (x1,y1) 
	void	line(int x0, int y0, int x1, int y1, bool clear = false);
	// Draw or clear a filled or empty rectangle defined by lower left and top right coordinates
	void	rectangle(int x0, int y0, int x1, int y1, bool fill = false, bool clear = false);
	// Draw or clear a filled or empty circle defined by center and radius
	void	circle(int xc, int yc, int r, bool fill = false, bool clear = false);
	// Draw a single character at a specified coordinate. Optionaly inverted.
	void	character(int c, int x, int y, bool invert = false);
	// Draw a single string of characters at a specified coordinate. Optionaly inverted.
	void	text(char* s, int x, int y, bool invert = false);



private:
	static int buffer[V_RES][H_WORDS];
	static IntervalTimer timer;

    static void	sendLine();

    
	#define MIN(a,b) (((a)<(b))?(a):(b))
	#define MAX(a,b) (((a)>(b))?(a):(b))
    int 	clampH(int v){ return MIN(255, MAX(0, v));}
    int 	clampV(int v){ return MIN(215, MAX(0, v));}
    void 	order(int* v0, int* v1);

    void	circleStep(int xc, int yc, int x, int y, bool fill, bool clear);
    
};