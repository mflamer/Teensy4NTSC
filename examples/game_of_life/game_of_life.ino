
#include "Teensy4NTSC.h"


Teensy4NTSC ntsc;


///////////////////////////////////////////////////////////////////////////////
//Port of Conway's Game of Life to Mark Flamer's FlexIO NTSC for Teensy 4
//G. Kovacs, 12/26/21

static const int H_RES = 320;
static const int V_RES = 216;

int xmax=ntsc.h_res-1;
int ymax=ntsc.v_res-1;
int generation = 0;
int maxgen = 2000; //number of generations before reseeding randomly
int neighbors(int x, int y);

byte world[H_RES][V_RES][2];
long density = 12;

void setup() {
// Create NTSC display system and set pin selections
// Teensy4NTSC(byte pinSync, byte pinPixels, int v_res = 256);
// pins =  6|7|8|9|10|11|12|13|35|36|37|39
// v_res = Display vertical resolution. Optimal value depends on display device. Max = 256.
    ntsc = Teensy4NTSC(6, 7, V_RES);  
    ntsc.clear();
}

FASTRUN void loop() {
  if (generation == 0) {
    // randomSeed(analogRead(A5));
    for (int i = 0; i < xmax; i++) {
      for (int j = 0; j < ymax; j++) {
        if (int(random(100)) < density) {
          world[i][j][0] = 1;
        }
        else {
          world[i][j][0] = 0;
        }
        world[i][j][1] = 0;
      }
    }
  } 

  //delay(100); //Optional delay to lengthen time world is displayed.
 
  //ntsc.clear(); //No need to clear screen, since writing every pixel each time at full resolution.

  // Display current generation
  for (int i = 0; i < xmax; i++)
   {
    for (int j = 0; j < ymax; j++)
      {
        ntsc.pixel(i, j, world[i][j][0]);
       }
   }
 
  // Birth and death cycle
  for (int x = 0; x < xmax; x++) {
    for (int y = 0; y < ymax; y++) {
      // Default is for cell to stay the same
      world[x][y][1] = world[x][y][0];
      int count = neighbors(x, y);
      if (count == 3 && !world[x][y][0]) {
        // A new cell is born
        world[x][y][1] = 1;
      }
      if ((count < 2 || count > 3) && world[x][y][0]) {
        // Cell dies
        world[x][y][1] = 0;
      }
    }
  }
 
  // Copy next generation into place
  for (int x = 0; x < xmax; x++) {
    for (int y = 0; y < ymax; y++) {
      world[x][y][0] = world[x][y][1];
    }
  }

  generation++;
  if (generation >= maxgen)
    generation=0;
}

FASTRUN int neighbors(int x, int y) {
return (1 & world[(x + 1) % xmax][y][0]) +
        (1 & world[x][(y + 1) % ymax][0]) +
        (1 & world[(x + xmax - 1) % xmax][y][0]) +
        (1 & world[x][(y + ymax - 1) % ymax][0]) +
        (1 & world[(x + 1) % xmax][(y + 1) % ymax][0]) +
        (1 & world[(x + xmax - 1) % xmax][(y + 1) % ymax][0]) +
        (1 & world[(x + xmax - 1) % xmax][(y + ymax - 1) % ymax][0]) +
        (1 & world[(x + 1) % xmax][(y + ymax - 1) % ymax][0]);
}
  	

   





