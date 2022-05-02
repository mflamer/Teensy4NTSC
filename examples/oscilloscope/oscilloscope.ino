// NTSC Oscilloscope Demo Using Teensy 4.X Internal ADC and Teensy4NTSC Library
// R0.0 1/15/22 G. Kovacs
// Based on much prior work using hardware NTSC generator VS23S010
// Trigger debugging help from Laurent Giovangrandi 3/10/20
// R0.3 1/17/22 Improve performance for non-triggered state (revert to free-run but show waveform).
// R0.4 4/24/22 Update for new NTSC4NTSC library.
//
//

#define internalADC //Uncomment to use internal ADC on analog input A0.
#define gridOn //Uncomment to draw (for now slow) gridlines on screen
#define connectDots //Uncomment to draw lines between samples on screen.
//#define potsNotInstalled //Uncomment for debug with no potentiometers installed - set to sensible values.
#define acquisitionBufferSize 1024
#define adcResolution 8 //Can use any value from 1 to 16 here, but 8 is best you could really see on monitor and allows one-port reads for either ADC with new pins
//#define enableFFT

#include <Arduino.h>
#include <SPI.h>
#include <arduinoFFT.h> //https://github.com/kosme/arduinoFFT
#include "Teensy4NTSC.h"

Teensy4NTSC ntsc;

#ifdef enableFFT
#define FFTbinsM 1024
#define acquisitionBufferSize FFTbinsM
double vReal[FFTbinsM]; //InputSignal and output arrays.
double vImag[FFTbinsM]; //Put data in, and it will be replaced by FFT results.
arduinoFFT FFT = arduinoFFT(); /* Create FFT object */
#define connectDots
#endif

const int xPixels = 320, yPixels = 256;
int i,j,x,y; // Used for counters
int xOut, yOut, xDelta, yDelta;
int delTimePointer;
float lineLength, xIncrement, yIncrement, xOutFloat, yOutFloat;

volatile boolean sampleNow = false;
volatile int samplePointer = 0;
volatile int samples[acquisitionBufferSize];
uint16_t samplesDisplayBuffer[acquisitionBufferSize];

float SampleRate = 50000.0; //Initial sample rate, set independently in FFT mode.
int interruptPeriod = int((1/SampleRate)*1E6); //Sample rate in integer microseconds.
int triggerPointer, triggerDisplayPoint;
boolean triggered = false;
int adcRangeMax = pow(2,adcResolution)-1;
float trigLevel = 0.5;
int trigLevelInt = int(trigLevel*adcRangeMax);
int hysteresisAmplitudeStep = 2;
int hysteresisTimeStep = 2;

int pot0 = 24, pot1 = 25, pot2 = 18, pot3 = 19; //Pins to which the potentiometer wipers are connected
float pot0val = 0, pot1val = 0, pot2val = 0, pot3val = 0; //Variables holding aquired pot settings
int button0 = 29, button1 = 30, button2 = 31, button3 = 32; //Pins to wich the buttons are connected

char temp[20], oldTemp[20];

IntervalTimer samplingTimer; //The overall interrupt timing for the system.

void setup() {
    analogReadResolution(adcResolution);
    pinMode(A0, INPUT);
    //See: https://forum.pjrc.com/threads/27690-IntervalTimer-is-not-precise
    //Technique to reduce intervalTimer jitter.
    SCB_SHPR3 = 0x20200000; // Systick = priority 32 (defaults to zero, or highest priority)
    samplingTimer.priority(0);
    //samplingTimer.begin(ISR, interruptPeriod);
    //Create NTSC display system and set pin selections
    //Teensy4NTSC(byte pinSync, byte pinPixels, int v_res = 256);
    //pins = 6|7|8|9|10|11|12|13|35|36|37|39
    //v_res = Display vertical resolution. Optimal value depends on display device. Max = 256.
    ntsc.begin(draw, yPixels);
    ntsc.clear();
    pinMode(pot0, INPUT);
    pinMode(pot1, INPUT);
    pinMode(pot2, INPUT);
    pinMode(pot3, INPUT);
    pinMode(button0, INPUT_PULLUP);
    pinMode(button1, INPUT_PULLUP);
    pinMode(button2, INPUT_PULLUP);
    pinMode(button3, INPUT_PULLUP);
}

void yield(); //Can use to suppress background interrupts - not sure if it helps here.

#ifdef enableFFT
void loop(){
interruptPeriod = 20; //set to 50 kSPS sample rate, which will display frequencies up to fs/2 - 25 kHz
readPots(); //Read potentiometers to get settings - nice analog feel.
getNewSamples(); //in this mode, acquisitionBufferSize is set to FFTbinsM
for (int i = 0; i<FFTbinsM; i++)
{
vImag[i] = 0;
vReal[i] = double(samples[i]-adcRangeMax/2); //For now no DC offset.
}
//Note: have to compensate FFT magnitude scaling for window type, which affects this!
//FFT.Windowing(vReal, FFTbinsM, FFT_WIN_TYP_HANN, FFT_FORWARD); // Weight data. Hanning best resolution
FFT.Windowing(vReal, FFTbinsM, FFT_WIN_TYP_FLT_TOP, FFT_FORWARD); // Weight data Flat top best amplitude accuracy.
FFT.Compute(vReal, vImag, FFTbinsM, FFT_FORWARD); // Compute FFT
FFT.ComplexToMagnitude(vReal, vImag, FFTbinsM); // Compute magnitudes

for (int i = 0; i< (xPixels-1); i++)
{
samples[i] = int(vReal[map(i, 0, xPixels, 0, FFTbinsM/2)]*16/FFTbinsM); //Map samples onto display width.
}

//ntsc.clear(); //Here erase only the data that has been plotted - way faster than clearing the screen.

for (x = 0; x < xPixels-1; x++)
{
#ifdef connectDots
ntsc.line(x, map(samplesDisplayBuffer[x], 0, adcRangeMax, 0, yPixels-1), x+1, map(samplesDisplayBuffer[(x+1)], 0, adcRangeMax, 0, yPixels-1), BLACK); //Display first xPixels pixels of FFTbinsM
#else
ntsc.pixel(x, map(samplesDisplayBuffer[x], 0, adcRangeMax, 0, yPixels-1), BLACK);
#endif
}

for (x = 0; x < xPixels-1; x++)
{
samplesDisplayBuffer[x] = samples[x]; //Store the data as plotted to allow erasing directly (versus screen clear) later.
#ifdef connectDots
ntsc.line(x, map(samples[x], 0, adcRangeMax, 0, yPixels-1), x+1, map(samples[(x+1)], 0, adcRangeMax,0, yPixels-1), WHITE);
#else
ntsc.pixel(x, map(samples[x], 0, adcRangeMax, 0, yPixels-1), WHITE);
#endif
}

//This point in the code defines how long the trace is on the screen before blanking - want to minimize "blank" time...
//This matters more for long sampling times, when the acquisition time is significant. So... put the sampling delay here.
//Still need a minimum delay of about 10 ms, which is built into the subroutine.

//Optional screen gridlines
#ifdef gridOn //Optional screen gridlines
drawGrid(10, 8, 127);
#endif

ntsc.text ("VoidLoop NTSC FFT Analyzer", 50, (yPixels-16), WHITE);
for (int stringPointer = 0; stringPointer < 20; stringPointer++)
{
oldTemp[stringPointer] = temp[stringPointer]; //Save last printed variable to re-print in BLACK to clear before printing next variable
//Could use string.h library and memcpy, but could be off-putting for beginners.
}
dtostrf(FFT.MajorPeak(vReal, FFTbinsM, 1/(interruptPeriod*1E-6)), 10, 1, temp); //Find largest peak in FFT
ntsc.text(oldTemp, 200, (yPixels-32), BLACK);
ntsc.text(temp, 200, (yPixels-32), WHITE);
ntsc.text(" Hz", 280, (yPixels-32), WHITE);
//delay(100);
}

#else //Scope mode
FASTRUN void loop() {
//Here need to clear what was drawn - hoping to avoid clearing whole screen (glitches on monitor) by redrawing saved points, graphics and text in black.
//Not yet properly implemented.
/*
// Not working yet... Needs to handle triggered and untriggered conditions to "undraw all points..."
for (x = 0; x < xPixels-1; x++)
{
#ifdef connectDots
ntsc.line(x, map(samplesDisplayBuffer[x+triggerPointer-triggerDisplayPoint], 0, adcRangeMax, 0, yPixels-1), x+1, map(samplesDisplayBuffer[x+1+triggerPointer-triggerDisplayPoint], 0, adcRangeMax, 0, yPixels-1), BLACK);
#else
ntsc.pixel(x, map(samplesDisplayBuffer[x+triggerPointer-triggerDisplayPoint], 0, adcRangeMax, 0, yPixels-1), BLACK);
#endif //Needs to be modified to that it does not erase this if triggered = TRUE
ntsc.text("No trigger", 10, 15, BLACK);
}
*/

    
}
#endif


void getNewSamples()
{
    samplingTimer.begin(ISR, interruptPeriod);
    samplePointer = 0;
    sampleNow = true;
    delayMicroseconds(interruptPeriod*acquisitionBufferSize+100); //Enough time to capture the samples. At 10 us sample period, 320 samples takes about 3ms.
    samplingTimer.end();
}

void ISR()
{
    // samples[samplePointer] = analogRead(A0);
    // samplePointer = (samplePointer + 1) & (acquisitionBufferSize - 1);

    noInterrupts();
    if (sampleNow == true && samplePointer < acquisitionBufferSize)
    {
        samples[samplePointer] = analogRead(A0);
        samplePointer++;
    }
    else
    {
        sampleNow = false;
    }
    interrupts();
}

void drawGrid(int numVerticalLines, int numHorizontalLines, int intensity)
{
    for (int vertLine = 0; vertLine<=numVerticalLines; vertLine++)
    {
        ntsc.line(vertLine*(xPixels-1)/numVerticalLines, 0, vertLine*(xPixels-1)/numVerticalLines, yPixels, intensity);
    }
    for (int horLine = 0; horLine<=numHorizontalLines; horLine++)
    {
        ntsc.line(0, horLine*(yPixels-1)/numHorizontalLines, xPixels, horLine*(yPixels-1)/numHorizontalLines, intensity);
    }
}

void readPots()
{
    int potAverages = 20;
    float potScale = 1/float(potAverages);
    float potOldScale = 1-potScale;
    {
        pot0val = pot0val*potOldScale + potScale*analogRead(pot0);
        pot1val = pot1val*potOldScale + potScale*analogRead(pot1);
        pot2val = pot2val*potOldScale + potScale*analogRead(pot2);
        pot3val = pot3val*potOldScale + potScale*analogRead(pot3);
    }
}


void draw(){
    ntsc.clear();
    ntsc.text ("VoidLoop NTSC Oscilloscope", 50, (yPixels-16), WHITE);

    triggered = false;
    readPots(); //Read potentiometers to get settings - nice analog feel.
    trigLevelInt = pot0val;

    #ifdef gridOn //Optional screen gridlines
    drawGrid(10, 8, 31);
    #else
    ntsc.line(0, yPixels/2, xPixels, yPixels/2, 127); //If no grid, just draw a single horizontal line at middle of vertical range.
    #endif


    #ifdef internalADC
    interruptPeriod = pot1val+7; //Here set maximum sample rate with the added int being 1/fmax. Here 140 kSPS for 8 bit resolution.
    #else
    delTimePointer = delTime[int(map(analogRead(A6), 0, adcRangeMax, 0, 6))];
    #endif
    triggerDisplayPoint = map(pot2val, 0, adcRangeMax, 0, int(xPixels/2));
    triggerPointer = int(xPixels/2); // Skip over enough points to show pre-trigger samples.
    #ifdef potsNotInstalled
    trigLevelInt = int(adcRangeMax/2); //Trigger to 50% of ADC range
    interruptPeriod = 20; //Maximum sample rate
    delTimePointer = 0; //No delay
    triggerDisplayPoint = int(xPixels/2); //Trigger display point to mid-screen horizontal
    #endif

    //noInterrupts();
    // Executes in the idle, "untriggered" state
    while (triggered == false)
    {
        if (((samples[triggerPointer])<=trigLevelInt-hysteresisAmplitudeStep) && (samples[triggerPointer+hysteresisTimeStep]>trigLevelInt+hysteresisAmplitudeStep) && (samples[triggerPointer+2*hysteresisTimeStep]>=trigLevelInt+2*hysteresisAmplitudeStep) && (triggerPointer<=(acquisitionBufferSize-xPixels-1)))
        {
            triggered = true;
            break;
        }
        else if (triggerPointer < (acquisitionBufferSize-xPixels))
        {
            triggerPointer++;
        }
        else
        {
            break;
        }
    }

    // Executes when scope is triggered.
    if (triggered == true)
    {

        ntsc.line(triggerDisplayPoint, yPixels/2 - 10, triggerDisplayPoint, yPixels/2 + 10, WHITE); //Draw current trigger marker
        ntsc.line(triggerDisplayPoint-10, yPixels/2, triggerDisplayPoint+10, yPixels/2, WHITE); //Draw current trigger marker

        for (x = 0; x < xPixels-1; x++)
        {
            samplesDisplayBuffer[x] = samples[x];
        #ifdef connectDots
            ntsc.line(x, map(samples[x+triggerPointer-triggerDisplayPoint], 0, adcRangeMax, 0, yPixels-1), x+1, map(samples[x+1+triggerPointer-triggerDisplayPoint], 0, adcRangeMax, 0, yPixels-1),WHITE);
        #else
            ntsc.pixel(x, map(samples[x+triggerPointer-triggerDisplayPoint], 0, adcRangeMax, 0, yPixels-1), WHITE);
        #endif
        }

        getNewSamples(); //Use interrupt-driven acquisiton using analog input A0.
    }

    if (triggered == false)

    { //Draw centered trace...
        ntsc.line(0, yPixels/2, xPixels, yPixels/2, WHITE);
        //Still need to get new samples if not triggered, or never will be triggered!

        getNewSamples(); //Use interrupt-driven acquisiton using analog input A0.
        for (x = 0; x < xPixels-1; x++)
        {
            samplesDisplayBuffer[x] = samples[x]; //Make a copy of what is to be plotted to erase it later (instead of clearing whole screen).
            #ifdef connectDots
            ntsc.line(x, map(samples[x+triggerPointer-triggerDisplayPoint], 0, adcRangeMax, 0, yPixels-1), x+1, map(samples[x+1+triggerPointer-triggerDisplayPoint], 0, adcRangeMax, 0, yPixels-1),WHITE);
            #else
            ntsc.pixel(x, map(samples[x+triggerPointer-triggerDisplayPoint], 0, adcRangeMax, 0, yPixels-1), WHITE);
            #endif
            ntsc.text("No trigger", 10, 15, WHITE);
        }
    }
    //interrupts();

    ntsc.swap();
}