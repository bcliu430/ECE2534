////////////////////////////////////////////////////////////////////////////////
// ECE 2534:        Lab 3  Pong Game
//
// File name:       Liu_Beichen_Lab3.c
//
// Description:     This c program is written to simulate a single person 
//                  Pong Game.
//                  You will need to use ADC joystick to control up and down.
//                  Right now, using BTN1 to scroll up and down
//                  Use BTN2 to confirm or enter.
//                  The goal of this game is to get the desired score
//                  Enjoy!
// Written by:      Beichen Liu 刘北辰
// Last modified:   21 October 2016 

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
 * 1. Setup ADC
 * 2. pixel move
 * 3. right-side paddle
 * 4. score count
 * 5. what to do after win
 * 6. init time 5 sec
 * 
 */

enum state {score5, score10, score20,confirm5, confirm10,confirm20,back5,back10,back20};
enum state sysState;

// function declaraction
void init();
void Menu(int num);
bool getBTN1();
bool getBTN2();
void game(int num);

int main() { // main()

    init(); // initialize system

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
                    sysState = back5;    
                    menu(7);
   
                }
                else if (getBTN2()){
                    game(5);
                }
                break;
            
            case confirm10:
                if(getBTN1()){
                    sysState = back10;    
                    menu(8);
   
                }
                else if (getBTN2()){
                    game(10);
                }
                break;
                
            case confirm20:
                if(getBTN1()){
                    sysState = back20;    
                    menu(9);   
                }
                else if (getBTN2()){
                    game(20);
                } 
                break;
            case back5:
                if(getBTN1()){
                    sysState = confirm5;    
                    menu(4);
   
                }
                else if (getBTN2()){
                    sysState = score5;
                    menu(1);
                }
                break;
            
            case back10:
                if(getBTN1()){
                    sysState = confirm10;    
                    menu(5);
   
                }
                else if (getBTN2()){
                    sysState = score10;
                    menu(2);
                }
                break;
                
            case back20:
                if(getBTN1()){
                    sysState = confirm20;    
                    menu(6);   
                }
                else if (getBTN2()){
                    sysState = score20;
                    menu(3);

                } 
                break;
        }

    }// end while
    
    return (EXIT_SUCCESS);
}
// Initialize Timer2 so that it rolls over 10 times per second
void TimerInit() {
    // The period of Timer 2 is (16 * 625)/(10 MHz) = 1 ms (freq = 10 Hz)
    OpenTimer2(T2_ON | T2_IDLE_CON | T2_SOURCE_INT | T2_PS_1_16 | T2_GATE_OFF, 624);
    INTClearFlag(INT_T2);
    // The period of Timer 3 is (256 * 39032)/(10 MHz) = 1 s (freq = 1 Hz)
    OpenTimer3(T3_ON | T3_IDLE_CON | T3_SOURCE_INT | T3_PS_1_256 | T3_GATE_OFF, 39061);
    INTClearFlag(INT_T3); 
    // The period of Timer 4 is (16 * 625)/(10 MHz) = 1 ms (freq = 10 Hz)
    OpenTimer4(T4_ON | T4_IDLE_CON | T4_SOURCE_INT | T4_PS_1_16 | T4_GATE_OFF, 624);
    INTClearFlag(INT_T4);
    return;
}

void menu(int num){
    if ((num == 1)||(num == 2)||(num ==3)) {
        OledClearBuffer();
        OledSetCursor(0, 0);          // column 0, row 0 of display
        OledPutString("Choose a score   ");
        OledSetCursor(3, 1);          // column 0, row 1 of display
        OledPutString("5");
        OledSetCursor(3, 2);          // column 0, row 2 of display
        OledPutString("10");
        OledSetCursor(3, 3);          // column 0, row 2 of display
        OledPutString("20");

        if (num == 1){
            OledSetCursor(0, 1);      
            OledPutString("->");
        }
        else if(num == 2){
            OledSetCursor(0, 2);      
            OledPutString("->");    
        }
        else if(num == 3){
            OledSetCursor(0, 3);      
            OledPutString("->");    
        }
        OledUpdate();    
    }
    else if ((num == 4)||(num == 7)) {
        OledClearBuffer();
        OledSetCursor(3, 1);          // column 0, row 1 of display
        OledPutString("Confirm 5 ");
        OledSetCursor(3, 2);          // column 0, row 2 of display
        OledPutString("Back");
        OledUpdate();

        if (num == 4){
            OledSetCursor(0, 1);      
            OledPutString("->");
        }
        else if(num == 7){
            OledSetCursor(0, 2);      
            OledPutString("->");    
        }
        OledUpdate();
    }
    else if ((num == 5)||(num == 8)) {
        OledClearBuffer();
        OledSetCursor(3, 1);          // column 0, row 1 of display
        OledPutString("Confirm 10 ");
        OledSetCursor(3, 2);          // column 0, row 2 of display
        OledPutString("Back");
        OledUpdate();

        if (num == 5){
            OledSetCursor(0, 1);      
            OledPutString("->");
        }
        else if(num == 8){
            OledSetCursor(0, 2);      
            OledPutString("->");    
        }
        OledUpdate();
    }
    else if ((num == 6)||(num == 9)) {
        OledClearBuffer();
        OledSetCursor(3, 1);          // column 0, row 1 of display
        OledPutString("Confirm 20");
        OledSetCursor(3, 2);          // column 0, row 2 of display
        OledPutString("Back");
        OledUpdate();

        if (num == 6){
            OledSetCursor(0, 1);      
            OledPutString("->");
        }
        else if(num == 9){
            OledSetCursor(0, 2);      
            OledPutString("->");    
        }
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
    
    while (timeCount <= 1000) { //delay for 5 second
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
void game(int num){

    int score = 3;
    char SCORE[1];
    char COUNTDOWN[17];
    char MSG[17];
    int time =0;
    int count = 6;


    setBack();
    sprintf(MSG,"Win Score is %d",num);
    OledSetCursor(1,1);
    OledPutString(MSG);

    while(time <7){
        sprintf(COUNTDOWN,"  start in %d",count);
        OledSetCursor(1,2);
        OledPutString(COUNTDOWN);
        if (INTGetFlag(INT_T3)) {
            time++;
            count--;
            INTClearFlag(INT_T3); 
        } 
    }
    
   OledClearBuffer();
   setBack();

    while(score < num){
        sprintf (SCORE, "%d" , score);
        OledSetCursor(9,1);
        OledPutString(SCORE);
        OledUpdate();
        if(getBTN2()) {
            menu(1);
            sysState = score5;
        }
    }
}


void setBack() { //setup the game background
    OledClearBuffer();
    /*
     * draw a rectangle;
     */
    OledMoveTo(0, 0);
    OledDrawRect(127, 31);

    /*
     * setup middle line 
     */
    OledMoveTo(63, 3);
    OledDrawRect(64, 4);
    OledMoveTo(63, 7);
    OledDrawRect(64, 8);
    OledMoveTo(63, 11);
    OledDrawRect(64, 12);
    OledMoveTo(63, 15);
    OledDrawRect(64, 16);
    OledMoveTo(63, 19);
    OledDrawRect(64, 20);
    OledMoveTo(63, 23);
    OledDrawRect(64, 24);    
    OledMoveTo(63, 27);
    OledDrawRect(64, 28);        
    OledUpdate();


}
