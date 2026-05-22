#include <APA102.h>
#include <arduino.h>

/*stuff to configure the APA102 addressable LEDs*/
const byte dataPin = 17;             // the clock pin, PIN_PC1
const byte clockPin = 16;            // the data pin, PIN_PC0
APA102<dataPin, clockPin> ledStrip;  // the "object" of the LED array
const byte ledCount = 12;            // how many LEDs are in this project?
rgb_color colors[ledCount];          // here's where the color info is stored
const byte brightness = 10;          // default brightness is 10, out of 31

/* this part handles the values coming in from the pots and jacks, along with
the low-resolution versions for LED colors*/
unsigned int circlePots[8];   // the array for the high-res value of the 8 pots in a circle
byte cirLEDs[8];              // the low-res version for the LEDs
unsigned int topRowPots[4];   // top four pots, high resolution value
byte topLEDs[4];              // low-res, for the LED colors
unsigned int parOneCV;        // these are the CV inputs for the top left and right
unsigned int parFourCV;       // parameters. par stands for parameter
int arPE7;                    // holds the  high-res value of CLOCK pot
int arPD7;                    // holds the high-res value of ENVELOPE pot
byte parOneLED;               // the low resolution color control for knob 1
byte parFourLED;              // low res color for knob 4
byte targetHue[12];           // holds the values for LED hues
byte targetSat[12];           // saturation values
byte targetVal[12];           // holds brightness values
bool potFourButton;           // button under top right pot
bool shiftButton;             // mechanical keyswitch in the middle, for SHIFT
byte lastModeState;           // for managing clock button mode
unsigned long debounceTimer;  // debounce timer
const byte debounce = 15;     // some millisecond amount of debounce
bool checkPpqnCount = false;
bool clockBrightToggle = true;
byte CFC;
byte clockDivider;
byte oldClockDivider;
const byte hysteresisWindow = 50;
int oldClockPotValue;



byte envelopeMode;                 // attack, decay, sustain, release
bool envPressed;                   // button pressed?
bool oldEnvPressed = true;         // was it pressed before? starts out HIGH because INPUT_PULLUP
unsigned long envPressedDebounce;  // debounce
bool envExtInt;                    // if something's plugged in to the envelope jack, envelope those jacks.
byte attack;                       // the attack parameter
byte decay;                        // decay parameter
byte sustain;                      // sustain
byte release;                      // and release
byte oldAttackValue;               //
byte oldDecayValue;                //
byte oldSustainValue;              //
byte oldReleaseValue;              //
byte playEnvTracker;               // keep track of what part of the ADSR is playing
byte LEDEnvTracker;                // only plays through the LED under the envelope pot
byte LEDenvelopeValue;             // the LED section gets its own variable
byte envelopeValue;                // okay global value I guess
unsigned long envelopeTimer;       // times the timing-related parts of the envelope
unsigned long LEDEnvelopeTimer;    // just for the LED part
unsigned long pot3Flash;           // for flashing the envelope pot light

/* variables for picking up pot turns */
unsigned int lastClockPotValue;  // where the pot was when mode last changed
unsigned int storedClockRate;    // the CCMP value saved when mode changed
bool clockPotPickedUp = false;   // has the pot found its pickup point yet?
bool envPotPickedUp = false;     // for the envelope pot
int Ediff;                       // difference for the envelope pot pickup




/* here's the variables we'll use for the clock*/
volatile byte clockTicks = 0;  // this is for the main timer
byte ppqnCounter;              // pulse per quarter note, MIDI uses 24
byte ppqnPerStep = 24;         // MIDI uses this, so you can get 8ths or 16ths by dividing by 2 or 4
byte currentStep;              // keeps track of what step the sequencer is on
byte numSteps = 8;             // basic circle sequence, a nice default, but can be RADICALLY changed :D
unsigned long stepFlash;       // timer for white flash on step change
unsigned long pot1Flash;       // for flashing the clock pot

/*Modes and Sequence Stuff*/





unsigned long aTimer;  // times the analogRead functions to 200 times per second



void setup() {
  /*INPUTS*/
  pinMode(PIN_PD4, INPUT);  // circlePot 1 (zero, don't forget it's address number zero in the array)
  pinMode(PIN_PD5, INPUT);  // circlePot 2 (these are the circlePot input pins) They default to INPUT,
  pinMode(PIN_PE0, INPUT);  // circlePOt 3 but I'm just being explicit for readability
  pinMode(PIN_PE1, INPUT);  // circlePot 4
  pinMode(PIN_PE4, INPUT);  // circlePot 5
  pinMode(PIN_PE3, INPUT);  // circlePot 6
  pinMode(PIN_PE2, INPUT);  // circlePot 7
  pinMode(PIN_PE5, INPUT);  // circlePot 8

  pinMode(PIN_PD0, INPUT);  // topRowPot 1 (again, remember that it's at address zero)
  pinMode(PIN_PD1, INPUT);  // topRowPot 2
  pinMode(PIN_PD2, INPUT);  // topRowPot 3
  pinMode(PIN_PD3, INPUT);  // topRowPot 4 (this is the gate input, so let's do PORT reads)
  pinMode(PIN_PE7, INPUT);  // top row ACTUAL pot 1, the CV and pot are separate
  pinMode(PIN_PD7, INPUT);  // top row ACTUAL pot 4,

  pinMode(PIN_PB1, INPUT_PULLUP);  // clicky button under pot 1
  pinMode(PIN_PB2, INPUT_PULLUP);  // clicky button under pot 4
  pinMode(PIN_PA7, INPUT_PULLUP);  // mechanical keyboard switch for SHIFT

  /*OUTPUTS*/
  // pinMode(PIN_PD6, OUTPUT);  // main high-quality 10-bit analog out
  pinMode(PIN_PA3, OUTPUT);  // filtered PWM analog out 2
  pinMode(PIN_PA4, OUTPUT);  // filtered PWM out 3
  pinMode(PIN_PA5, OUTPUT);  // filtered PWM out 4
                             // okay that's the top row of outputs
  pinMode(PIN_PF2, OUTPUT);  // gate or trigger out 1
  pinMode(PIN_PF3, OUTPUT);  // gate or trigger out 2
  pinMode(PIN_PF4, OUTPUT);  // gate or trigger out 3
  pinMode(PIN_PF5, OUTPUT);  // gate or trigger out 4
                             // now the LED output for the shift key
  pinMode(PIN_PA6, OUTPUT);  // shift key LED

  analogSampleDuration(8);   // gives the ADC caps a bit of extra time to load
  analogReadResolution(12);  // the AVR128DB has 12 bit analog read resolution! That's 4096 values!!! May as well use it


  VREF.DAC0REF = 0x05; // writing to this register enables VDD voltage reference for the DAC
  DAC0.CTRLA = DAC_ENABLE_bm | DAC_OUTEN_bm; // and this enables that DAC
  
  
  VREF.ADC0REF = VREF_REFSEL_VDD_gc;          // this WEIRD line selects VCC (5V) as the voltage reference for analog reads
  // I guess DXCore has a bug where it writes analogReference(VDD) to the wrong register?
  setupClock();  // starting the clock in the setup routine
}

void setupClock() {
  /* we're gonna use the timer called TCB0 to run the sequence clock we're working with a variable PPQN (pulses per quarter note) to get different divisions
  of the clock that might be coming in from a MIDI device (24PPQN) or something else (4PPQN?)
  Yeah. 
  We're using low-level code to use TCB0, one of the chip's timers in "Periodic Interrupt Mode".
  It counts up to a value (CCMP), fires an interrupt, resets to zero, and repeats. 
  TCB0's max count is OXFFFF (65,535) so we have to do mathhhh ugh
  
  At 46,875HzHz (24MHz cpu divided by nice-round-number, 512)
  40 BPM  × 24 PPQN = 16Hz  → CCMP = (46875 / 16)  - 1 = 2929
  120 BPM × 24 PPQN = 48Hz  → CCMP = (46875 / 48)  - 1 = 976
  240 BPM × 24 PPQN = 96Hz  → CCMP = (46875 / 96)  - 1 = 487 
  Those numbers fit comfortably into 16 bits, with fine resolution
  
  We're using the prescaler for TCA1 to divide down TCB0. I hope this works?
  TCB0 has a /2 divider, TCA1 has a /256 divider, so we can get all the way from 24HMz to about 47KHz.
  It would be cool to just prescale by bitshifting, throwing away the least significant bits,
  but you can't do that with a hardware clock. You need to do it in hardware, 
  like a CD4017 decade counter or whatever. 
   */


  TCA1.SINGLE.CTRLA = TCA_SINGLE_CLKSEL_DIV256_gc  // okay, setting up TCA1 as prescaler number one:
                      | TCA_SINGLE_ENABLE_bm;      // why is this on two lines? Weird. A bitmath thing?
  TCB0.CTRLA = TCB_CLKSEL_TCA1_gc;                 // then say "HEY! TCB0!!! Use TCA1 as the clock source!" Also, divide by 2
  TCB0.CTRLB = TCB_CNTMODE_INT_gc;                 // okay, turning on "Periodic Interrupt" mode. Timer counts to CCMP, fires the interrupt, resets
  TCB0.CCMP = 976;                                 // initial tempo = 120BPM, you know, for house music
                                                   // CCMP stands for Compare/Capture Match Point, by the way
  TCB0.INTCTRL = TCB_CAPT_bm;                      // enabling the interrupt
                                                   // we're gonna enable *just* the "enable" bit here, to enable TCB0
  TCB0.CTRLA |= TCB_ENABLE_bm;                     // _bm stands for bit mask. The |= just writes that one bit, leaving the rest of the byte alone
  sei();                                           /* "sei" stands for Set Enable Interrupts. The old "enableInterrupts()" is not a part of DXCore, so gotta use this instead
  the converse is cli() which replaces "disableInterrupts()" and means "Clear Interrupts". So far, the interrupt just writes one boolean value
  so I don't know if we'll need to disable interrupts aka cli();*/
}

/* okay here's the Interrupt Service Routine */

ISR(TCB0_INT_vect) {
  //gotta clear the interrupt as soon as the interrupt fires!
  TCB0.INTFLAGS = TCB_CAPT_bm;
  clockTicks++;
}

void loop() {
  lewp();
}
