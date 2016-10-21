////////////////////////////////////////////////////////////////////////////////////
// ECE 2534:        Lab 2
// File name:       Liu_Beichen_bcliu430.c
// Description:     This project is written to find the Hamming distance between the
//                  user input from uart and the system-generated random number;
//                  Menu()         is used to display all the menu
//                  HDisplay()     is used to find the hamming distance. using the 
//                                 LED on the board to display the hamming distance
//                  statsDisplay() is used to save result and display result
// Resources:       main.c uses Timer2 to measure elapsed time.
//                  delay.c uses Timer1 to provide delays with increments of 1 ms.
//		    PmodOLED.c uses SPI1 for communication with the OLED.
// Written by:      Beichen Liu 
// Last modified:   10/12/2016

#include <stdio.h>                      // for sprintf()
#include <stdbool.h>
#include <stdlib.h>
#include <plib.h>                       // Peripheral Library
#include <string.h>
#include "PmodOLED.h"
#include "OledChar.h"
#include "OledGrph.h"
#include "delay.h"
#include <time.h>

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

enum state {score5, score10, score20,confirm5, confirm10,confirm20};
enum state sysState;
/*
 * TODO
 * 
 *
 */

void init();
void Menu(int num);
bool getBTN1();
bool getBTN2();

int main() {

    init();
   
    while (1) {
        switch(sysState){
            case score5:
                if(getBTN1()){
                    sysState = score10;
                    menu(2);
                }
                else if (getBTN2()){
                    sysState = confirm5;
                    menu(4);
                }
                break;
            
            case score10:
                if(getBTN1()){
                    sysState = score20;
                    menu(3);
                }
                else if (getBTN2()){
                    sysState = confirm10;
                    menu(5);
                }
                break;
            
            case score20:
                if(getBTN1()){
                    sysState = score5;
                    menu(1);
                }
                else if (getBTN2()){
                    sysState = confirm20;
                    menu(6);
                }
                break;
            
            case confirm5:
                if(getBTN1()){
                    sysState = score5;    
                    menu(1);
   
                }
                else if (getBTN2()){
                    OledClearBuffer();
                    OledSetCursor(0, 0);          // column 0, row 0 of display
                    OledPutString("The win score is 5  ");
                    OledUpdate();
                }
                break;
            
            case confirm10:
                if(getBTN1()){
                    sysState = score10;    
                    menu(2);
   
                }
                else if (getBTN2()){
                    OledClearBuffer();
                    OledSetCursor(0, 0);          // column 0, row 0 of display
                    OledPutString("The win score is 10  ");
                    OledUpdate();
                }
                break;
                
            case confirm20:
                if(getBTN1()){
                    sysState = score20;    
                    menu(3);   
                }
                else if (getBTN2()){
                    OledClearBuffer();
                    OledSetCursor(0, 0);          // column 0, row 0 of display
                    OledPutString("The win score is 20  ");
                    OledUpdate();
                } 
                break;
        }
       
    }//END WHILE
        
   return EXIT_SUCCESS;           // This return should never occur
} // end main  

// Initialize Timer2 so that it rolls over 10 times per second
void TimerInit() {
    // The period of Timer 2 is (16 * 625)/(10 MHz) = 1 ms (freq = 10 Hz)
    OpenTimer2(T2_ON | T2_IDLE_CON | T2_SOURCE_INT | T2_PS_1_16 | T2_GATE_OFF, 624);
    INTClearFlag(INT_T2);
    OpenTimer3(T3_ON | T3_IDLE_CON | T3_SOURCE_INT | T3_PS_1_16 | T3_GATE_OFF, 62499);
    INTClearFlag(INT_T3);
    // The period of Timer 4 is (16 * 625)/(10 MHz) = 1 ms (freq = 10 Hz)
    OpenTimer4(T4_ON | T4_IDLE_CON | T4_SOURCE_INT | T4_PS_1_16 | T4_GATE_OFF, 624);
    INTClearFlag(INT_T4);
    return;
}


void init(){ //initialization
       
    TRISGSET = 0xC0;     // For BTN 1 and 2: set pin 6 and 7 to 1 as input 
    TRISGCLR = 0xF000;   // For LEDs: configure PortG pin for output
    ODCGCLR  = 0xF000;   // For LEDs: configure as normal output (not open drain)
    LATGSET  = 0xf000;   // all LEDs are on for 5 seconds;
    // initialize functions
    DelayInit();
    OledInit();
    TimerInit();

    unsigned int timeCount=0;
    
    
    // Send a welcome message to the OLED display
    // initialization message
    OledClearBuffer();
    OledSetCursor(0, 0);          // column 0, row 0 of display
    OledPutString(" ECE 2534 LAB 3  ");
    OledSetCursor(0, 1);          // column 0, row 1 of display
    OledPutString("    PONG GAME    ");
    OledSetCursor(0, 2);          // column 0, row 2 of display
    OledPutString("   WRITTEN BY    ");
    OledSetCursor(0, 3);          // column 0, row 2 of display
    OledPutString("   BEICHEN LIU   ");
    OledUpdate(); 
    
    while (timeCount <= 5000) { //delay for 5 second
        if (INTGetFlag(INT_T2)) // Has roll-over occurred? (Has 1 ms passed?)
        {
            timeCount++;
            INTClearFlag(INT_T2);// Clear flag so we don't respond until it sets again
        }

    }
    
    LATGCLR  = 0xf000; // testing LEDs finishes
    OledClearBuffer();
    sysState = score5;
    menu(1); 

}

void menu(int num){
    if (num == 1){
    OledClearBuffer();
    OledSetCursor(0, 0);          // column 0, row 0 of display
    OledPutString("Choose a score   ");
    OledSetCursor(0, 1);          // column 0, row 1 of display
    OledPutString("->  5            ");
    OledSetCursor(0, 2);          // column 0, row 2 of display
    OledPutString("    10    ");
    OledSetCursor(0, 3);          // column 0, row 2 of display
    OledPutString("    20  ");
    OledUpdate();    
    }
    else if(num ==2){
    OledClearBuffer();
    OledSetCursor(0, 0);          // column 0, row 0 of display
    OledPutString("Choose a score   ");
    OledSetCursor(0, 1);          // column 0, row 1 of display
    OledPutString("    5            ");
    OledSetCursor(0, 2);          // column 0, row 2 of display
    OledPutString("->  10    ");
    OledSetCursor(0, 3);          // column 0, row 2 of display
    OledPutString("    20  ");
    OledUpdate();        
    }
    else if (num ==3){
    OledClearBuffer();
    OledSetCursor(0, 0);          // column 0, row 0 of display
    OledPutString("Choose a score   ");
    OledSetCursor(0, 1);          // column 0, row 1 of display
    OledPutString("    5            ");
    OledSetCursor(0, 2);          // column 0, row 2 of display
    OledPutString("    10    ");
    OledSetCursor(0, 3);          // column 0, row 2 of display
    OledPutString("->  20  ");
    OledUpdate();       
    }
    else if(num ==4){
    OledClearBuffer();
    OledSetCursor(0, 0);          // column 0, row 0 of display
    OledPutString(" push BTN2 to ");
    OledSetCursor(0, 1);          // column 0, row 1 of display
    OledPutString("confirm score 5  ");
    OledSetCursor(0, 2);          // column 0, row 2 of display
    OledPutString("or BTN1 to go back");
    OledSetCursor(0, 3);          // column 0, row 2 of display
    OledPutString("to previous menu  ");
    OledUpdate();       
    }
    else if(num ==5){
    OledClearBuffer();
    OledSetCursor(0, 0);          // column 0, row 0 of display
    OledPutString(" push BTN2 to ");
    OledSetCursor(0, 1);          // column 0, row 1 of display
    OledPutString("confirm score 10  ");
    OledSetCursor(0, 2);          // column 0, row 2 of display
    OledPutString("or BTN1 to go back");
    OledSetCursor(0, 3);          // column 0, row 2 of display
    OledPutString("to previous menu  ");
    OledUpdate();       
    }
    else if(num ==6){
    OledClearBuffer();
    OledSetCursor(0, 0);          // column 0, row 0 of display
    OledPutString("  push BTN2 to ");
    OledSetCursor(0, 1);          // column 0, row 1 of display
    OledPutString("confirm score 20  ");
    OledSetCursor(0, 2);          // column 0, row 2 of display
    OledPutString("or BTN1 to go back");
    OledSetCursor(0, 3);          // column 0, row 2 of display
    OledPutString("to previous menu  ");
    OledUpdate();       
    }

}


bool getBTN1() {
    enum Button1Position {UP, DOWN}; // Possible states of BTN1
    static enum Button1Position button1CurrentPosition = UP;  // BTN1 current state
    static enum Button1Position button1PreviousPosition = UP; // BTN1 previous state
    static unsigned int button1History = 0x0;            // Last 32 samples of BTN1
    // Reminder - "static" variables retain their values from one call to the next.
    button1PreviousPosition = button1CurrentPosition;

    while (!INTGetFlag(INT_T2)) { } // Has roll-over occurred? (Has 1 ms passed?)
  
    INTClearFlag(INT_T2);

     
    button1History = button1History << 1;           // Sample BTN1
    if(PORTG & 0x40)                
        button1History = button1History | 0x01;
     
    if ((button1History == 0xFFFFFFFF) && (button1CurrentPosition == UP))
        button1CurrentPosition = DOWN;
    
    else if ((button1History == 0x0000) && (button1CurrentPosition == DOWN))
         button1CurrentPosition = UP;  
    
    if((button1CurrentPosition == DOWN) && (button1PreviousPosition == UP))
        return TRUE; // debounced 0-to-1 transition has been detected

    return FALSE;    // 0-to-1 transition not detected
}

bool getBTN2() {
    enum Button2Position {UP, DOWN}; // Possible states of BTN1
    static enum Button2Position button2CurrentPosition = UP;  // BTN1 current state
    static enum Button2Position button2PreviousPosition = UP; // BTN1 previous state
    static unsigned int button2History = 0x0;            // Last 32 samples of BTN1
    // Reminder - "static" variables retain their values from one call to the next
    button2PreviousPosition = button2CurrentPosition;

    while (!INTGetFlag(INT_T2)) { } // Has roll-over occurred? (Has 1 ms passed?)

    INTClearFlag(INT_T2);

    button2History = button2History << 1;           // Sample BTN1
    if(PORTG & 0x80 )               
        button2History = button2History | 0x01;
     
    if ((button2History == 0xFFFFFFFF) && (button2CurrentPosition == UP))
        button2CurrentPosition = DOWN;
    
    else if ((button2History == 0x0000) && (button2CurrentPosition == DOWN))
        button2CurrentPosition = UP;  
    
    
    if((button2CurrentPosition == DOWN) && (button2PreviousPosition == UP))
        return TRUE; // debounced 0-to-1 transition has been detected
    
    return FALSE;    // 0-to-1 transition not detected
}

void game(int num){
    
    
}
