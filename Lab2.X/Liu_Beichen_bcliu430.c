////////////////////////////////////////////////////////////////////////////////////
// ECE 2534:        Lab 2
// File name:       Liu_Beichen_bcliu430.c
// Description:     Test the Digilent board by writing a short message to the OLED.
//                  Also display a counter that updates every 100 milliseconds.
// Resources:       main.c uses Timer2 to measure elapsed time.
//					delay.c uses Timer1 to provide delays with increments of 1 ms.
//					PmodOLED.c uses SPI1 for communication with the OLED.
// Written by:      Beichen Liu 
// Last modified:   10/6/2016

#include <stdio.h>                      // for sprintf()
#include <stdbool.h>
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

/*
 * TODO
 * how to get a random number 4bit 0-15 8bit 0-255
 * how to store the buf into mem
 * how to use uart to do i/o
 * figure out Timer
 * other details
 * 
 * Question?
 * display milisecond?
 * page 4 example
 * power cycle?
 * next step after stat displayed?
 */

void init();
bool getBTN1();
bool getBTN2();
void Menu(int num);
void statsDisplay(int num);
void HDisplay(int num);
void timer2Input();
long getRand();


enum state {hd, stats, Hd4Bit,Hd8Bit, Stats4Bit, Stats8Bit,BackHD, BackStats};
enum state menu;

int main()
{
    

    init();
    while (1) {
        switch(menu){
            
///////////////////////////////////////////////////////////////////////////////            
//This part is for HD            
            case hd:
                if(getBTN1()) {
                    Menu(2); //2 means the arrow at stats;
                    menu= stats; 
                }
                else if(getBTN2()){
                    Menu(3);
                    menu = Hd4Bit;
                }
                break;
                
            case stats:
                if (getBTN1()) {
                    Menu(1);
                    menu = hd; 
                } 
                else if (getBTN2()) {
                    Menu(6);
                    menu = Stats4Bit;
                }
                break;
                
            case Hd4Bit:
                if(getBTN1()) {
                    Menu(4); //4 means the arrow at HD 8bit;
                    menu= Hd8Bit; 
                }
                else if(getBTN2()){
                    HDisplay(1);
                }
               
                    //run 4 bit hd;
                break;
                
            case Hd8Bit:
                if(getBTN1()) {
                    Menu(5); //3 means the arrow at HD 4 bit;
                    menu= BackHD; 
                }
                else if(getBTN2()){
                    HDisplay(2);
                }
                break;
            case BackHD:
                if(getBTN1()){
                    Menu(3);
                    menu = Hd4Bit;
                }
                else if(getBTN2()){
                    Menu(1);
                    menu = hd;
                }
                break;
                
                
            case Stats4Bit:
                if (getBTN1()) {
                    Menu(7);
                    menu = Stats8Bit;
                } else if (getBTN2()) {
                    statsDisplay(1);

                }
                //show 4 bit stats 
                break;
            case Stats8Bit:
                if (getBTN1()) {
                    Menu(8);
                    menu = BackStats;
                } else if (getBTN2()) {
                    statsDisplay(2);
                    
                }
                break;
                //show 8bit statistics;
                
            case BackStats:
                if(getBTN1()){
                    Menu(6);
                    menu = Stats4Bit;
                }
                else if(getBTN2()){
                    Menu(1);
                    menu = hd;
                }
                break;

        } //END SWITCH
    }//END WHILE
        
   return EXIT_SUCCESS;           // This return should never occur
} // end main  

// Initialize Timer2 so that it rolls over 10 times per second
void Timer2Init() 
{
    // The period of Timer 2 is (16 * 62500)/(10 MHz) = 100 ms (freq = 10 Hz)
    OpenTimer2(T2_ON | T2_IDLE_CON | T2_SOURCE_INT | T2_PS_1_16 | T2_GATE_OFF, 62499);
    INTClearFlag(INT_T2);
    return;
}

void init(){
       
    TRISGSET = 0xC0;     // For BTN 1 and 2: set pin 6 and 7 to 1 as input 
    TRISGCLR = 0xF000;   // For LEDs: configure PortG pin for output
    ODCGCLR  = 0xF000;   // For LEDs: configure as normal output (not open drain)

    // Initialize PmodOLED, also Timer1 and SPI1
    DelayInit();
    OledInit();
    Timer2Init();
    /*
         // Send a welcome message to the OLED display
    OledClearBuffer();
    OledSetCursor(0, 0);          // upper-left corner of display
    OledPutString("ECE 2534");
    OledSetCursor(0, 1);          // column 0, row 1 of display
    OledPutString("Lab 2");
    OledSetCursor(0, 2);          // column 0, row 2 of display
    OledPutString("Security Sim");
    OledUpdate(); 
    
    DelayMs(5000); //delay 5s
    */
    menu=hd;
    Menu(1);
    
}

bool getBTN1() {
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

bool getBTN2()
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
    if(num ==1){
        OledClearBuffer();
        OledSetCursor(0, 0); // upper-left corner of display
        OledPutString("Security Sim");
        OledSetCursor(0, 1); // column 0, row 1 of display
        OledPutString("-> HD");
        OledSetCursor(0, 2); // column 0, row 2 of display
        OledPutString("   STATs");
        OledUpdate();
    }
    else if (num == 2){
        OledClearBuffer();
        OledSetCursor(0, 0); // upper-left corner of display
        OledPutString("Security Sim");
        OledSetCursor(0, 1); // column 0, row 1 of display
        OledPutString("   HD");
        OledSetCursor(0, 2); // column 0, row 2 of display
        OledPutString("-> STATs");
        OledUpdate();
    }
    else if (num == 3){
        OledClearBuffer();
        OledSetCursor(0, 0); // upper-left corner of display
        OledPutString("HD");
        OledSetCursor(0, 1); // column 0, row 1 of display
        OledPutString("-> 4-bit");
        OledSetCursor(0, 2); // column 0, row 2 of display
        OledPutString("   8-bit");
        OledSetCursor(0, 3); // column 0, row 2 of display
        OledPutString("   Back");
        OledUpdate();
    }
    else if (num == 4){
        OledClearBuffer();
        OledSetCursor(0, 0); // upper-left corner of display
        OledPutString("HD");
        OledSetCursor(0, 1); // column 0, row 1 of display
        OledPutString("   4-bit");
        OledSetCursor(0, 2); // column 0, row 2 of display
        OledPutString("-> 8-bit");
        OledSetCursor(0, 3); // column 0, row 2 of display
        OledPutString("   Back");
        OledUpdate();
    }
        else if (num == 5){
        OledClearBuffer();
        OledSetCursor(0, 0); // upper-left corner of display
        OledPutString("HD");
        OledSetCursor(0, 1); // column 0, row 1 of display
        OledPutString("   4-bit");
        OledSetCursor(0, 2); // column 0, row 2 of display
        OledPutString("   8-bit");
        OledSetCursor(0, 3); // column 0, row 2 of display
        OledPutString("-> Back");
        OledUpdate();
    }
        

    else if (num == 6){
        OledClearBuffer();
        OledSetCursor(0, 0); // upper-left corner of display
        OledPutString("Statistics");
        OledSetCursor(0, 1); // column 0, row 1 of display
        OledPutString("-> 4-bit");
        OledSetCursor(0, 2); // column 0, row 2 of display
        OledPutString("   8-bit");
        OledSetCursor(0, 3); // column 0, row 2 of display
        OledPutString("   Back");
        OledUpdate();
    }
    else if (num == 7){
        OledClearBuffer();
        OledSetCursor(0, 0); // upper-left corner of display
        OledPutString("Statistics");
        OledSetCursor(0, 1); // column 0, row 1 of display
        OledPutString("   4-bit");
        OledSetCursor(0, 2); // column 0, row 2 of display
        OledPutString("-> 8-bit");
         OledSetCursor(0, 3); // column 0, row 2 of display
        OledPutString("   Back");
        OledUpdate();
    }
    else if (num == 8){
        OledClearBuffer();
        OledSetCursor(0, 0); // upper-left corner of display
        OledPutString("Statistics");
        OledSetCursor(0, 1); // column 0, row 1 of display
        OledPutString("   4-bit");
        OledSetCursor(0, 2); // column 0, row 2 of display
        OledPutString("   8-bit");
        OledSetCursor(0, 3); // column 0, row 2 of display
        OledPutString("-> Back");
        OledUpdate();
    }

}

void statsDisplay(int num){
    if (num ==1){
        //display 4 bit best 3 records;
        OledClearBuffer();
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
        OledClearBuffer();
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
        static int timeCount=0;        

        char buf[17];               // Temporary string for OLED display
              
        OledClearBuffer();
         /*
        if (INTGetFlag(INT_T2))
        {            
            // Timer2 has rolled over, so increment count of elapsed time
            INTClearFlag(INT_T2);
            timeCount++;

        }
            // Display elapsed time in units of seconds, with decimal point
            sprintf(buf, "%14d.%d", timeCount/10, timeCount%10);
            OledSetCursor(0, 3);
            OledPutString(buf);
            OledUpdate();*/
        Timer2Init();
        //display 4 bit best 3 records;
        OledSetCursor(0, 0);          // upper-left corner of display
        OledPutString("4-Bit HD");
        OledSetCursor(0, 1);          
        OledPutString("****");//add some buf here.
        OledSetCursor(0, 2);          
        OledPutString("  "); //wait for user input.
        OledUpdate();
    }
    else if (num == 2){
        OledClearBuffer();
        timer2Input();
        //run 8 bit result;
        OledSetCursor(0, 0);          // upper-left corner of display
        OledPutString("8-Bit HD");
        OledSetCursor(0, 1);          
        OledPutString("********");//add some buf here.
        OledSetCursor(0, 2);          
        OledPutString("  "); //wait for user input.
        
        OledUpdate();
    }
}

void timer2Input(){
        char buf[17];               // Temporary string for OLED display
        static unsigned int timeCount = 0; // Elapsed time since initialization of program
        unsigned int timer2_current=0, timer2_previous=0;
        
        Timer2Init();
        
        
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
    }

long getRand(int num){
    int i;
    long bit_out;
    if (num == 4){
        for(i = 0; i<3; i++)
            bit_out = bit_out | (rand()%2 << i);
    }
    else if (num == 8){
        for(i = 0; i<7; i++)
            bit_out = bit_out | (rand()%2 << i);
        
    } 
    
    return bit_out;
}