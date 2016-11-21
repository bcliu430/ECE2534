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
//                  ver.11-21-2016 setup target moving, score system and animition when hit target
//                                 setup leds
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
 *  1. Object Moving fast -> probabaly using pixel
 *  2. calculate miss
 *  3. setup accel
 *  4. extra credit: if hit/total > 90% continue game, display the highest score after the game ends.
 *  
 */

enum state {easy, hard};
enum state sysState;


/*
 *  Global Variable
 */
static int CountDown = 90; // initial game time is 90 second
static int a = 8;          // initial clear pixel variable
static int timer = 0;
static int score = 0;
static int miss = 0;
static int pos_x = 0, pos_y = 0;
BYTE blank[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
char blank_char = 0x00;
BYTE line[8] = {0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08};
char line_char = 0x01;
BYTE target[8] = {0x00, 0x18, 0x3c, 0x7e, 0x7e, 0x3c, 0x18, 0x00};
char target_char = 0x02;
BYTE hit[8] = { 0x18, 0x3c, 0x7e, 0xff, 0xff, 0x7e, 0x3c, 0x18};
char hit_char = 0x03;
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
void TarMove();
void DrawBall();
void Hit(pos_x, pos_y);
void disp_score(int hit, int miss);
void ledLit(int num);

int main() { // main function
    
    init(); // initialize system
//    while(1);
    while (1) {
        if (INTGetFlag(INT_T2)){
            INTClearFlag(INT_T2);
            timer++;
        }

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
    OpenTimer3(T3_ON | T3_IDLE_CON | T3_SOURCE_INT | T3_PS_1_64 | T3_GATE_OFF, 19530);
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
    OledMoveTo(0,3);
    OledLineTo(79,3);
    OledMoveTo(0,11);
    OledLineTo(79,11);
    OledMoveTo(0,19);
    OledLineTo(79,19);
    OledMoveTo(8,27);
    OledDrawRect(97,28);
   
}
void game() {
    char buf[2];
    OledClearBuffer();
    setLine();
    while (CountDown > 0) {
        countDown();
        TarMove();
        disp_score(score, miss);
        OledUpdate();
        LATGCLR =0XF000; // clear led
     
    }

    if(CountDown == 0) {
        OledClearBuffer();
        OledSetCursor(2,0);
        OledPutString("GAME OVER");
        OledSetCursor(2,1);
        OledPutString("Your Score:");
        OledSetCursor(7,2);
        sprintf(buf, "%d",score);
        OledPutString(buf);
        OledSetCursor(1,3);
        OledPutString("congratulations");
        score = 0; // clear hit score
        miss = 0; // clear miss score
        pos_x = 0; // reset x pos
    }
    CountDown = 90; // reset countDown
    a = 8;         // reset clear pixel 
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

void TarMove(){
    OledDefUserChar(target_char, target);
    OledDefUserChar(line_char,line);
    pos_y = timer%3;
    if (pos_x > 9){
        Hit(pos_x,pos_y);
        // if (hit)   // user hit the target
        ledLit(pos_y); // pos_y=0, led1 on; pos_y=1, led2 on, pos_y=2, led3 on 
        // else       // user miss
        // ledLit(3); led 4 on
        score++;
        pos_x = 0;
    }
    OledSetCursor(pos_x,pos_y);
    OledPutChar(line_char);
    pos_x++;
    OledSetCursor(pos_x,pos_y);
    OledPutChar(target_char);
   
}



void Hit(pos_x, pos_y){
    OledDefUserChar(hit_char,hit);
    OledDefUserChar(blank_char,blank);
    OledSetCursor(pos_x,pos_y);
    OledPutChar(hit_char);
    delay(DELAY);
    OledSetCursor(pos_x,pos_y);
    OledPutChar(blank_char);
    
}

void countDown(){
    OledDefUserChar(blank_char, blank);
    int one_sec = 0;   
    char buf[2];
    while (one_sec<1000){
    if(INTGetFlag(INT_T2)) {
            INTClearFlag(INT_T2);
            one_sec++;
        }
    }
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
        a++;
        
}

void disp_score(int score, int miss){
    char buf[4];
    OledSetCursor(13, 0);
    sprintf(buf,"S:%d",score);
    OledPutString(buf);
    OledSetCursor(13, 1);
    sprintf(buf,"M:%d",miss);
    OledPutString(buf);
}

void ledLit(int num){
    int led = num+1+11; // led0 is bit 12  
    LATGSET = (1<< led);
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
