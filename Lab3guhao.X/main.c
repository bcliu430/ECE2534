////////////////////////////////////////////////////////////////////////////////////
// ECE 2534:        Lab 3                                                         
// Name:            Hao Gu                                                        
// File name:       main.c                                                        
// Description:     Create a Minesweeper Game                                     
//                  Push bottoms 1 and 2 to select the difficulty of the game.         
//                  Display the remaining time on the Pmod Oled board.            
//                  If all the bombs have been found and flag. And all the places 
//                  with no bombs been revealed. You win the game.
//                  If you step on a bomb or didn't finish within the designed gametime
//                  You lose.
// Resources:       delay.c uses Timer1 to provide delays with increments of 1 ms.               
//					PmodOLED.c uses SPI1 for communication with the OLED.        
// Written by:      Hao Gu                                                       
// Last modified:   3/31/16                                                        
////////////////////////////////////////////////////////////////////////////////////
#define GetPBCClock() 10000000
#define BaudRate 9600
#define _PLIB_DISABLE_LEGACY
#include <plib.h>
#include <stdlib.h>                
#include "PmodOLED.h"                           
#include "OledChar.h"                            
#include "timer.h"
#include "int.h"            
#include "OledGrph.h"            
#include "delay.h"                       
#include "Digilent_config.h" 
#include "adc10.h"

#define _PLIB_DISABLE_LEGACY  //legacy code not allowed in this course
#define BUTTON1 1<<6
#define BUTTON2 1<<7
#define BUTTON3 1<<0
#define LEDS 0xF<<12

//define all the costumized characters
BYTE BlockDef[] = {0x00, 0x7E, 0x7E, 0x7E, 0x7E, 0x7E, 0x7E, 0x00};
BYTE CreeperDef[] = {0x00, 0x06, 0x76, 0x38, 0x38, 0x76, 0x06, 0x00};
BYTE EmptyDef[] = {0x00, 0x3C, 0x42, 0x42, 0x42, 0x42, 0x3C, 0x00};
BYTE OneDef[] = {0x00, 0x00, 0x00, 0x00, 0x7E, 0x00, 0x00, 0x00};
BYTE FlagDef[] = {0x00, 0x10, 0x18, 0x1C, 0x1E, 0x7E, 0x00, 0x00};
BYTE BombDef[] = {0x00 ,0x42, 0x24, 0x18, 0x18, 0x24, 0x42, 0x00};
BYTE BlockCurDef[] = {0xFF, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0xFF};
BYTE CurOneDef[] = {0xFF, 0xFF, 0xFF, 0xFF, 0x81, 0xFF, 0xFF, 0xFF};
BYTE CurEmptyDef[]= {0xFF, 0xC3, 0xBD, 0xBD, 0xBD, 0xBD, 0xC3, 0xFF};
BYTE CurFlagDef[] = {0xFF, 0xEF, 0xE7, 0xE3, 0xE1, 0x81, 0xFF, 0xFF};
BYTE CurBombDef[] = {0xFF, 0xBD, 0xDB, 0xE7, 0xE7, 0xDB, 0xBD, 0xFF};
BYTE TwoDef[] = {0x00, 0x3A, 0x2A, 0x2A, 0x2A, 0x2A, 0x2E, 0x00};
BYTE CurTwoDef[] = {0xFF, 0xC5, 0xD5, 0xD5, 0xD5, 0xD5, 0xD1, 0xFF};
BYTE ThreeDef[] = {0x00, 0x2A, 0x2A, 0x2A, 0x2A, 0x2A, 0x3E, 0x00};
BYTE CurThreeDef[] = {0xFF, 0xD5, 0xD5, 0xD5, 0xD5, 0xD5, 0xC1, 0xFF};

static float VoltLR = 0;//read number for L/R joystick
static float VoltUD = 0;//read number for U/D joystick
static unsigned int select = 0;
static char Block= 0x00;
static char Creeper =0x01;
static char Empty = 0x02;
static char One = 0x03;
static char Flag = 0x04;
static char Bomb = 0x05;
static char BlockCur = 0x06;
static char CurOne = 0x07;
static char CurEmpty = 0x08;
static char CurFlag = 0x09;
static char CurBomb = 0x0A;
static char Two = 0x0B;
static char CurTwo = 0x0C;
static char Three = 0x11;
static char CurThree = 0x12;
static int gametime = 0;
static int BombNum = 0;

void init();
int ReadButton1();
int ReadButton2();
int ReadButton3();
void SetReveal(int c, int r,int array[]);
void SetCursor(int c, int r,int array[]);
int CheckFlag(int array[], int array2[]);

int main( int argc, char **argv)
{
	
    init();
    DDPCONbits.JTAGEN = 0; // to disable the JTAG controller and enable bottom 3
    //20Hz
    INTConfigureSystem(INT_SYSTEM_CONFIG_MULT_VECTOR);
    INTEnable(INT_T3, INT_ENABLED);
    INTSetVectorPriority(INT_T3, INT_PRIORITY_LEVEL_7);
    INTClearFlag(INT_T3);
    INTEnableInterrupts();
    srand(ReadTimer3());

    
    OledDefUserChar(Block, BlockDef);
    OledDefUserChar(Creeper, CreeperDef);
    OledDefUserChar(Empty, EmptyDef);
    OledDefUserChar(One, OneDef);
    OledDefUserChar(Flag, FlagDef);
    OledDefUserChar(Bomb, BombDef);
    OledDefUserChar(BlockCur,BlockCurDef);
    OledDefUserChar(CurOne, CurOneDef);
    OledDefUserChar(CurEmpty, CurEmptyDef);
    OledDefUserChar(CurFlag, CurFlagDef);
    OledDefUserChar(CurBomb, CurBombDef);
    OledDefUserChar(CurTwo,CurTwoDef);
    OledDefUserChar(Two, TwoDef);
    OledDefUserChar(Three,ThreeDef);
    OledDefUserChar(CurThree,CurThreeDef);
    
    char buf[17];
    enum {start,wait,level,easy,medium,hard,game,Next,lose,win,waitwin, waitlose,timeup,clear} mode = start;
    int seconds = 0;
    int tenths = 0;;
    int i = 1;
    int j = 0;
    int k = 0;
    int array[40] = {0};
    int array2[40] = {0};
    int reveal[40] = {0};

    int count = 1;
    int R = 0;
    int C = 0;
    int a = 0;
    int b = 0;
    int x = 0;
    int d = 0;
    int tempC =0;
    int tempR = 0;
    int tempd = 0;
    int random = 0;
    int countdown = 0;
    OpenADC10(
            // turn on converter, 32 bit int format, start conversions with timer, sample the signal
            ADC_FORMAT_INTG|ADC_CLK_INT0|ADC_AUTO_SAMPLING_ON,
            // standard Vref & Vdd, no scanning,  1 sample per interupt, use regular input
            ADC_VREF_AVDD_AVSS|ADC_OFFSET_CAL_DISABLE|ADC_SCAN_OFF|ADC_SAMPLES_PER_INT_2|ADC_ALT_BUF_ON|ADC_ALT_INPUT_ON,
            // fast conversion clock, use internal clock
            ADC_CONV_CLK_PB|ADC_SAMPLE_TIME_10,
            // enable AN2 for analog             4
            ENABLE_AN3_ANA|ENABLE_AN2_ANA,
            // no scanning
            SKIP_SCAN_ALL
    );    
    SetChanADC10( ADC_CH0_NEG_SAMPLEA_NVREF|ADC_CH0_POS_SAMPLEA_AN3|ADC_CH0_NEG_SAMPLEB_NVREF|ADC_CH0_POS_SAMPLEB_AN2);
    EnableADC10();
    
    
    
    while(1) {
        switch (mode){
            case start:                
                OledClearBuffer();
                OledSetCursor(0, 0);
                OledPutString("Minesweeper Game");
                OledSetCursor(0, 2);
                OledPutString("Hao Gu");
                OledUpdate();
                mode = wait;
                break;
            case wait:
                if (INTGetFlag(INT_T2)) 
                    {  //set the timer
                        INTClearFlag(INT_T2); 
                        tenths++;
                        if (tenths>=10) 
                        {
                            seconds++;
                            tenths = 0;                           
                        }
                        if (seconds > 3) mode = level;
                    }               
                
                
                break;
                
            case level:
                
                OledClearBuffer();
                OledSetCursor(0, 0);
                OledPutString("Choose Level:");
                OledSetCursor(3, 1);
                OledPutString("Easy");
                OledSetCursor(3, 2);
                OledPutString("Medium");
                OledSetCursor(3, 3);
                OledPutString("Hard");
                OledUpdate();
                mode = easy;
                break;
            case easy:
                OledSetCursor(1, 3);
                OledPutString(" ");
                OledSetCursor(1, 1);
                OledPutString("*");
                if (ReadButton1()) mode = medium;  
                if (ReadButton2()) {
                    mode = game;
                    gametime = 150;
                    BombNum = 3;
                }
                break;
                
            case medium:
                OledSetCursor(1, 1);
                OledPutString(" ");
                OledSetCursor(1, 2);
                OledPutString("*");
                if (ReadButton1()) mode = hard;   
                if (ReadButton2()) {
                    mode = game;
                    gametime = 125;
                    BombNum = 6;
                }
                break;
                        
            case hard:
                OledSetCursor(1, 2);
                OledPutString(" ");
                OledSetCursor(1, 3);
                OledPutString("*");
                if (ReadButton1()) mode = easy;  
                if (ReadButton2()) {
                    mode = game;
                    gametime = 100;
                    BombNum = 9;
                }
                break;
                
            case game:
                C = 0;
                R = 0;
                x = 0;
                tenths = 0;
                seconds = 0;
                while (count < 40){
                    array[count] = 0;
                    
                    reveal[count] = 0;
                    count++;
                }
                count = 0;
                while (count < BombNum){
                    random = rand()%40;
                    array[random] = 9;
                    count++;
                }
                
                count = 0;
                
                // to store information of number and bombs
                while (count < 40){
                    if (array[count]== 9) count++;
                    else{
                        if (count == 0){
                            if (array[count+1] == 9) x++;
                            if (array[count+10] == 9) x++;
                            if (array[count+11] == 9) x++;
                            array[count] = x;
                            count++;
                            x = 0;
                        }
                        else if (count == 10 || count == 20){
                            if (array[count-10] == 9) x++;
                            if (array[count-9] == 9) x++;
                            if (array[count+1] == 9) x++;
                            if (array[count+10] == 9) x++;
                            if (array[count+11] == 9) x++;
                            array[count] = x;
                            count++;
                            x = 0;
                        }
                        else if (count == 30) {
                            if (array[count-10] == 9) x++;
                            if (array[count-9] == 9) x++;
                            if (array[count+1] == 9) x++;
                            array[count] = x;
                            count++;
                            x = 0;
                        }
                        else if (count == 1 || count == 2 || count == 3 || count == 4 || count == 5 || count == 6 || count == 7 || count == 8 ){
                            if (array[count-1] == 9) x++;
                            if (array[count+1] == 9) x++;
                            if (array[count+9] == 9) x++;
                            if (array[count+10] == 9) x++;
                            if (array[count+11] == 9) x++;  
                            array[count] = x;
                            count++;
                            x = 0;
                        }
                        else if (count == 31 || count == 32 || count == 33 || count == 34 || count == 35 || count == 36 || count == 37 || count == 38 ){
                            if (array[count-11] == 9) x++;
                            if (array[count-10] == 9) x++;
                            if (array[count-9] == 9) x++;
                            if (array[count-1] == 9) x++;
                            if (array[count+1] == 9) x++;
                            array[count] = x;
                            count++;
                            x = 0;
                        }
                        else if (count == 19 || count == 29){
                            if (array[count-11] == 9) x++;
                            if (array[count-10] == 9) x++;
                            if (array[count-1] == 9) x++;
                            if (array[count+9] == 9) x++;
                            if (array[count+10] == 9) x++;
                            array[count] = x;
                            count++;
                            x = 0;
                        }
                        else if (count == 9){
                            if (array[count-1] == 9) x++;
                            if (array[count+9] == 9) x++;
                            if (array[count+10] == 9) x++;
                            array[count] = x;
                            count++;
                            x = 0;
                        }
                        else if (count == 39){
                            if (array[count-11] == 9) x++;
                            if (array[count-10] == 9) x++;
                            if (array[count-1] == 9) x++;
                            array[count] = x;
                            count++;
                            x = 0;
                        }
                        else {
                            if (array[count-11] == 9) x++;
                            if (array[count-10] == 9) x++;
                            if (array[count-9] == 9) x++;
                            if (array[count-1] == 9) x++;
                            if (array[count+1] == 9) x++;
                            if (array[count+9] == 9) x++;
                            if (array[count+10] == 9) x++;
                            if (array[count+11] == 9) x++;  
                            array[count] = x;
                            count++;
                            x = 0;
                        }
                    }
                    
                }
                
                
                OledClearBuffer();
                
                //plot 40 blocks to initialize the game
                while (k < 4){                   
                    while (j < 10){
                        
                            OledSetCursor(j,k);
                            OledDrawGlyph(Block);
                            OledUpdate();
                        j++;
                    }
                    k++;
                    j = 0;
                }               
                OledSetCursor(C,R);
                OledPutChar(BlockCur);
                mode = Next;
                k = 0;
                j = 0;
                break;
            case Next:
      
                count = 0;
                // if button1 is been pressed, then set cursor to it and record
                // the value in reveal array
                if (ReadButton1()){
                    //reveal[d] = 2;
                    if (array[d] == 0){
                        OledSetCursor(C,R);
                        OledPutChar(CurEmpty);
                        reveal[d] = 2;
                    }
                    else if (array[d] == 1){
                        OledSetCursor(C,R);
                        OledPutChar(CurOne);
                        reveal[d] = 3;
                    }
                    else if (array[d] == 2){
                        OledSetCursor(C,R);
                        OledPutChar(CurTwo);
                        reveal[d] = 6;
                    }
                    else if (array[d] == 3){
                        OledSetCursor(C,R);
                        OledPutChar(CurThree);
                        reveal[d] = 7;
                    }
                    else if (array[d]== 9){
                        OledSetCursor(C,R);
                        OledPutChar(CurBomb);
                        mode = waitlose;
                        tenths = 0;
                        seconds = 0;
                    }
                    
                }
                // if button2 is been pressed, put a flag on it and record the value
                if (ReadButton2()){
                    if (reveal[d] == 0){
                        OledSetCursor(C,R);
                        OledPutChar(CurFlag);
                        reveal[d] = 4;
                    }
                    else if (reveal[d] == 4){
                        OledSetCursor(C,R);
                        OledPutChar(BlockCur);
                        reveal[d] = 0;
                    }
                }
                //if joystick moves, move the cursor to related position
                    if (VoltLR < 300 && C > 0) {
                        C--;
                        SetCursor(C, R, reveal);
                        SetReveal(C+1,R ,reveal);
                    }
                    else if (VoltLR > 700 && C < 9) {
                        C++;
                        SetCursor(C, R, reveal);
                        SetReveal(C-1,R ,reveal);
                    }
                    else if (VoltUD < 300 && R < 3) {
                        R++;
                        SetCursor(C, R, reveal);
                        SetReveal(C,R-1 ,reveal);
                        
                    }
                    else if (VoltUD > 700 && R > 0) {
                        R--;
                        SetCursor(C, R, reveal);
                        SetReveal(C,R+1 ,reveal);
                    }
                    d = 10* R+ C;
                    
                    if (INTGetFlag(INT_T2)) 
                    {  //set the timer
                        INTClearFlag(INT_T2); 
                        tenths++;
                        if (tenths>=10) 
                        {
                            seconds++;
                            tenths = 0;                           
                        }
                        countdown = gametime - seconds;
                        if (countdown < 1) {
                            mode = lose;
                            tenths = 0;
                            seconds = 0;
                        }
                    } 
                    
                    
                    sprintf(buf, "     ");
                    OledSetCursor(11 ,0);
                    OledPutString(buf);
                
                    sprintf(buf, "TIME ");
                    OledSetCursor(11 ,1);
                    OledPutString(buf);
                    
                    sprintf(buf, "%03d", countdown);
                    OledSetCursor(12 ,2);
                    OledPutString(buf);
                    
                    sprintf(buf, "     ");
                    OledSetCursor(11 ,3);
                    OledPutString(buf);
                                        
                    if (CheckFlag(array, reveal)) {
                        mode = waitwin;
                        tenths = 0;
                        seconds = 0;
                    }   
                    
                    if (ReadButton3()){
                        mode = waitlose;
                        tenths = 0;
                        seconds = 0;
                    }
                break;
            case lose:
                OledClearBuffer();
                OledSetCursor(0, 0);
                OledPutString("You Lose!");
                OledSetCursor(0, 1);
                OledPutString("Step on A Bomb!");
                mode = clear;
                break;
                
            case win:
                OledClearBuffer();
                OledSetCursor(0, 0);
                OledPutString("You Win!");
                mode = clear;
                break;
                
            case waitwin:
                if (INTGetFlag(INT_T2)) 
                    {  //set the timer
                        INTClearFlag(INT_T2); 
                        tenths++;
                        if (tenths>=10) 
                        {
                            seconds++;
                            tenths = 0;                           
                        }
                        if (seconds > 2) mode = win;
                    } 
                break;
                
            case waitlose:
                if (INTGetFlag(INT_T2)) 
                    {  //set the timer
                        INTClearFlag(INT_T2); 
                        tenths++;
                        if (tenths>=10) 
                        {
                            seconds++;
                            tenths = 0;                           
                        }
                        if (seconds > 2) mode = lose;
                    } 
                break;
            case timeup:
                OledClearBuffer();
                OledSetCursor(0, 0);
                OledPutString("You Lose!");
                OledSetCursor(0, 1);
                OledPutString("Time is Up");
                mode = clear;
                break;
            case clear:
                tenths = 0;
                seconds = 0;
                mode = wait;
                break;
        }
        
    }
         
	return(0);
}

//Interrupt service routine

void __ISR(_TIMER_3_VECTOR, ipl7) TimerISR(void) {
    select = 8 * ((~ReadActiveBufferADC10() & 0x01)); 
    VoltUD = ReadADC10(select); 
    VoltLR = ReadADC10(select + 1); 
    INTClearFlag(INT_T3);
}

// *****************************************************************************
/* initialization function

  Summary:
 Initialize Timer2 and Timer3
 * 
 * Description:
 * Initialize Oled
  
*/
void init(){
    
    TRISGSET = BUTTON1 | BUTTON2;     // inputs
    TRISASET = BUTTON3;
    TRISBSET = 0x0C;//configure the first two pins as inputs
    TRISBCLR = 0x40;//configure the sixth bit as output
    ODCBCLR = 0x40;
    PORTB = PORTB | 0x40;

   // Initialize Timer1 and OLED
    DelayInit();
    OledInit();

   // Set up Timer2 to roll over every 0.1 second
    OpenTimer2(T2_ON         |
              T2_IDLE_CON    |
              T2_SOURCE_INT  |
              T2_PS_1_64     |
              T2_GATE_OFF,
              15624);  // freq = 10MHz/64/15625 = 10 Hz
    INTClearFlag(INT_T2);  //clear timer 2 rollover flag
    
    OpenTimer3(T3_ON        |          // timer 3 on
             T3_IDLE_CON    |           // don't stop on idle
             T3_SOURCE_INT  |           // Use internal clock
             T3_PS_1_64     |           // Prescale = 32
             T3_GATE_OFF,               // No gate control on timer
             15625);                      // counter register
    INTClearFlag(INT_T3);
    
}

// *****************************************************************************
/* Button1 debouncing function
  
*/
int ReadButton1(){
    int preview = 0;
    int cur = PORTG & BUTTON1;
    if (cur == preview) return 0;
    DelayMs(60);
    cur = PORTG & BUTTON1;
    if (cur == preview) return 0;
    preview = cur;
    if (cur) return 1;
    return 0;
}
// *****************************************************************************
/* Button2 debouncing function
  
*/
int ReadButton2(){
    int preview = 0;
    int cur = PORTG & BUTTON2;
    if (cur == preview) return 0;
    DelayMs(60);
    cur = PORTG & BUTTON2;
    if (cur == preview) return 0;
    preview = cur;
    if (cur) return 1;
    return 0;
}
// *****************************************************************************
/* Button3 debouncing function
  
*/
int ReadButton3(){
    int preview = 0;
    int cur = PORTA & BUTTON3;
    if (cur == preview) return 0;
    DelayMs(60);
    cur = PORTA & BUTTON3;
    if (cur == preview) return 0;
    preview = cur;
    if (cur) return 1;
    return 0;
}

// *****************************************************************************
/* SetCursor function

  Summary:
 When cursor goes to specific position on LED, check what is that and make it into
 highlight mode.
 * 
 * Description:
 * c is Colum. R is Row.
 * array is the array to store information about if certain position has been 
 * revealed
  
*/
void SetCursor(int c, int r,int array[]){
    int d = 10*r + c;
    
                        if (array[d] == 0){
                            OledSetCursor(c,r);
                            OledPutChar(BlockCur);
                        }
                        if (array[d] == 2){
                            OledSetCursor(c,r);
                            OledPutChar(CurEmpty);
                        }
                        if (array[d] == 3){
                            OledSetCursor(c,r);
                            OledPutChar(CurOne);
                        }
                        if (array[d] == 4){
                            OledSetCursor(c,r);
                            OledPutChar(CurFlag);
                        }
                        if (array[d] == 6){
                            OledSetCursor(c,r);
                            OledPutChar(CurTwo);
                        }
                        if (array[d] == 7){
                            OledSetCursor(c,r);
                            OledPutChar(CurThree);
                        }
}

// *****************************************************************************
/* SetReveal function

  Summary:
 When cursor goes to specific position on LED, check what is the previous state of LED 
 and make it into normal mode.
 * 
 * Description:
 * c is Colum. R is Row.
 * array is the array to store information about if certain position has been 
 * revealed
  
*/
void SetReveal(int c, int r,int array[]){
    int d = 10*r + c;
    
                        if (array[d] == 0){
                            OledSetCursor(c,r);
                            OledPutChar(Block);
                        }
                        if (array[d] == 2){
                            OledSetCursor(c,r);
                            OledPutChar(Empty);
                        }
                        if (array[d] == 3){
                            OledSetCursor(c,r);
                            OledPutChar(One);
                        }
                        if (array[d] == 4){
                            OledSetCursor(c,r);
                            OledPutChar(Flag);
                        }
                        if (array[d] == 6){
                            OledSetCursor(c,r);
                            OledPutChar(Two);
                        }
                        if (array[d] == 7){
                            OledSetCursor(c,r);
                            OledPutChar(Three);
                        }
}

// *****************************************************************************
/* CheckFlag function

  Summary:
 To check if you win the game by checking if every bomb has been flag and at the
 same time if every position with no bomb has not been flag.
 * 
 * Description:
 * array is the array to store bomb location
 * array2 is the array to store the flag information
  
*/

int CheckFlag(int array[], int array2[]){
    int count = 0;
    while (count < 40){
        if (array[count] == 9){
            if (array2[count] != 4) return 0;
        }
        if (array[count]!= 9){
            if (array2[count] == 4) return 0;
        }
        count++;
    }
    return 1;
}