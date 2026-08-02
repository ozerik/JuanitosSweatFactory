# Juanito's Sweat Factory - Hardware Reference

Juanito's Sweat Factory is an 8-bit microprocessor sitting at the center of a web of potentiometers, and spits out a couple analog voltages, and several digital voltages.

It runs on an **AVR128DB64** chip.

## The Chip

| Spec | Value |
|---|---|
| Core | 8-bit AVR 24MHz internal clock |
| Flash | 128 KB |
| SRAM | 16 KB |
| EEPROM | 512 B |
| Package | TQFP-64 |
| Programming | UPDI but there's a programmer built in to the module |

## Pinout 

Okay this is **3rd prototype** hardware, so for example pin 62 has jumper pins to make it easy for me to connect that pin or not.


| Pin | MCU Pin | Function |
|-----:|---------|----------|
| 1 | PA3 | PWM output, bottom row position 1 |
| 2 | PA4 | PWM output, top row position 2 |
| 3 | PA5 | PWM output, bottom row position 2 |
| 4 | PA6 | — |
| 5 | PA7 | Shift key |
| 6 | VDD | +5V |
| 7 | GND | Ground |
| 8 | PB0 | — |
| 9 | PB1 | Button behind Top knob 1 |
| 10 | PB2 | Button behind Top knob 4 |
| 11 | PB3 | — |
| 12 | PB4 | — |
| 13 | PB5 | — |
| 14 | PB6 | — |
| 15 | PB7 | — |
| 16 | PC0 | APA102 LED Clock |
| 17 | PC1 | APA102 LED Data |
| 18 | PC2 | — |
| 19 | PC3 | — |
| 20 | VDDIO2 | +5V |
| 21 | GND | Ground |
| 22 | PC4 | — |
| 23 | PC5 | — |
| 24 | PC6 | — |
| 25 | PC7 | — |
| 26 | PD0 | Top knob 1 |
| 27 | PD1 | Top knob 2 |
| 28 | PD2 | Top knob 3 |
| 29 | PD3 | Top knob 4 |
| 30 | PD4 | Circle knob 1 (12 o'clock position) |
| 31 | PD5 | Circle knob 2 |
| 32 | PD6 | Hardware DAC out |
| 33 | PD7 | input, right near Top knob 4 |
| 34 | AVDD | +5V (electrically connected to other VDD) |
| 35 | GND | Ground |
| 36 | PE0 | Circle knob 3 |
| 37 | PE1 | Circle knob 4 |
| 38 | PE2 | Circle knob 7 |
| 39 | PE3 | Circle knob 6 |
| 40 | PE4 | Circle knob 5 |
| 41 | PE5 | Circle knob 8 |
| 42 | PE6 | — |
| 43 | PE7 | input, right near Top knob 1 |
| 44 | PF0 (XTAL32K1) | Serial Tx out to USB for debugging |
| 45 | PF1 (XTAL32K2) | — |
| 46 | PF2 | Gate/trigger out, top row position 4 |
| 47 | PF3 | Gate/trigger out, bottom row position 4 |
| 48 | PF4 | Gate/trigger out, bottom row position 3 |
| 49 | PF5 | Gate/trigger out, top row position 3 |
| 50 | PF6 | Reset (active low, tied high with 12K to +5V) |
| 51 | UPDI | Programming |
| 52 | PG0 | — |
| 53 | PG1 | — |
| 54 | PG2 | — |
| 55 | PG3 | — |
| 56 | VDD | +5V |
| 57 | GND | Ground |
| 58 | PG4 | — |
| 59 | PG5 | — |
| 60 | PG6 | — |
| 61 | PG7 | — |
| 62 | PA0 (XTALHF1) | UPDI? there's jumper pads to connect this to UPDI |
| 63 | PA1 (XTALHF2) | — |
| 64 | PA2 | — |

## Flashing

This project uses a CH340E USB programmer. All you need to program it? A USB-C cable and a computer to connect it to.

This is what works for me, an unserious coder who uses the Arduino IDE and writes comments on every line, panicked that future-me won't remember what the code does (happens all the time)

I'm using the Arduino IDE. The board manager is DxCore by Spence Konde.
Chip is AVR128DB64
Clock speed: 24MHz internal
Programmer: SerialUPDI
Default values (as configured by my setup) work for the rest of the options.

It has never been required, in my case, to use a reset button. It **is** useful for me, since enabling serial communications breaks UPDI (at least with prototype 2) and it's necessary to stop serial communications when initializing UPDI flashing.

As I write this, my code has two ways to stop serial communications:
1. reset or power-cycle the module while holding the shift key. Hold it until the the upload initializes
2. while the module is running, simulatneously turn pot 1 all the way down, and press shift and button-under-knob-4. This executes `Serial2.end();` and also completely hangs the module for some reason. But that's okay, you're going to flash it anyway.