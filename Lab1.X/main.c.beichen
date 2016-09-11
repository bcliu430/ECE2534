////////////////////////////////////////////////////////////////////////////////////
// ECE 2534:        Lab 1
// File name:       main.c
// Description:     Test the Digilent board by writing a short message to the OLED.
//                  Also display a counter that updates every 100 milliseconds.
// Resources:       main.c uses Timer2 to measure elapsed time.
//					delay.c uses Timer1 to provide delays with increments of 1 ms.
//					PmodOLED.c uses SPI1 for communication with the OLED.
// Written by:      Patterson, Plassmann, Abbott
// Last modified:   8/30/2016

#include <stdio.h>                      // for sprintf()
#include <plib.h>                       // Peripheral Library
#include "PmodOLED.h"
#include "OledChar.h"
#include "OledGrph.h"
#include "delay.h"

// Digilent board configuration
#pragma config ICESEL       = ICS_PGx1  // ICE/ICD Comm Channel Select
#pragma config DEBUG        = OFF       // Debugger Disabled for Starter Kit
#pragma config FNOSC        = PRIPLL	// Oscillator selection
#pragma config POSCMOD      = XT	    // Primary oscillator mode
#pragma config FPLLIDIV     = DIV_2	    // PLL input divider
#pragma config FPLLMUL      = MUL_20	// PLL multiplier
#pragma config FPLLODIV     = DIV_1	    // PLL output divider
#pragma config FPBDIV       = DIV_8	    // Peripheral bus clock divider
#pragma config FSOSCEN      = OFF	    // Secondary oscillator enable

// Initialize Timer2 so that it rolls over 10 times per second
void Timer2Init() 
{
    // The period of Timer 2 is (16 * 62500)/(10 MHz) = 100 ms (freq = 10 Hz)
    OpenTimer2(T2_ON | T2_IDLE_CON | T2_SOURCE_INT | T2_PS_1_16 | T2_GATE_OFF, 62499);
    return;
}

int main()
{
    char buf[17];               // Temporary string for OLED display
    unsigned int timeCount = 0; // Elapsed time since initialization of program
    unsigned int timer2_current=0, timer2_previous=0;
    
    // Initialize GPIO for BTN1 and LED1
    TRISGSET = 0xC0;     // For BTN 1 and 2: set pin 6 and 7 to 1 as input 
    TRISGCLR = 0xF000;   // For LEDs: configure PortG pin for output
    ODCGCLR  = 0xF000;   // For LEDs: configure as normal output (not open drain)

    // Initialize PmodOLED, also Timer1 and SPI1
    DelayInit();
    OledInit();

    // Set up Timer2 to roll over every 100 ms
    Timer2Init();

    // Send a welcome message to the OLED display
    OledClearBuffer();
    OledSetCursor(0, 0);          // upper-left corner of display
    OledPutString("ECE 2534");
    OledSetCursor(0, 2);          // column 0, row 2 of display
    OledPutString("Beichen Liu");
    OledUpdate();

    // Main processing loop
    while (1)
    {
        // Poll BTN1, and update LED1 to indicate the state of BTN1
        if(      (PORTG & (1 << 6)) && ( PORTG & (1 << 7)) )// Is BTN1 and BTN2 being pressed?
            LATGCLR = 0xF000;
        
        else if((PORTG & (1 << 6)) || ( PORTG & (1 << 7)) )  // Is BTN1 xor BTN2 being pressed?
            LATGSET = 0xF000;    // LEDs on 
         
        else 
            LATGCLR = 0xF000;
        
        // Poll Timer 2, and update display if Timer 2 has rolled over
        timer2_current = ReadTimer2();
        if (timer2_previous > timer2_current)
        {
            // Timer2 has rolled over, so increment count of elapsed time
            timeCount++;
            
            // Display elapsed time in units of seconds, with decimal point
            sprintf(buf, "%14d.%d", timeCount/10, timeCount%10);
            OledSetCursor(0, 3);
            OledPutString(buf);
            OledUpdate();
        }
        timer2_previous = timer2_current;
        
   } // end while
    
   return EXIT_SUCCESS;           // This return should never occur
} // end main  

