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
//
// Written by:      Beichen Liu 刘北辰
//
// Last modified:   10.21.2016
//
// Version:         ver.10-21-2016 create file, setup menu
//                  ver.10.30-2016 setup adc, paddle, score counting 
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
 * 1.  pixel move
 * 2.  reset if the ball does not hit the paddle
 * 3.  ball speed 
 * 4.  init time for 5 second
 * 
 */

enum state {score5, score10, score20,confirm5, confirm10,confirm20,back5,back10,back20};
enum state sysState;


/*
 *  Global variable 
 */
volatile int LR_value, UD_value;
volatile unsigned int timer2_ms_value = 0;

static int a=13,b=14,c=15,d=16; //paddle position
static int o=126, p=127;

static unsigned int x=66,y=15; //ball position
static unsigned int spd_x=2,spd_y=2; //ball speed
static unsigned int xpos=65,ypos=15;
// Definitions for the ADC averaging. How many samples (should be a power
// of 2, and the log2 of this number to be able to shift right instead of
// divide to get the average.
#define NUM_ADC_SAMPLES 32
#define LOG2_NUM_ADC_SAMPLES 5

/*
 *   ADC Configuration
 */

#define AD_MUX_CONFIG ADC_CH0_POS_SAMPLEA_AN2   |\
                      ADC_CH0_NEG_SAMPLEA_NVREF |\
                      ADC_CH0_POS_SAMPLEB_AN3   |\
                      ADC_CH0_NEG_SAMPLEB_NVREF
#define AD_CONFIG1 ADC_FORMAT_INTG |\
                   ADC_CLK_TMR     |\
                   ADC_AUTO_SAMPLING_ON

#define AD_CONFIG2 ADC_VREF_AVDD_AVSS    |\
                   ADC_SCAN_OFF          |\
                   ADC_SAMPLES_PER_INT_2 |\
                   ADC_ALT_BUF_ON        |\
                   ADC_ALT_INPUT_ON
#define AD_CONFIG3 ADC_SAMPLE_TIME_8 |\
                   ADC_CONV_CLK_20Tcy

#define AD_CONFIGPORT ENABLE_AN2_ANA|\
                      ENABLE_AN3_ANA

#define AD_CONFIGSCAN SKIP_SCAN_ALL

//end ADC configuration


// function declaraction
void init();
void Menu(int num);
bool getBTN1();
bool getBTN2();
void game(int num);


int main() { // main function
    
    
    init(); // initialize system
    
//    while(1) { // used for debugging
//        ballmoving(); }

    while (1) {
        switch(sysState){
            case score5:
                if(UD_value<300){
                    sysState = score10;
                    menu(2);
                }
                else if(UD_value>700){
                    sysState = score20;
                    menu(3);
                } 
                else if (LR_value>700){
                    sysState = confirm5;
                    menu(4);
                }
                break;
            
            case score10:
                if(UD_value<300){
                    sysState = score20;
                    menu(3);
                }
                if(UD_value>700){
                    sysState = score5;
                    menu(1);
                }
                else if (LR_value>700){
                    sysState = confirm10;
                    menu(5);
                }
                break;
            
            case score20:
                if(UD_value<300){
                    sysState = score5;
                    menu(1);
                }
                else if(UD_value>700){
                    sysState = score10;
                    menu(2);
                }
                else if (LR_value>700){
                    sysState = confirm20;
                    menu(6);
                }
                break;
            
            case confirm5:
                if(UD_value<300 || UD_value>700){
                    sysState = back5;    
                    menu(7);
   
                }
                else if (LR_value>700){
                    game(5);
                }
                break;
            
            case confirm10:
                if(UD_value<300 || UD_value>700){
                    sysState = back10;    
                    menu(8);
   
                }
                else if (LR_value>700){
                    game(10);
                }
                break;
                
            case confirm20:
                if(UD_value<300 || UD_value>700){
                    sysState = back20;    
                    menu(9);   
                }
                else if (LR_value>700){
                    game(20);
                } 
                break;
            case back5:
                if(UD_value<300 || UD_value>700){
                    sysState = confirm5;    
                    menu(4);
   
                }
                else if (LR_value>700){
                    sysState = score5;
                    menu(1);
                }
                break;
            
            case back10:
                if(UD_value<300 || UD_value>700){
                    sysState = confirm10;    
                    menu(5);
   
                }
                else if (LR_value>700){
                    sysState = score10;
                    menu(4);

                } 
                break;
                
            case back20:
                if(UD_value<300 || UD_value>700){
                    sysState = confirm20;    
                    menu(6);   
                }
                else if (LR_value>700){
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
    OpenTimer3(T3_ON | T3_IDLE_CON | T3_SOURCE_INT | T3_PS_1_16 | T3_GATE_OFF, 624);
    INTSetVectorPriority(INT_TIMER_3_VECTOR, INT_PRIORITY_LEVEL_4);
    INTClearFlag(INT_T3);
    INTEnable(INT_T3, INT_ENABLED);    // The period of Timer 4 is (16 * 625)/(10 MHz) = 1 ms (freq = 10 Hz)
    OpenTimer4(T4_ON | T4_IDLE_CON | T4_SOURCE_INT | T4_PS_1_16 | T4_GATE_OFF, 62499);
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
    //TimerInit();
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
    INTEnable(INT_T3, INT_ENABLED);    // The period of Timer 4 is (16 * 625)/(10 MHz) = 1 ms (freq = 10 Hz)
    OpenTimer4(T4_ON | T4_IDLE_CON | T4_SOURCE_INT | T4_PS_1_256 | T4_GATE_OFF, 39061);
    INTClearFlag(INT_T4);
    
    // ADC initialization
    SetChanADC10(AD_MUX_CONFIG);
    OpenADC10(AD_CONFIG1, AD_CONFIG2, AD_CONFIG3, AD_CONFIGPORT, AD_CONFIGSCAN);
    EnableADC10();
    
    INTSetVectorPriority(INT_ADC_VECTOR, INT_PRIORITY_LEVEL_7);
    INTClearFlag(INT_AD1);
    INTEnable(INT_AD1, INT_ENABLED);
    INTConfigureSystem(INT_SYSTEM_CONFIG_MULT_VECTOR);
    INTEnableInterrupts();

    //end ADC initialization
    
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

    enum dir {TL,TR,BL,BR};
    enum dir direction = TR;

    int score = 0;
    char SCORE[1];
    char COUNTDOWN[20];
    char MSG[20];
    char win[20];
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
        if (INTGetFlag(INT_T4)) {
            time++;
            count--;
            INTClearFlag(INT_T4); 
        } 
    }
    
   OledClearBuffer();
   setBack();

    while(score<num){
        paddle();
	    OledMoveTo(xpos,ypos);
	    OledDrawPixel();
	    OledUpdate();
        if (INTGetFlag(INT_T2)){
            INTClearFlag(INT_T2);
            OledMoveTo(xpos,ypos);
            OledClearPixel();
            xpos -= spd_x;
            ypos -= spd_y; 
            OledMoveTo(xpos,ypos);
            OledDrawPixel();
        }
        if (xpos < 4)
            spd_x = -spd_x;
        if (ypos < 4)
            spd_y = -spd_y;
        if (xpos > 126)
            spd_x = -spd_x;
        if (ypos > 28)
            spd_y = -spd_y;

/*	    switch (direction){
		case TL:
	        OledClearPixel();
		    x-=3;
//		    y-=1;
		    if (x < 10){
                x+=1;
//                y-=1;
		        direction = TR;}
		    if (y < 3){
                x-=1;
//                y-=1;
		        direction = BL; }

		break;
		case TR:
	        OledClearPixel();
		    x+=3;
//		    y-=1;
		    if ((x == 126)&& !(y==a||y==b||y==c||y==d)){
                x = 66;
                y = 15;
		        direction = TL;
            }
            else{
                x-=3;
 //               y-=1;
                direction = TL;}
		    if (y < 3 ) {
                x+=1;
//                y+=1;
		        direction = BR; }
		break;
		case BL:
	        OledClearPixel();
		    x-=3;
//            y+=1;
		    if (y > 28 ) {
                x-=3;
//                y-=1;
		        direction = TL; }
		    if (x < 3){
                x+=3;
//                y+=1;
		        direction = BR;}
		break;
		case BR:
	        OledClearPixel();
		    x += 3;
//		    y += 1;
		    if (y > 28 ){
                x+=3;
 //            y-=1;
		        direction = TR;}
            if ((x == 126 ) && !(y==a||y==b||y==c||y==d)){
                x = 66;
 //               y = 15;
                direction = TL;
            }
            else{
                x-=3;
//                y+=1;
		        direction = BL;}
		    break;    
	    }*/




        if( (xpos>125) && ((a==ypos)||(b==ypos)||(c==ypos)||(d==ypos))){ // check if the ball touches the paddle            
            score =score+1;
        }
        if ((xpos > 125) && !(ypos ==a)&& !(ypos==b) && !(ypos==c) && !(ypos==d)){
            OledMoveTo(xpos,ypos);
            OledClearPixel();
            xpos=66;
            ypos=15;
            spd_x=2;
            spd_y=2;}
        sprintf (SCORE, "%d" , score);
        OledSetCursor(9,1);
        OledPutString(SCORE);
        OledUpdate();
        /*
     if(INTGetFlag(INT_T4)){
            INTClearFlag(INT_T4); 
        }*/
    }
    
   while(score == num){
       OledSetCursor(1,1);
       OledPutString( " player 1 win");
       OledUpdate();
       if( getBTN2()) {
            menu(1);
            sysState = score5;
            score = 0;
       } 
   }
}


void setBack() { //setup the game background

    int i;
    OledClearBuffer();
    /*
     * draw a rectangle;
     */
    OledMoveTo(0, 0);
    OledDrawRect(127, 1); //top
    OledMoveTo(0, 30);
    OledDrawRect(127, 31); //bottom
    OledMoveTo(0, 2);
    OledDrawRect(1, 29); // left
    /*
     * setup middle line 
     */
    for (i =3; i<32; i++) {
        OledMoveTo(63, i);
        OledDrawRect(63, i+1);
        i += 3;
    }

    OledUpdate();
}




void paddle(){
    
    OledMoveTo(o,a);
    OledDrawPixel();
    OledMoveTo(o,b);
    OledDrawPixel(); 
    OledMoveTo(o,c);
    OledDrawPixel();
    OledMoveTo(o,d);
    OledDrawPixel(); 
    OledMoveTo(p,a);
    OledDrawPixel();
    OledMoveTo(p,b);
    OledDrawPixel();
    OledMoveTo(p,c);
    OledDrawPixel();
    OledMoveTo(p,d);
    OledDrawPixel();
    OledUpdate();
    
    if (a>=1 && UD_value >700){ //moving up
        OledMoveTo(o,a);
        OledClearPixel();
        OledMoveTo(o,b);
        OledClearPixel();        
        OledMoveTo(o,c);
        OledClearPixel();        
        OledMoveTo(o,d);
        OledClearPixel();
        OledMoveTo(p,a);
        OledClearPixel();
        OledMoveTo(p,b);
        OledClearPixel();        
        OledMoveTo(p,c);
        OledClearPixel();        
        OledMoveTo(p,d);
        OledClearPixel();
        
        a-=2;
        b-=2;
        c-=2;
        d-=2;
    }
    if (d<=31 && UD_value <300){
        OledMoveTo(o,a);
        OledClearPixel();
        OledMoveTo(o,b);
        OledClearPixel();        
        OledMoveTo(o,c);
        OledClearPixel();        
        OledMoveTo(o,d);
        OledClearPixel();
        OledMoveTo(p,a);
        OledClearPixel();
        OledMoveTo(p,b);
        OledClearPixel();        
        OledMoveTo(p,c);
        OledClearPixel();        
        OledMoveTo(p,d);
        OledClearPixel();
        
        
        a+=2;
        b+=2;
        c+=2;
        d+=2;
        
    }
    OledUpdate();
}


/*
 *   ADC setup 
 */

void __ISR(_ADC_VECTOR, IPL7SRS) _ADCHandler(void) {
    static unsigned int current_reading = 0;
    static unsigned int ADC_LR_Readings[NUM_ADC_SAMPLES], ADC_UD_Readings[NUM_ADC_SAMPLES];
    unsigned int i;
    unsigned int LRtotal, UDtotal;

    if (ReadActiveBufferADC10()) {
        ADC_LR_Readings[current_reading] = ReadADC10(0);
        ADC_UD_Readings[current_reading] = ReadADC10(1);
    }
    else {
        ADC_LR_Readings[current_reading] = ReadADC10(8);
        ADC_UD_Readings[current_reading] = ReadADC10(9);
    }

    current_reading++;
    if (current_reading == NUM_ADC_SAMPLES) {
        current_reading = 0;
        LRtotal = 0;
        UDtotal = 0;
        for (i = 0; i < NUM_ADC_SAMPLES; i++) {
            LRtotal += ADC_LR_Readings[i];
            UDtotal += ADC_UD_Readings[i];
        }
        LR_value = LRtotal >> LOG2_NUM_ADC_SAMPLES; // divide by num of samples
        UD_value = UDtotal >> LOG2_NUM_ADC_SAMPLES; // divide by num of samples
    }
    INTClearFlag(INT_AD1);
}

void __ISR(_TIMER_3_VECTOR, IPL4AUTO) _Timer3Handler(void) {
    timer2_ms_value++; // Increment the millisecond counter.
    INTClearFlag(INT_T3); // Acknowledge the interrupt source by clearing its flag.
}

