////////////////////////////////////////////////////////////////////////////////////
// ECE 2534:        HW3
// File name:       main.c
// Description:     Test the Digilent board by writing a short message to the OLED.
//                  Also display a counter that updates every 100 milliseconds.
// Resources:       main.c uses Timer2 to measure elapsed time.
//					delay.c uses Timer1 to provide delays with increments of 1 ms.
//					PmodOLED.c uses SPI1 for communication with the OLED.
// Written by:      Beichen Liu
// Last modified:   9/26/2016

#include <stdio.h>                      // for sprintf()
#include <stdbool.h>
#include <string.h>
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

enum state {init, LED1, LED2, LED3, LED4};
enum state systemState;

void initialization();
bool getInputBTN1();
bool getInputBTN2();
int getInput();
bool Timer2Input();

int main() {
    // Initialize GPIO for BTN1 and LED1
    TRISGSET = 0xC0; // For BTN1: configure PortG bit for input
    TRISGCLR = 0x4000; // For LED1: configure PortG pin for output
    ODCGCLR = 0xF000; // For LED1: configure as normal output (not open drain)

    void Timer2Init();

    int next;
    initialization();
    while (1) {
        
        switch (systemState) {
            case init:
                if (getInput() == 0)
                    initialization();
                    systemState = LED1;
                break;
 
            case LED1:
                LATGSET = (1 << 12); 
                next = getInput();
               
                if (next == 1){
                    LATGCLR =0XF000;
                    systemState = LED2; }
                if (next == 2) {
                    LATGCLR =0XF000;
                    systemState = LED4; }
               ;
                break;

            case LED2:
                LATGSET = (1 << 13);
                next = getInput();

                if (next == 1) {
                    LATGCLR =0XF000;
                    systemState = LED3; }
                if (next == 2) {
                    LATGCLR =0XF000;
                    systemState = LED1; }
                
                break;

            case LED3:
                LATGSET = (1 << 14);
                next = getInput();

                if (next == 1) {
                    LATGCLR =0XF000;
                    systemState = LED4; }
                if (next == 2) { 
                    LATGCLR =0XF000;
                    systemState = LED2; }
                 
                break;

            case LED4:
                LATGSET = (1 << 15);
                next = getInput();

                if (next == 1) {
                    LATGCLR =0XF000;
                    systemState = LED1; }
                if (next == 2) {                    
                    LATGCLR =0XF000;
                    systemState = LED3; }
                
                break;
        }

    } // end while


    return EXIT_SUCCESS; // This return should never occur
} // end main  

void initialization() {
    // Initialize GPIO for BTN1-2 and LED1-4
    TRISGSET = 0xC0;     // For BTN1: configure PortG bit for input
    TRISGCLR = 0xF000;   // For LED1-4: configure PortG pin for output
    ODCGCLR  = 0xF000;   // For LED1-4: configure as normal output (not open drain)
    int i;
    for (i =0; i<5; i++){
        LATGCLR = 0XF000;
        LATGSET = 0XF000;
        LATGCLR = 0XF000;
        //DelayMs(500);
   
    }
    LATGCLR = 0XF000;
    DelayMs(1);
    LATGSET =0X1000;//0001 0000 0000 0000

    
} //end initialization

int getInput() {
/* pseudocode
    if BTN1 == 1
        return BTN1;
    else if BTN2 == 1;
        return BTN2;
    else if (BTN1 == 1) && (BTN2 == 1)
        return Both;
*/

    if (getInputBTN1() && getInputBTN2()) //both return true 
        return 0;
    else if (getInputBTN1()) //if only BTN1 is pushed
        return 1;
    else if (getInputBTN2()) //if only BTN2 is pushed
        return 2;
    else
        return; 

} //END getInput

bool getInputBTN1() {
    enum Button1Position {UP, DOWN}; // Possible states of BTN1
    
    static enum Button1Position button1CurrentPosition = UP;  // BTN1 current state
    static enum Button1Position button1PreviousPosition = UP; // BTN1 previous state
    static unsigned int button1History = 0x0;            // Last 32 samples of BTN1
    // Reminder - "static" variables retain their values from one call to the next.
    
    button1PreviousPosition = button1CurrentPosition;

    button1History = button1History << 1;           // Sample BTN1
    if(PORTG & 0x40)                
    {
        button1History = button1History | 0x01;
    }
    
    if ((button1History == 0xFFFFFFFF) && (button1CurrentPosition == UP))
    {
        button1CurrentPosition = DOWN;
    } else if ((button1History == 0x0000) && (button1CurrentPosition == DOWN))
    {
         button1CurrentPosition = UP;  
    }
    
    if((button1CurrentPosition == DOWN) && (button1PreviousPosition == UP))
    {
        return TRUE; // debounced 0-to-1 transition has been detected
    }
    return FALSE;    // 0-to-1 transition not detected
}

bool getInputBTN2()
{
    enum Button2Position {UP, DOWN}; // Possible states of BTN1
    
    static enum Button2Position button2CurrentPosition = UP;  // BTN1 current state
    static enum Button2Position button2PreviousPosition = UP; // BTN1 previous state
    static unsigned int button2History = 0x0;            // Last 32 samples of BTN1
    // Reminder - "static" variables retain their values from one call to the next.
    
    button2PreviousPosition = button2CurrentPosition;

    button2History = button2History << 1;           // Sample BTN1
    if(PORTG & 0x80)                
    {
        button2History = button2History | 0x01;
    }
    

    if ((button2History == 0xFFFFFFFF) && (button2CurrentPosition == UP))
    {
        button2CurrentPosition = DOWN;
    } else if ((button2History == 0x0000) && (button2CurrentPosition == DOWN))
    {
         button2CurrentPosition = UP;  
    }
    
    if((button2CurrentPosition == DOWN) && (button2PreviousPosition == UP))
    {
        return TRUE; // debounced 0-to-1 transition has been detected
    }
    return FALSE;    // 0-to-1 transition not detected
}

bool Timer2Input()
{
    int timer2_current;                 // current reading from Timer2
    static int timer2_previous = 0;     // previous reading from Timer2
                                        //  (note:  static value is retained
                                        //  from previous call)  
    
    timer2_current = ReadTimer2();
    if(timer2_previous > timer2_current)
    {
        timer2_previous = timer2_current;
        return TRUE;
    } else
    {
        timer2_previous = timer2_current;
        return FALSE;
    }
}

void Timer2Init() {
    // The period of Timer 2 is (32 * 62500)/(10 MHz) = 500 ms (freq = 2 Hz)
    OpenTimer2(T2_ON | T2_IDLE_CON | T2_SOURCE_INT | T2_PS_1_32 | T2_GATE_OFF, 156249);
    return;
}







