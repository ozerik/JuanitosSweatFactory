#include <APA102.h>
#include <arduino.h>
#include <util/atomic.h>
#include <EEPROM.h>

/*EEPROM byte addresses const section*/
const byte RANGE_LSB = 0;
const byte RANGE_MSB = 1;
const byte QUANTMODE = 2;
const byte BRIGHTNESS = 3;

bool CVRAdjust = false;  // turns on adjustment for config pots
bool QMAdjust = false;   // for quantize
bool BRAdjust = false;   // for bright
int CVRCheck;            // stores original pot values to avoid INSTANT SUDDEN ADJUST!!! for range
int QMCheck;             // q
int BRCheck;             // br

/*stuff to configure the APA102 addressable LEDs*/
#define DATAPIN 17                    // the clock pin, PIN_PC1
#define CLOCKPIN 16                   // the data pin, PIN_PC0
APA102<DATAPIN, CLOCKPIN > ledStrip;  // the "object" of the LED array
#define LEDCOUNT 13                   // how many LEDs are in this project?
rgb_color colors[LEDCOUNT];           // here's where the color info is stored
byte brightness = 10;                 // default brightness is 10, out of 31
#define JITTER_DEFEATER 70            // hysteresis conqueror -- how much the pot has to turn into the new value to count?

volatile unsigned int pwmTarget10bit = 0;  // shared variable that holds the whole value for the dithering PWM thing

byte mode;     // specifically mode of sequence, from "linear" to cylon to patterns
byte cylonFix; // sigh. Just so cylon mode will flash the right way
int metaMode;  // modifies the mode of the sequences

const int shortGlide = 300;
const int longGlide = 1500;






/* this part handles the values coming in from the pots and jacks, along with
the low-resolution versions for LED colors*/
// int circlePots[8];            // the array for the high-res value of the 8 pots in a circle
byte cirLEDs[8];              // the low-res version for the LEDs
unsigned int topRowPots[4];   // top four pots, high resolution value
byte topLEDs[4];              // low-res, for the LED colors
unsigned int parOneCV;        // these are the CV inputs for the top left and right
unsigned int parFourCV;       // parameters. par stands for parameter
int arPE7;                    // holds the  high-res value of CLOCK pot
int arPD7;                    // holds the high-res value of ENVELOPE pot
byte targetHue[13];           // holds the values for LED hues
byte targetSat[13];           // saturation values
byte targetVal[13];           // holds brightness values
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
const byte hysteresisWindow = 200;
int oldClockPotValue;
int oldEnvPotValue;  // for track forking, to make module play recorded track without being altered by SEQUENCER




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
bool loopStart = false;            // tracker for the start of the loop. Just for trigger output
unsigned long envelopeTimer;       // times the timing-related parts of the envelope
unsigned long LEDEnvelopeTimer;    // just for the LED part
unsigned long pot3Flash;           // for flashing the envelope pot light

/* variables for picking up pot turns */
unsigned int lastClockPotValue;  // where the pot was when mode last changed
unsigned int storedClockRate;    // the CCMP value saved when mode changed
bool clockPotPickedUp = true;    // has the pot found its pickup point yet?
bool envPotPickedUp = false;     // for the envelope pot
int Ediff;                       // difference for the envelope pot pickup

/* here's the variables we'll use for the clock*/
volatile byte clockTicks = 0;    // this is for the main timer
volatile byte ppqnCounter;       // pulse per quarter note, MIDI uses 24
volatile byte ppqnPerStep = 24;  // MIDI uses this, so you can get 8ths or 16ths by dividing by 2 or 4
int currentStep;                 // keeps track of what step the sequencer is on
byte numSteps = 8;               // basic circle sequence, a nice default, but can be RADICALLY changed :D
unsigned long stepFlash;         // timer for white flash on step change
unsigned long pot1Flash;         // for flashing the clock pot
bool doStepSelection = false;    // this says HEY METAMODE KNOB EQUALS STEP SELECTOR NOW
#define TRIGGER_TIME 8           // how long trigger stays high???? 8ms is default
bool doFlash; // for the flashing stuff

/*Modes and Sequence Stuff*/


byte countDown = 8;     // global variables to reset
bool UP = true;         // because when they were local static variables
byte patternCount = 0;  // the modes would get messed up


unsigned long aTimer;  // times the analogRead functions to 200 times per second



bool shift;
unsigned long shiftPressedDebounce;  // the shift key gets its own debounce timer!
byte shiftMode;                      // shift button modes, one or two?

const byte countsPerOctave = 100;         // on 2nd prototype 100 "values" goes up one volt and octave. On 3rd prototype this will probably change to 128 so math is faster
byte quantSnapTable[9][countsPerOctave];  // my very first two dimensional array!!!
byte qMode = 0;                           // which quantize mode?
int countsPerSemitone;                    // Some scales have fewer notes. Just teh way it is
const uint16_t scaleTable[9] = {
  0b111111111111,  // qMode 0, turns off quantization
  0b111111111111,  // qMode 1, chromatic
  0b101011010101,  // qMode 2: major
  0b101101011001,  // qMode 3: minor
  0b101010010101,  // qMode 4: major pentatonic
  0b100101001001,  // qMode 5: minor pentatonic
  0b100101011001,  // qMode 6: harmonic minor
  0b010101010101,  // qMode 7: whole tone
  0b100101101001,  // qMode 8: blues
};

int TcirclePots[8];             // temp circle pot value
bool circlePotValueChanged[8];  // if the shift key is pressed AND something else is done, well, that means don't change tracker position
int slewValue[8];               // HERE'S THE VARIABLE that contains slews for each circle pot
int oldPotsValue[8];            // old pots value, for setting the circlePotValueChanged value
bool potNeedCatch[8];           // all 1s to keep all the pots from flashing on powerup
unsigned long CPotTimer[8];     // this is for FLASHYFLASH when the circlepots get back into being latched
int circlePots[8];              // real time circle pot readings
int currentCV;                  // main current CV value, ten bit number, 0 through 1023
int rangedCV;                   // CV adjusted to current CVRange, to be sent to DAC and recorded to the forkable sequence
int CVRange = 307;              // changes how high the final output CV will go
unsigned long glideTimer;       // for glide, the timer for it :P
int targetCV;                   // where we heading toward?
int oldCV; // checks for a large enough CV jump, then triggers a trigger
#define CVJUMP 10 // okay ten? is that enough to make triggers cool?
bool isGliding = false;


bool record = true;                    // sets the record flag
unsigned int recordSteps;              // tracks where the recording is
unsigned int recorded[1536];           // should be long enough? 64 steps of values. Records one value per peak, holds the CV and the gate
byte recordedB[1536];                  // 64 step variable dedicated to the envelope
bool gateForRecord;                    // gate value to record into array
unsigned int recordBOC = 500;          // recorded Beginning of Cycle for record BOC
int arPD7TEMP;                         // holds the value of arPD7, the envelope pot
bool ignoreClockPot = false;           // ignore clock pot? NO!!! Except yes, in tapTempo()
unsigned int newCCMP;                  // new compare contrast (???) match point for the timer
byte tapCount;                         // counts taps in tapTempo()
bool GS;                               // gate state! for recording the gate state
int arPD3;                             // sure, why not, analogRead(PIN_PD3) is this value
unsigned int BOCtrack;                 // maybe this will count how many ACTUAL pulses between clock resets???
volatile byte internalPulseCount = 0;  // counts up to like 6 or 24 oor 12, depending on shift-clockPot setting?
bool firstClock = true;                // watches for a first clock
volatile bool run = true;              // stops internal clock when external clock exists
unsigned long doubleClickTimer;        // times a double-click of shift button for HOLD and ENVELOPE (whee)
bool doubleClick;                      // tracks the doubleClick state
byte shiftStep;                        // for when doubleShift is active



volatile bool extClock = false;           // flags that there's an external clock
volatile unsigned long lastExtClock;      // time of the last external clock tick or pulse
unsigned long lastExtPeriod;              // the previous period of time to get sent to the internal clock timer? I think?
volatile unsigned long extNow;            // the exact time the external clock came to the module
volatile unsigned long extPeriod;         // the amount of time between last two external clocks
volatile unsigned long extDividedPeriod;  // the amount of micros() we have to wait between clockTicks++
byte tickTrack;                           // counter to see how many ticks have happened! It'll roll over at like 11 or something
volatile byte tickTotal;                  // how many ticks between extClocks??????
volatile byte extClockMode;               // um, this is for extClock, 24ppqn? 2 peaks per sequence advance? 1 peak per sequence advance?
bool extClockFirst = true;                // this is for the main timer reads
unsigned long extNowReal;                 // the variable the main lewp() uses to watch for when to do the recordSTeps++

void setup() {
  /*DELAY to allow power supply caps to charge*/
  delay(500);          // caps charging
  buildQuantTables();  // for quantization! Make all those notes snap to the tables this is gonna build

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

  pinMode(PIN_PB1, INPUT_PULLUP);                                                  // clicky button under pot 1
  pinMode(PIN_PB2, INPUT_PULLUP);                                                  // clicky button under pot 4
  pinMode(PIN_PA7, INPUT_PULLUP);                                                  // mechanical keyboard switch for SHIFT
  while (digitalRead(PIN_PA7) == LOW) digitalWrite(PIN_PF2, (millis() >> 8) & 1);  // this is to prevent more BRICKINGS. Hold shift to pause everything so UPDI can initialize
                                                                                   // OOOORRRRRRRR ^^^^^^^^^^ reset the module while holding shift, that'll pause the module so UPDI can initialize
  // read the relevant values that get configured in config mode (shiftMode == 1)
  byte lo = EEPROM.read(RANGE_LSB);
  byte hi = EEPROM.read(RANGE_MSB);
  int stored = (hi << 8) | lo;
  if (stored < 102 || stored > 1023) {
    CVRange = 307;
  } else {
    CVRange = stored;
  }
  qMode = EEPROM.read(QUANTMODE);
  brightness = EEPROM.read(BRIGHTNESS);
  if (brightness < 1 || brightness > 31) brightness = 10;

  /*OUTPUTS*/
  // HARDWARE INTERRUPT!!! This is for the clock input!
  PORTD.PIN0CTRL = PORT_ISC_RISING_gc;  // interrupt on rising edge only

  // CHATGPT and Claude helped with this part. Thanks, robots
  PORTMUX.TCAROUTEA = PORTMUX_TCA0_PORTA_gc;
  TCA0.SPLIT.CTRLA = 0;                       // stop timer
  TCA0.SPLIT.CTRLB = TCA_SPLIT_HCMP0EN_bm     // PA3 = WO3 = H timer CMP0
                     | TCA_SPLIT_HCMP1EN_bm   // PA4 = WO4 = H timer CMP1
                     | TCA_SPLIT_HCMP2EN_bm;  // PA5 = WO5 = H timer CMP2

  TCA0.SPLIT.HPER = 255;  // H timer period

  TCA0.SPLIT.HCMP0 = 0;  // PA3
  TCA0.SPLIT.HCMP1 = 0;  // PA4
  TCA0.SPLIT.HCMP2 = 0;  // PA5

  TCA0.SPLIT.CTRLA = TCA_SPLIT_CLKSEL_DIV16_gc
                     | TCA_SPLIT_ENABLE_bm;

  pinMode(PIN_PA3, OUTPUT);  // filtered PWM analog out 2
  pinMode(PIN_PA4, OUTPUT);  // filtered PWM analog out 3
  pinMode(PIN_PA5, OUTPUT);  // filtered PWM out 4

  // okay that's the top row of outputs
  pinMode(PIN_PF2, OUTPUT);  // gate or trigger out 1
  pinMode(PIN_PF3, OUTPUT);  // gate or trigger out 2
  pinMode(PIN_PF4, OUTPUT);  // gate or trigger out 3
  pinMode(PIN_PF5, OUTPUT);  // gate or trigger out 4
                             // now the LED output for the shift key
  pinMode(PIN_PA6, OUTPUT);  // shift key LED

  ADC0.CTRLB = ADC_SAMPNUM_ACC16_gc;  // hardware-accumulate 16 samples
  ADC0.SAMPCTRL = 16;                 // longer sample window — pots are high-impedance sources, this matters more than people expect
  // ADC0.CTRLA = ADC_ENABLE_bm | ADC_RESSEL_12BIT_gc;
  analogReadResolution(12);  // the AVR128DB has 12 bit analog read resolution! That's 4096 values!!! May as well use it


  VREF.DAC0REF = 0x05;                        // writing to this register enables VDD voltage reference for the DAC
  DAC0.CTRLA = DAC_ENABLE_bm | DAC_OUTEN_bm;  // and this enables that DAC
  VREF.ADC0REF = VREF_REFSEL_VDD_gc;          // this WEIRD line selects VCC (5V) as the voltage reference for analog reads
  // I guess DXCore has a bug where it writes analogReference(VDD) to the wrong register?

  setupClock();                             // starting the clock in the setup routine
  TCA0.SPLIT.INTCTRL |= TCA_SPLIT_HUNF_bm;  // this line enables high-side underflow interrupts for the dithering
  // Serial2.begin(9600);                      //Enabling this requires reset-hold-shift routine to get the module to flash. Boo.
  //
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
  sei();                                           /* "sei" stands for Set Interrupts. The old "enableInterrupts()" is not a part of DxCore, so gotta use this instead
  the converse is cli() which replaces "disableInterrupts()" and means "Clear Interrupts". So far, the interrupt just writes one boolean value
  so I don't know if we'll need to disable interrupts aka cli();*/
}


ISR(TCB0_INT_vect) {            // internal CLOCK timer
  TCB0.INTFLAGS = TCB_CAPT_bm;  // clear interrupt flag of course
  clockTicks++;                 // internal mode, fire forever
}



ISR(PORTD_PORT_vect) {             // this is the external clock Interrupt SErvice Routine
  PORTD.INTFLAGS = PIN0_bm;        // reset that bad mamma ISR flag for real
  extClock = true;                 // it's true. The extClock is happning
  if (firstClock == true) {        // first clock just dropped. DO SOMETHING
    firstClock = false;            // set this to false do it doesn't run anymore
    TCB0.CTRLA &= ~TCB_ENABLE_bm;  // and stop the internal timer
  }                                //

  clockTicks++;
}

ISR(TCA0_HUNF_vect) {
  static byte dither = 0;
  uint16_t value10bit = pwmTarget10bit;                                 // in the writePWM() function, it uses ATOMIC_BLOCK which sounds pretty damn serious
  byte high = value10bit >> 2;                                          // the most important 8 bits of the value being written to the PWM pin
  if (high < 255) {                                                     // prevents dithering if the value is all the way up
    dither += value10bit & 0x03;                                        // adds the 2 least-significant-bits to value dither
    TCA0.SPLIT.HCMP0 = (dither >= 4) ? (dither -= 4, high + 1) : high;  // watches for dither to get up to 4, if it is? write high + 1. If not, just write high
  } else TCA0.SPLIT.HCMP0 = 255;                                        // just write max value, 255, don't try to dither "up"
  TCA0.SPLIT.INTFLAGS = TCA_SPLIT_HUNF_bm;                              // reset the interrrupt
}


bool inScale(uint16_t mask, int absPos) {
  int s = ((absPos % 12) + 12) % 12;  // proper mod, handles negative absPos too
  return mask & (1 << s);
}

void buildQuantTables() {
  for (byte mode = 1; mode < 9; mode++) {
    uint16_t mask = scaleTable[mode];
    for (byte remainder = 0; remainder < countsPerOctave; remainder++) {
      float semitonePos = (float)remainder * 12.0f / countsPerOctave;
      int nearest = round(semitonePos);  // 0..12

      int best = nearest;
      for (byte offset = 0; offset <= 6; offset++) {
        int up = nearest + offset;  // NOT wrapped -- can legitimately exceed 11
        int down = nearest - offset;
        if (inScale(mask, up)) {
          best = up;
          break;
        }
        if (inScale(mask, down)) {
          best = down;
          break;
        }
      }

      int octaveDelta = (best >= 12) ? 1 : 0;  // root is in every one of your scales, so best never needs to go negative
      int semitoneInOctave = best - (octaveDelta * 12);
      int tableVal = (int)round((float)semitoneInOctave * countsPerOctave / 12.0f) + (octaveDelta * countsPerOctave);
      quantSnapTable[mode][remainder] = (byte)tableVal;
    }
  }
}



void loop() {

  lewp();
}