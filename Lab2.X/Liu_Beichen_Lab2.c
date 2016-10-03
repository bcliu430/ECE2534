////////////////////////////////////////////////////////////////////////////////////
// ECE 2534:        Lab 2
// File name:       Liu_Beichen_Lab2.c
// Description:     user should be able to decode the Hamming distance.
//                  Also display a counter that records the running time.
//                  Save the best 3 time to stats.
// Resources:       main.c uses Timer2 to measure elapsed time.
//					delay.c uses Timer1 to provide delays with increments of 1 ms.
//					PmodOLED.c uses SPI1 for communication with the OLED.
//                  myUART.c allows user input.
// Written by:      Beichen Liu
// Last modified:   10/3/2016

/*
 * TODO
 * how to get a random number 4bit 0-15 8bit 0-255
 * how to store the buf into mem
 * how to use uart to do i/o
 * figure out Timer
 * other details
 * 
 * Question
 * display milisecond?
 * page 4 example
 * power cycle?
 * next step after stat displayed?
 */

#include <stdio.h>                      // for sprintf()
#include <stdbool.h>
#include <plib.h>                       // Peripheral Library
#include "PmodOLED.h"
#include "OledChar.h"
#include "OledGrph.h"
#include "delay.h"
#include "myUART.h"

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

void Timer2Init(); 
bool Timer2Input();
void init();
bool BTN1();
bool BTN2();
//int Menu(num);


enum state {hd, stats, hd4Bit,hd8Bit, stats4Bit, stats8Bit};
enum state menu;

int main()
{
    OledInit();
    init(); //initialization;
   
   // Main processing loop
    while (1) {
        switch(menu){
///////////////////////////////////////////////////////////////////////////////            
//This part is for HD            
            case hd:
                if(BTN1()) {
                    Menu(2); //2 means the arrow is at stats;
                    menu= stats; }
                else if(BTN2()){
                    Menu(3);
                    menu = hd4Bit;}
            case hd4Bit:
                if(BTN1()) {
                    Menu(4); //2 means the arrow is at stats;
                    menu= hd8Bit; }
                else if(BTN2()){
                    HDisplay(1);
                }
                    //run 4 bit hd;
            case hd8Bit:
                if(BTN1()) {
                    Menu(3); //2 means the arrow is at stats;
                    menu= hd4Bit; }
                else if(BTN2()){
                    HDisplay(2);
                }
                    //run 8bit hd
///////////////////////////////////////////////////////////////////////////////
//This part is for statistics;
                
            case stats:
                if(BTN1()) {
                    Menu(1);
                    menu = hd; }
                else if(BTN2()){
                    Menu(5)
                    menu = stats4Bit; }  
            case stats4Bit:
                if(BTN1()) {
                    Menu(6);
                    menu = stats8Bit; }
                else if(BTN2()){
                    statsDisplay(1);
                }
                    //show 4 bit stats 
            case stats8Bit:
                if(BTN1()) {
                    Menu(5);
                    menu = stats4Bit;}
                else if(BTN2()){
                    statsDisplay(2);
                }
                    //show 8bit statistics;
////////////////////////////////////////////////////////////////////////////////                
              
        }

        
        
    } // end while
    
   return EXIT_SUCCESS;           // This return should never occur
} // end main  

// Initialize Timer2 so that it rolls over 10 times per second
void Timer2Init() 
{
    // The period of Timer 2 is (16 * 62500)/(10 MHz) = 100 ms (freq = 10 Hz)
    OpenTimer2(T2_ON | T2_IDLE_CON | T2_SOURCE_INT | T2_PS_1_16 | T2_GATE_OFF, 62499);
    INTClearFlag(INT_T3);
    return;
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

void init(){
      // Initialize GPIO for BTN1 and LED1
    TRISGSET = 0x40;     // For BTN1: configure PortG bit for input
    TRISGCLR = 0x1000;   // For LED1: configure PortG pin for output
    ODCGCLR  = 0x1000;   // For LED1: configure as normal output (not open drain)

    // Initialize PmodOLED, also Timer1 and SPI1
    //DelayInit();
    OledInit();
    Timer2Init();
    unsigned int count =0;
/*    while(count<10){
        if(INTGetFlag(INT_T2)){
            count++;
            INTClearFlag(INT_T2);
            }
        OledSetCursor(0, 0);          // upper-left corner of display
        OledPutString("ECE 2534");
        OledSetCursor(0, 1);          // column 0, row 1 of display
        OledPutString("Lab 2");
        OledSetCursor(0, 2);          // column 0, row 2 of display
        OledPutString("Security Sim");
        OledUpdate();
    }*/
    

    menu = hd;
}


bool BTN1() {
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

bool BTN2()
{
    enum Button2Position {UP, DOWN}; // Possible states of BTN1
    
    static enum Button2Position button2CurrentPosition = UP;  // BTN1 current state
    static enum Button2Position button2PreviousPosition = UP; // BTN1 previous state
    static unsigned int button2History = 0x0;            // Last 32 samples of BTN1
    // Reminder - "static" variables retain their values from one call to the next.
    
    button2PreviousPosition = button2CurrentPosition;

    button2History = button2History << 1;           // Sample BTN1
    if(PORTG & 0x80 )                
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

void Menu(int num){
    OledInit();
    if (num == 1){
        OledSetCursor(0, 0); // upper-left corner of display
        OledPutString("Security Sim");
        OledSetCursor(0, 1); // column 0, row 1 of display
        OledPutString("-> HD");
        OledSetCursor(0, 2); // column 0, row 2 of display
        OledPutString("   STATs");
        OledUpdate();
    }
    else if (num == 2){
        OledSetCursor(0, 0); // upper-left corner of display
        OledPutString("Security Sim");
        OledSetCursor(0, 1); // column 0, row 1 of display
        OledPutString("   HD");
        OledSetCursor(0, 2); // column 0, row 2 of display
        OledPutString("-> STATs");
        OledUpdate();
    }
    else if (num == 3){
        OledSetCursor(0, 0); // upper-left corner of display
        OledPutString("HD");
        OledSetCursor(0, 1); // column 0, row 1 of display
        OledPutString("-> 4-bit");
        OledSetCursor(0, 2); // column 0, row 2 of display
        OledPutString("   8-bit");
        OledUpdate();
    }
    else if (num == 4){
        OledSetCursor(0, 0); // upper-left corner of display
        OledPutString("HD");
        OledSetCursor(0, 1); // column 0, row 1 of display
        OledPutString("   4-bit");
        OledSetCursor(0, 2); // column 0, row 2 of display
        OledPutString("-> 8-bit");
        OledUpdate();
    }
    else if (num == 5){
        OledSetCursor(0, 0); // upper-left corner of display
        OledPutString("Statistics");
        OledSetCursor(0, 1); // column 0, row 1 of display
        OledPutString("-> 4-bit");
        OledSetCursor(0, 2); // column 0, row 2 of display
        OledPutString("   8-bit");
        OledUpdate();
    }
    else if (num == 4){
        OledSetCursor(0, 0); // upper-left corner of display
        OledPutString("Statistics");
        OledSetCursor(0, 1); // column 0, row 1 of display
        OledPutString(" 4-bit");
        OledSetCursor(0, 2); // column 0, row 2 of display
        OledPutString("->   8-bit");
        OledUpdate();
    }

}

void statsDisplay(int num){
    if (num ==1){
        //display 4 bit best 3 records;
        OledSetCursor(0, 0);          
        OledPutString("4-Bit Stats");
        OledSetCursor(0, 1);          
        OledPutString("1. ");//add some buf here.
        OledSetCursor(0, 2);          
        OledPutString("2. "); //add some buf here.
        OledSetCursor(0, 3);          
        OledPutString("3. "); //add some buf here
        OledUpdate();
    }
    else if (num == 2){
        //display 8 bit best 3 records;
        OledSetCursor(0, 0);          
        OledPutString("8-Bit Stats");
        OledSetCursor(0, 1);          
        OledPutString("1. ");//add some buf here.
        OledSetCursor(0, 2);          
        OledPutString("2. "); //add some buf here.
        OledSetCursor(0, 3);          
        OledPutString("3. "); //add some buf here
        OledUpdate();
    }
}
    
void HDisplay(int num){
    if (num ==1){
        //display 4 bit best 3 records;
        OledSetCursor(0, 0);          // upper-left corner of display
        OledPutString("4-Bit HD");
        OledSetCursor(0, 1);          
        OledPutString("****");//add some buf here.
        OledSetCursor(0, 2);          
        OledPutString("  "); //wait for user input.
        OledSetCursor(0, 3);          
        OledPutString("  "); //add timer
        OledUpdate();
    }
    else if (num == 2){
        //run 8 bit result;
        //display 4 bit best 3 records;
        OledSetCursor(0, 0);          // upper-left corner of display
        OledPutString("8-Bit HD");
        OledSetCursor(0, 1);          
        OledPutString("********");//add some buf here.
        OledSetCursor(0, 2);          
        OledPutString("  "); //wait for user input.
        OledSetCursor(0, 3);          
        OledPutString("  "); //add timer
        OledUpdate();
    }
}


