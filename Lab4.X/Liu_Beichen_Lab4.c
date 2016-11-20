/////////////////////////////////////////////////////////////////////////////////////////////////////
// ECE 2534:        Lab 4 Drum Hero
//
// File name:       Liu_Beichen_Lab4.c
//
// Description:     This c program is written to simulate the famous Drum Hero Game.
//                  Enjoy!
//
// Written by:      Beichen Liu 刘北辰
//
// Last modified:   11.20.2016
//
// Version:         ver.11-20-2016 create file, setup menu, set time countdown
// 
////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                //
//                                             README                                             //
//                                                                                                // 
//         This game is controled by button 1, button 2, and accel. In the menu list, use         //
//         Button 1 to select up and down, and use button 2 to confirm.                           //
//         
//         In the game interface, you have 5 seconds to test your accel, left twist will 
//         make the third line a circle which means you hit the "target", and led3 will on 
//         while you right twist. double 
//         
//         Enjoy your game.                                                                       //
//                                                                                                //
////////////////////////////////////////////////////////////////////////////////////////////////////
//
//
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
 * 
 *  1. Object Moving
 *  2. Object hit with led
 *  3. display hit and miss on oled
 *  4. setup accel
 *  5. extra credit: if hit/total > 90% continue game, display the highest score after the game ends.
 *  
 */

enum state {easy, hard};
enum state sysState;


/*
 *  Global Variable
 */
static int CountDown = 90; // initial game time is 90 second
static int a =97;          // initial clear pixel variable

BYTE blank[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
char blank_char = 0x00;
BYTE doubleline[8] = {0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18};
char doubleline = 0x01;
#define DELAY 300


// function declaraction
void init();
void setLine();
void Menu(int num);
bool getBTN1();
bool getBTN2();
void delay(int num);
void ClearPix(int xAixs);
void countDown();

int main() { // main function
    
    init(); // initialize system
    while (1) {
        switch(sysState){
            case easy:
                if(getBTN1()){
                    menu(2);
                    sysState = hard;
                }
                else if(getBTN2()){
                    game();
                }
                else
                    sysState = sysState;
             break;
            case hard:
                if(getBTN1()){
                    menu(1);
                    sysState = easy;
                }
                else if(getBTN2()){
                    game();
                }
                else
                    sysState = sysState;
             break;
        
        
        }
    }// end while
    
    return (EXIT_SUCCESS);
}



void init(){ //initialization
       
    TRISGSET = 0xC0;     // For BTN 1 and 2: set pin 6 and 7 to 1 as input 
    TRISGCLR = 0xF000;   // For LEDs: configure PortG pin for output
    ODCGCLR  = 0xF000;   // For LEDs: configure as normal output (not open drain)
    LATGSET  = 0xf000;   // all LEDs are on for 5 seconds;
    // initialize functions
    DelayInit();
    OledInit();
    
    TRISBCLR = 0x0040;   
    ODCBCLR  = 0x0040;   
    LATBSET  = 0x0040;
    
    // The period of Timer 2 is (16 * 625)/(10 MHz) = 1 ms (freq = 10 Hz)
    OpenTimer2(T2_ON | T2_IDLE_CON | T2_SOURCE_INT | T2_PS_1_16 | T2_GATE_OFF, 624);
    INTClearFlag(INT_T2);
    // The period of Timer 3 is (256 * 39032)/(10 MHz) = 1 s (freq = 1 Hz)
    OpenTimer3(T3_ON | T3_IDLE_CON | T3_SOURCE_INT | T3_PS_1_16 | T3_GATE_OFF, 624);
    INTSetVectorPriority(INT_TIMER_3_VECTOR, INT_PRIORITY_LEVEL_4);
    INTClearFlag(INT_T3);
    INTEnable(INT_T3, INT_ENABLED);    // The period of Timer 4 is 1 s (freq = 1 Hz)
    OpenTimer4(T4_ON | T4_IDLE_CON | T4_SOURCE_INT | T4_PS_1_256 | T4_GATE_OFF, 39061);
    INTClearFlag(INT_T4);


    //end ADC initialization
    
    unsigned int timeCount=0;
    
    
    // Send a welcome message to the OLED display
    // initialization message
    OledClearBuffer();
    OledSetCursor(0, 0);          // column 0, row 0 of display
    OledPutString(" ECE 2534 LAB 4  ");
    OledSetCursor(0, 1);          // column 0, row 1 of display
    OledPutString("    DRUM HERO    ");
    OledSetCursor(0, 2);          // column 0, row 2 of display
    OledPutString("   WRITTEN BY    ");
    OledSetCursor(0, 3);          // column 0, row 2 of display
    OledPutString("   BEICHEN LIU   ");
    OledUpdate(); 
    
    while (timeCount <= 500) { //delay for 5 second
        if (INTGetFlag(INT_T2)) {
            timeCount++;
            INTClearFlag(INT_T2);// Clear flag so we don't respond until it sets again
        }

    }
    LATGCLR  = 0xf000; // testing LEDs finishes
    OledClearBuffer();
//    sysState = easy;
//    menu(1); 

}


void menu(int num){
    OledClearBuffer();
    OledSetCursor(0,0);
    OledPutString("ChooseDifficulty");
    OledSetCursor(3,1);
    OledPutString("Easy");
    OledSetCursor(3,2);
    OledPutString("Hard");
    if(num == 1) { // easy mode
        OledSetCursor(0,1);
        OledPutString("->");
    }
    if(num ==2) {
        OledSetCursor(0,2);
        OledPutString("->");
    }
    OledUpdate();
}

void setLine() {
    OledClearBuffer();
    OledMoveTo(0,3);
    OledDrawRect(111,4);
    OledMoveTo(0,11);
    OledDrawRect(111,12);
    OledMoveTo(0,19);
    OledDrawRect(111,20);
    OledUpdate();
    
   
}
void game() {
    OledClearBuffer();
    OledMoveTo(8,27);
    OledDrawRect(97,28);
    while (CountDown > 0) {
        countDown();
        OledUpdate();
     
    }

    if(CountDown == 0) {
        OledSetCursor(2,0);
        OledPutString("GAME OVER");
        OledSetCursor(2,1);
        OledPutString("Your Score:");
        OledSetCursor(1,3);
        OledPutString("congratulations");

    }
    CountDown = 90; // reset countDown
    a = 97;         // reset clear pixel 
    while(!getBTN2());
    menu(1);
    sysState = easy;
}
void ClearPix(int xAxis) {
    OledMoveTo(xAxis,27);
    OledClearPixel();
    OledMoveTo(xAxis,28);
    OledClearPixel();
    OledUpdate();
}

void countDown(){
    OledDefUserChar(blank_char, blank);
    
    char buf[2];    
    if(INTGetFlag(INT_T4)) {
        INTClearFlag(INT_T4);
        CountDown--;
        if (CountDown < 10) {
            OledSetCursor(14,3);
            OledPutChar(blank_char);
            OledSetCursor(15,3);
        }
        else 
            OledSetCursor(14,3);
        sprintf(buf,"%d",CountDown);
        OledPutString(buf);
        ClearPix(a);
        a--;
        }
}



/*
 *  delay function
 */ 

void delay(int num) {
    unsigned int time = 0;
    while(time < num) {
        if(INTGetFlag(INT_T2)){
            time++;
            INTClearFlag(INT_T2);
        }

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
