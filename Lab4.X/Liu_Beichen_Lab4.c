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
// Last modified:   12.03.2016
//
// Version:         ver.11-20-2016 create file, setup menu, set time countdown
//                  ver.11-21-2016 setup target moving, score system and animition when hit target
//                                 setup leds
//                  ver.12-03-2016 setup ACL, configure double tap and twist, setup random number
//                                 add extra credit          
//                                 
////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                //
//                                             README                                             //
//                                                                                                // 
//         This game is controled by button 1, button 2, and accel. In the menu list, use         //
//         Button 1 to select up and down, and use button 2 to confirm.                           //
//                                                                                                //
//         In the game interface, you will hit the target using accel, left twist is used         //
//         to hit the target on third line, and led1 will on if you hit. double tap is for        //
//         second line and right twist is for the first line. if you miss, the forth led will     //
//         flash indicates that you miss one. when time is over, it will display your score on    //
//         Oled, push button2 to go back to the main interface.                                   //
//                                                                                                //
//         Extra Credit:                                                                          //
//         I add the new feature for the game, whenever your hit rate                             // 
//         hit/(hit+miss) is less than 50% you automatically lose the game, the oled will show    //
//         that you lose the game.                                                                //
//                                                                                                //
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
static int CountDown = 30; // initial game time is 90 second
static int a = 8;          // initial clear pixel variable
volatile unsigned int timer2 = 0;
static int score = 0;
static int miss = 0;
static int pos_x = 4, pos_y = 0;
static short y=0; // y axis from accel
static int one_sec = 0;   

BYTE blank[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
char blank_char = 0x00;

BYTE L[8] = {0x00, 0x40, 0x40, 0x40, 0x40, 0x40, 0x7C, 0x00};
char L_char = 0x10;
char D[8] = {0x00, 0x70, 0x48, 0x44, 0x44, 0x48, 0x70, 0x00};
char D_char = 0x11;
char R[8] = {0x00, 0x78, 0x48, 0x70, 0x60, 0x50, 0x48, 0x00};
char R_char = 0x12;
#define DELAY 300

static short axis [6]; // 2 bit for each axis x, y, z;
static int source;
bool doubletap;

// function declaraction
void init();
void Menu(int num);
bool getBTN1();
bool getBTN2();
void ClearPix(int xAixs);
void countDown();
void disp_score(int hit, int miss);
void initAccelerometer(SpiChannel chn);
void setAccelReg(SpiChannel chn, unsigned int address, unsigned int data);
int  getAccelReg(SpiChannel chn, unsigned int address);
void getAccelData(SpiChannel chn, short accelData[]);
bool RightTwist();
bool LeftTwist();
void initSPI(SpiChannel chn, unsigned int srcClkDiv);
void readData();
void game(int speed);
void hitarea();
void drawchar();
void lose();

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
                    game(1);
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
                    game(2);
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
    INTSetVectorPriority(INT_TIMER_2_VECTOR, INT_PRIORITY_LEVEL_4);
    INTClearFlag(INT_T2);
    INTEnable(INT_T2, INT_ENABLED);
    // The period of Timer 3 is (256 * 39032)/(10 MHz) = 1 s (freq = 1 Hz)
    OpenTimer3(T3_ON | T3_IDLE_CON | T3_SOURCE_INT | T3_PS_1_64 | T3_GATE_OFF, 19530);
    INTSetVectorPriority(INT_TIMER_3_VECTOR, INT_PRIORITY_LEVEL_4);
    INTClearFlag(INT_T3);
    INTEnable(INT_T3, INT_ENABLED);    // The period of Timer 4 is 1 s (freq = 1 Hz)
    OpenTimer4(T4_ON | T4_IDLE_CON | T4_SOURCE_INT | T4_PS_1_16 | T4_GATE_OFF, 624);
    INTClearFlag(INT_T4);



    
    // SPI and accel    
    initSPI(SPI_CHANNEL3,1024);
    initAccelerometer(SPI_CHANNEL3);
    TRISESET = 1<<8;
    PORTESET = 1<<8;
   //*/ 
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
    sysState = easy;
    menu(1); 

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
    OledMoveTo(0,0);
    OledLineTo(0,24);
    OledMoveTo(0,0);
    OledLineTo(71,0);
    OledMoveTo(0,8);
    OledLineTo(71,8);
    OledMoveTo(0,16);
    OledLineTo(71,16);
    OledMoveTo(0,24);
    OledLineTo(71,24);
    
    // count down
    OledMoveTo(8,27);
    OledDrawRect(97,28);

  
}

void hitarea(){
        // hit area
    OledMoveTo(72,0);
    OledDrawRect(79,7);
    OledMoveTo(72,8);
    OledDrawRect(79,15);
    OledMoveTo(72,16);
    OledDrawRect(79,23); 
}


void game(int speed) {
    float percent;
    char buf[2];
    OledClearBuffer();
    setLine();
    OledUpdate();

    while (CountDown > 0) {
        timer2++;
        int y_axis = 0;
        y_axis = 8*pos_y+1;

        
        drawchar(pos_x,y_axis);
        countDown();
        hitarea();

        clearchar(pos_x,y_axis);

        pos_x = pos_x+2*speed;
        drawchar(pos_x,y_axis);

            if (pos_x >=72 && pos_x<=79){
                getAccelData(SPI_CHANNEL3, axis);
                y = axis[2] | axis[3] << 8;
 
                source = getAccelReg(3,0x30);
                if (source &0x20) doubletap = TRUE;
                else doubletap = FALSE;
 
                if( (pos_y == 0) && (RightTwist()) ) { 
                    LATGSET = 0x4000;
                    score++;
                }
                else if  ( (pos_y == 1) && (doubletap) ) { 
                    LATGSET = 0x2000;
                    score++;
                }
                
                else if( (pos_y == 2) && (LeftTwist()) ) { 
                    LATGSET = 0x1000;
                    score++;
                }
                
                else{ 
                    LATGSET = 0x8000;
                    miss++; 
                }

                
                clearchar(pos_x,y_axis);

                pos_x = 1;
                pos_y = rand()%3;
                pos_y = rand()%3;
                pos_y = rand()%3;
                pos_y = rand()%3;
                pos_y = rand()%3;


                
            }
        if(score !=0 && miss !=0 && score <miss){
            lose();
            break;
        }



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
        pos_x = 1; // reset x pos 
        pos_y = rand()%3;
    }
    CountDown = 30; // reset countDown
    a = 8;         // reset clear pixel 
    while(!getBTN2());
    menu(1);
    sysState = easy;
}

void lose(){
        char buf[17];
        OledClearBuffer();
        OledSetCursor(2,0);
        OledPutString("GAME OVER");
        OledSetCursor(2,1);
        OledPutString("Your lose");
        OledSetCursor(7,2);
        score = 0; // clear hit score
        miss = 0; // clear miss score
        pos_x = 1; // reset x pos 
        pos_y = rand()%3;
    CountDown = 30; // reset countDown
    a = 8;         // reset clear pixel 
    while(!getBTN2());
    menu(1);
    sysState = easy;
    
}


void drawchar(int pos_x, int pos_y){
    int x1,x2,x3;
    int y1,y2,y3,y4,y5,y6;
    
    x1 = pos_x;
    x2 = pos_x+2;
    x3 = pos_x+3;
    
    y1 = pos_y;
    y2 = pos_y+1;
    y3 = pos_y+2;
    y4 = pos_y+3;
    y5 = pos_y+4;
    y6 = pos_y+5;

    
    OledMoveTo(x1,y1);
    OledDrawPixel();
    OledMoveTo(x2,y1);
    OledDrawPixel();
    OledMoveTo(x1,y2);
    OledDrawPixel();
    OledMoveTo(x3,y2);
    OledDrawPixel();
    OledMoveTo(x3,y3);
    OledDrawPixel();
    OledMoveTo(x3,y4);
    OledDrawPixel();
    OledMoveTo(x1,y5);
    OledDrawPixel();
    OledMoveTo(x3,y5);
    OledDrawPixel();
    OledMoveTo(x1,y6);
    OledDrawPixel();
    OledMoveTo(x2,y6);
    OledDrawPixel();


}

void clearchar(int pos_x, int pos_y){
    int x1,x2,x3;
    int y1,y2,y3,y4,y5,y6;
    
    x1 = pos_x;
    x2 = pos_x+2;
    x3 = pos_x+3;
    
    y1 = pos_y;
    y2 = pos_y+1;
    y3 = pos_y+2;
    y4 = pos_y+3;
    y5 = pos_y+4;
    y6 = pos_y+5;

    
    OledMoveTo(x1,y1);
    OledClearPixel();
    OledMoveTo(x2,y1);
    OledClearPixel();
    OledMoveTo(x1,y2);
    OledClearPixel();
    OledMoveTo(x3,y2);
    OledClearPixel();
    OledMoveTo(x3,y3);
    OledClearPixel();
    OledMoveTo(x3,y4);
    OledClearPixel();
    OledMoveTo(x1,y5);
    OledClearPixel();
    OledMoveTo(x3,y5);
    OledClearPixel();
    OledMoveTo(x1,y6);
    OledClearPixel();
    OledMoveTo(x2,y6);
    OledClearPixel();


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
            one_sec++;
        }
    if (one_sec == 10){
        CountDown--;
        ClearPix(a);
        ClearPix(a+1);
        ClearPix(a+2);
        a = a+3;
        one_sec =0;
    }
        if (CountDown < 10) {
            OledSetCursor(14,3);
            OledPutChar(blank_char);
            OledSetCursor(15,3);
        }
        else 
            OledSetCursor(14,3);
        
        sprintf(buf,"%d",CountDown);
        OledPutString(buf);

     
    OledUpdate();
        
}

void disp_score(int score, int miss){
    char buf[4];
    int percent;

    OledSetCursor(12, 0);
    sprintf(buf,"S:%d",score);
    OledPutString(buf);
    OledSetCursor(12, 1);
    sprintf(buf,"M:%d",miss);
    OledPutString(buf);
    OledDefUserChar(L_char, L);
    OledDefUserChar(D_char, D);
    OledDefUserChar(R_char, R);
    OledSetCursor(10,0);
    OledPutChar(R_char);
    OledSetCursor(10,1);
    OledPutChar(D_char);
    OledSetCursor(10,2);
    OledPutChar(L_char);
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

void initAccelerometer(SpiChannel chn) {
    setAccelReg(chn, 0x2C, 0x06); // addr_bw_rate
    setAccelReg(chn, 0x31, 0x2A); // addr_data_format
    setAccelReg(chn, 0x2D, 0x08); // addr_power_ctl
    setAccelReg(chn, 0x2E, 0x20); // INT_ENABLE
    setAccelReg(chn, 0x2F ,0x20); // INT_MAP
    setAccelReg(chn, 0x2A, 0x01);
    setAccelReg(chn, 0x1D ,0x30); // THRESH_TAP
    setAccelReg(chn, 0x21 ,0x10); // DUR
    setAccelReg(chn, 0x22 ,0x10); // latency time
    setAccelReg(chn, 0x23 ,0x90); // window register
    setAccelReg(chn, 0x2A ,0x01); // Z axis
}

void setAccelReg(SpiChannel chn, unsigned int address, unsigned int data) {
    SpiChnPutC (chn, address);
    SpiChnPutC (chn, data);
    SpiChnGetC (chn);
    SpiChnGetC (chn);
}

int getAccelReg(SpiChannel chn, unsigned int address) {
    SpiChnPutC(chn, 0x80 + address);
    SpiChnPutC(chn, 0);
    SpiChnGetC(chn);
    int value = SpiChnGetC(chn);
    
    return value;
}

void getAccelData(SpiChannel chn, short accelData[]) {
    SpiChnPutC(chn,(0x80 | 0x40 | 0x32));
    SpiChnPutC(chn,0);
    SpiChnPutC(chn,0);
    SpiChnPutC(chn,0);
    SpiChnPutC(chn,0);
    SpiChnPutC(chn,0);
    SpiChnPutC(chn,0);
    SpiChnGetC(chn);
    int i;
    for(i = 0; i < 6; i++) 
        accelData[i] = SpiChnGetC(chn);

}

bool RightTwist(){
    short thresh = -100;
    if (y < thresh){
        DelayMs(200);
        if (y < 100)   return TRUE;
        return FALSE;
    }


}

bool LeftTwist(){
    short thresh =100;
    if (y > thresh){
        DelayMs(200);
        if (y > 100)   return TRUE;
        return FALSE;
    }
}


void initSPI(SpiChannel chn, unsigned int srcClkDiv) {
    SpiChnOpen(chn, \
               SPI_OPEN_MSSEN | \
               SPI_OPEN_MSTEN | \
               SPI_OPEN_MODE8 | \
               SPI_OPEN_CKP_HIGH | \
               SPI_OPEN_ENHBUF, \
               srcClkDiv);
}



