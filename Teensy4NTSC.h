#include <DMAChannel.h>

// Luma constants
#define BLACK 0x00
#define WHITE 0xFF

typedef unsigned char byte;

class Teensy4NTSC {
public:	
	// Start the system and begin sending the NTSC signal. 
	// v_res = The desired display vertical resolution. Optimal value depends on display device. Max = 256.
	// Note: h_res is fixed at 320.
	void begin(void (*draw)(void), int v_res = 256);

	void swap();

	// Output pins
	// 9 = Sync
	// 37, 36, 7, 8, 13, 11, 12, 10 = MSB-LSB for 8 bit grayscale	

	//// Drawing functions /////////////////////////////////////////////////////////////////////
	// The follwing functions draw shapes in a specified luma
	//
	// Clear screen to a luma
	void	clear(byte luma = BLACK);
	// Copy a buffer of pixels into the buffer. User must verify size of src!
	void	copy(byte* src);
	// Draw a pixel at a specified (x, y) coordinate 		
	void 	pixel(int x, int y, byte luma = WHITE);
	// Draw a line from (x0,y0) to (x1,y1) 
	void	line(int x0, int y0, int x1, int y1, byte luma = WHITE);
	// Draw a filled or empty rectangle defined by lower left and top right coordinates
	void	rectangle(int x0, int y0, int x1, int y1, byte luma = WHITE, bool fill = true);
	// Draw a filled or empty circle defined by a center coordinate and radius
	void	circle(int xc, int yc, int r, byte luma = WHITE, bool fill = true);
	// Draw a single character at a specified coordinate.
	void	character(char c, int x, int y, byte luma = WHITE);
	// Draw a single string of characters at a specified coordinate.
	void	text(const char* s, int x, int y, byte luma = WHITE);


	// Dump the buffer to serial for debugging
	void	dump_buffer();

	#define HRES 320
	static int v_res;
	static const int h_res = HRES;
	

private:
	// constant parameters
	static const int v_active_lines = 256;
	static const int v_blank_lines = 12;
	static const int v_total_lines = v_active_lines + v_blank_lines;


	static byte buffer[2 * v_total_lines * HRES] __attribute__((aligned(32)));
	static byte* active_buffer;
	static DMAChannel dma;	
    
	
    int 	clampH(int v);
    int 	clampV(int v);
    void 	order(int* v0, int* v1);

    void	circleStep(int xc, int yc, int x, int y, byte luma, bool fill);
    
};

