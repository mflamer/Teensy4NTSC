#include "IntervalTimer.h"

// NTSC horiz sync timing
#define H_SYNC 4.7
#define H_BACK 10
// NTSC vert 
#define V_TOTAL_LINES 262
#define V_START 8
#define V_RES 216
#define V_SYNC 240
#define H_RES 256 
#define H_WORDS 8

typedef unsigned char byte;

class Teensy4NTSC {
public:
	Teensy4NTSC(){}
	Teensy4NTSC(byte pinBlack, byte pinWhite);

	void	clear(int v = 0);
	void 	pixel(int x, int y, bool clear = false);
	void	line(int x0, int y0, int x1, int y1, bool clear = false);
	void	rectangle(int x0, int y0, int x1, int y1, bool fill = false, bool clear = false);
	void	circle(int xc, int yc, int r, bool fill = false, bool clear = false);



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