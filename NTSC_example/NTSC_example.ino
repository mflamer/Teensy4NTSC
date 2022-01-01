
#include "Teensy4NTSC.h"


Teensy4NTSC ntsc;


void setup() {

	// Create object setting signal output pin selections
	// pinBlack = any digital pin
	// pinWhite = 6|7|8|9|10|11|12|13|35|36|37|39
	// Teensy4NTSC(byte pinBlack, byte pinWhite);
   	ntsc = Teensy4NTSC(3, 7);   
   	ntsc.clear();
   	   
   	// ntsc.line(128, 108, 255, 215);
   	// ntsc.line(128, 108, 255, 0);
   	// ntsc.line(128, 108, 0, 215);
   	// ntsc.line(128, 108, 0, 0);
   	// ntsc.line(128, 108, 148, 215);
   	// ntsc.line(128, 108, 148, 0);
   	// ntsc.line(128, 108, 108, 215);
   	// ntsc.line(128, 108, 108, 0);
	
   	// ntsc.line(128, 108, 128, 215);
   	// ntsc.line(128, 108, 128, 0);
   	// ntsc.line(128, 108, 0, 108);
   	// ntsc.line(128, 108, 255, 108);
	
   	ntsc.rectangle(0, 0, H_RES-1, V_RES-1);
   	// ntsc.rectangle(0, 0, 20, 20, true);
   	// ntsc.rectangle(5, 5, 15, 15, true, true);
   	// ntsc.rectangle(255, 215, 235, 195, true);
   	// ntsc.rectangle(250, 210, 240, 200, true, true);
	
   	
   	// ntsc.circle(128, 108, 20, true);
   	// ntsc.circle(128, 108, 10, true, true);
   	// ntsc.circle(128, 108, 25);

   	// ntsc.text("Hi Greg! \x1 \x12", 4, 180);
   	// ntsc.text("This is a test", 4, 50, true);

}






// int cnt = 0;
// int y_ = 0;
// int x = 0;
// int y = 108;


void loop() {


	// // fizz
	// for (int i = 0; i < 150; i++)
 //  	{
 //    	int x = random(255);
 //    	int y = random(215);
 //    	int r = random(25);
 //    	bool fill = random(0, 2); 
 //    	bool clear = random(0, 2);
 //    	delay(20);
 //        ntsc.circle(x, y, r, fill, clear);
 //  	}
 //  	delay(200);
 //  	ntsc.clear();
  	
	// random eraser
 //  	delay(30);  	
	// ntsc.circle(x++ & 0xFF, y & 0xFF, 2, true, true);	
	// y += y_;
	// ntsc.circle(x & 0xFF, y & 0xFF, 2, true); 	   
 //    // randomly set y delta
 //    if(cnt == 10){
	// 	y_ = random(-1, 2);
	// 	cnt = 0;
	// }
	// cnt++;  
   
}



// //Port of Conway's Game of Life to Mark Flamer's FlexIO NTSC for Teensy 4
// //G. Kovacs, 12/26/21


// //NTSC x=256, y=215 pixels (maybe 219 - need to check for sure)

// int xmax=256;
// int ymax=215;
// int generation = 0;
// int maxgen = 2000; //number of generations before reseeding randomly
// int neighbors(int x, int y);

// byte world[256][256][2];
// long density = 12;

// void setup() {
//   // Create object setting signal output pin selections
//   // pinBlack = any digital pin6
//   // pinWhite = 6|7|8|9|10|11|12|13|35|36|37|39
//     ntsc = Teensy4NTSC(3, 7);  
//     ntsc.clear();
// }

// FASTRUN void loop() {
//   if (generation == 0) {
//     // randomSeed(analogRead(A5));
//     for (int i = 0; i < xmax; i++) {
//       for (int j = 0; j < ymax; j++) {
//         if (int(random(100)) < density) {
//           world[i][j][0] = 1;
//         }
//         else {
//           world[i][j][0] = 0;
//         }
//         world[i][j][1] = 0;
//       }
//     }
//   } 

//   //delay(100); //Optional delay to lengthen time world is displayed.
 
//   //ntsc.clear(); //No need to clear screen, since writing every pixel each time at full resolution.

//   // Display current generation
//   for (int i = 0; i < xmax; i++)
//    {
//     for (int j = 0; j < ymax; j++)
//       {
//         ntsc.pixel(i, j, !world[i][j][0]);
//        }
//    }
 
//   // Birth and death cycle
//   for (int x = 0; x < xmax; x++) {
//     for (int y = 0; y < ymax; y++) {
//       // Default is for cell to stay the same
//       world[x][y][1] = world[x][y][0];
//       int count = neighbors(x, y);
//       if (count == 3 && !world[x][y][0]) {
//         // A new cell is born
//         world[x][y][1] = 1;
//       }
//       if ((count < 2 || count > 3) && world[x][y][0]) {
//         // Cell dies
//         world[x][y][1] = 0;
//       }
//     }
//   }
 
//   // Copy next generation into place
//   for (int x = 0; x < xmax; x++) {
//     for (int y = 0; y < ymax; y++) {
//       world[x][y][0] = world[x][y][1];
//     }
//   }

//   generation++;
//   if (generation >= maxgen)
//     generation=0;
// }

// FASTRUN int neighbors(int x, int y) {
// return (1 & world[(x + 1) % xmax][y][0]) +
//         (1 & world[x][(y + 1) % ymax][0]) +
//         (1 & world[(x + xmax - 1) % xmax][y][0]) +
//         (1 & world[x][(y + ymax - 1) % ymax][0]) +
//         (1 & world[(x + 1) % xmax][(y + 1) % ymax][0]) +
//         (1 & world[(x + xmax - 1) % xmax][(y + 1) % ymax][0]) +
//         (1 & world[(x + xmax - 1) % xmax][(y + ymax - 1) % ymax][0]) +
//         (1 & world[(x + 1) % xmax][(y + ymax - 1) % ymax][0]);
// }


