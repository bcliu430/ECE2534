////////////////////////////////////////////////////////////////////////////////////
// ECE 2534:        HW3
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
    int countdebounce = 0;
    char buf[17];               // Temporary string for OLED display
    unsigned int timeCount = 0; // Elapsed time since initialization of program
    unsigned int timer2_current=0, timer2_previous=0;
    
    // Initialize GPIO for BTN1 and LED1
    //TRISGSET = 0x40;     // For BTN1: configure PortG bit for input
   

    // Initialize PmodOLED, also Timer1 and SPI1
    DelayInit();
    OledInit();

    // Set up Timer2 to roll over every 100 ms
    Timer2Init();

    // Send a welcome message to the OLED display
    OledClearBuffer();


    // Main processing loop
    while (1)
    {
             
        // Poll Timer 2, and update display if Timer 2 has rolled over
        timer2_current = ReadTimer2();
        if (timer2_previous > timer2_current)
        {
            if (timeCount <= 50){
            // Timer2 has rolled over, so increment count of elapsed time
            timeCount++;
            // Display elapsed time in units of seconds, with decimal point
            OledSetCursor(0, 1);          // col 0 row 1 of display
            OledPutString("Homework 3");
            OledSetCursor(0, 2);          // col 0 row 1 of display
            OledUpdate();
            }
            else
                    OledClearBuffer();
            if(timeCount>50){
                
                timeCount++;
                OledSetCursor(0,1);
                OledPutString("w/o");
                OledSetCursor(0,3);
                OledPutString("w");
                OledUpdate();

            }
                
            
        }
        timer2_previous = timer2_current;
        
        if (PORTG &(1<<6)){
            countdebounce++;
        
        }
        
   } // end while   
    
   return EXIT_SUCCESS;           // This return should never occur
} // end main  

