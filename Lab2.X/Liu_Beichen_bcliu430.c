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
#include <stdlib.h>
#include <plib.h>                       // Peripheral Library
#include "string.h"
#include "PmodOLED.h"
#include "OledChar.h"
#include "OledGrph.h"
#include "delay.h"
#include "myUART.h"
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

/*
 * TODO
 * how to get a random number 4bit 0-15 8bit 0-255
 * How to check enter?
 * 
 * 
 * 
 *
 */

void init();
bool getBTN1();
bool getBTN2();
void Menu(int num);
void statsDisplay(int num,int result);
void HDisplay(int num);
bool Timer4Input();

unsigned int stat4bit1,stat4bit2,stat4bit3;
unsigned int stat8bit1,stat8bit2,stat8bit3;
static int Time;
enum state {hd, stats, Hd4Bit,Hd8Bit, Stats4Bit, Stats8Bit,BackHD, BackStats};
enum state menu;
int main() {
    init();
   
    while (1) {
        if (INTGetFlag(INT_T4)) // Has roll-over occurred? (Has 1 ms passed?)
        {
            Time++;
            INTClearFlag(INT_T4); // Clear flag so we don't respond until it sets again
        }        switch(menu){
           
/*            
 *   This part is for HD            
 */  
            case hd: // main menu arroy at HD
                if(getBTN1()) {
                    Menu(2); 
                    menu= stats; 
                }
                else if(getBTN2()){
                    Menu(3);
                    menu = Hd4Bit;
                }
                break;
                
            case Hd4Bit: //HD menu arrow at bit4
                if(getBTN1()) {
                    Menu(4); 
                    menu= Hd8Bit; 
                }
                else if(getBTN2()){
                    HDisplay(1);
                }
                break;
                
            case Hd8Bit: //HD menu arrow at bit8
                if(getBTN1()) {
                    Menu(5); 
                    menu= BackHD; 
                }
                else if(getBTN2()){
                    HDisplay(2);
                }
                break;
            case BackHD: //HD menu arrow at Back
                if(getBTN1()){
                    Menu(3);
                    menu = Hd4Bit;
                }
                else if(getBTN2()){
                    Menu(1);
                    menu = hd;
                }
                break;
//end HD display

/*
 *  This is Statistics display
 */            
            case stats: //main menu arrow at STATs
                if (getBTN1()) {
                    Menu(1);
                    menu = hd;
                }
                else if (getBTN2()) {
                    Menu(6);
                    menu = Stats4Bit;
                }
                break;                
                
            case Stats4Bit: //STATs menu arrow at Bit4
                if (getBTN1()) {
                    Menu(7);
                    menu = Stats8Bit;
                } 
                else if (getBTN2()) {
                    statsDisplay(1,0);
                }
                
                break;
            case Stats8Bit: //STATs menu arrow at Bit8
                if (getBTN1()) {
                    Menu(8);
                    menu = BackStats;
                } else if (getBTN2()) {
                    statsDisplay(2,0);

                    
                }
                break;
                //show 8bit statistics;
                
            case BackStats: //STATs menu arrow at Back
                if(getBTN1()){
                    Menu(6);
                    menu = Stats4Bit;
                }
                else if(getBTN2()){
                    Menu(1);
                    menu = hd;
                }
                break;

//end statiscs display

        } //END SWITCH
    }//END WHILE
        
   return EXIT_SUCCESS;           // This return should never occur
} // end main  

// Initialize Timer2 so that it rolls over 10 times per second
void Timer2Init() {
    // The period of Timer 2 is (16 * 625)/(10 MHz) = 1 ms (freq = 10 Hz)
    OpenTimer2(T2_ON | T2_IDLE_CON | T2_SOURCE_INT | T2_PS_1_16 | T2_GATE_OFF, 624);
    INTClearFlag(INT_T2);
    return;
}
void Timer3Init() {
    // The period of Timer 3 is (16 * 62550)/(10 MHz) = 100 ms (freq = 10 Hz)
    OpenTimer3(T3_ON | T3_IDLE_CON | T3_SOURCE_INT | T3_PS_1_16 | T3_GATE_OFF, 62499);
    INTClearFlag(INT_T3);
    return;
}

// Initialize Timer4 so that it rolls over 1ms
void Timer4Init() {
    // The period of Timer 4 is (16 * 625)/(10 MHz) = 1 ms (freq = 10 Hz)
    OpenTimer4(T4_ON | T4_IDLE_CON | T4_SOURCE_INT | T4_PS_1_16 | T4_GATE_OFF, 624);
    INTClearFlag(INT_T4);
    return;
}

void init(){
       
    TRISGSET = 0xC0;     // For BTN 1 and 2: set pin 6 and 7 to 1 as input 
    TRISGCLR = 0xF000;   // For LEDs: configure PortG pin for output
    ODCGCLR  = 0xF000;   // For LEDs: configure as normal output (not open drain)
    LATGSET  = 0xf000;
    // Initialize PmodOLED, also Timer1 and SPI1
    DelayInit();
    OledInit();
    Timer2Init();
    Timer3Init();
    Timer4Init();
    initUART(UART1, 10000000, 9600);


    unsigned int timeCount=0;
    
    
         // Send a welcome message to the OLED display
    OledClearBuffer();
    OledSetCursor(0, 0);          // upper-left corner of display
    OledPutString("ECE 2534");
    OledSetCursor(0, 1);          // column 0, row 1 of display
    OledPutString("Lab 2");
    OledSetCursor(0, 2);          // column 0, row 2 of display
    OledPutString("Security Sim");
    OledUpdate(); 
    
    while (timeCount <= 1000) {
        if (INTGetFlag(INT_T2)) // Has roll-over occurred? (Has 1 ms passed?)
        {
            timeCount++;
            INTClearFlag(INT_T2); // Clear flag so we don't respond until it sets again
        }

    }
    
    LATGCLR  = 0xf000;

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



void Menu(int num){
    if(num ==1){
        OledClearBuffer();
        OledSetCursor(0, 0); // upper-left corner of display
        OledPutString("Security Sim");
        OledSetCursor(0, 1); // column 0, row 1 of display
        OledPutString("-> HD");
        OledSetCursor(0, 2); // column 0, row 2 of display
        OledPutString("   STATs");
        OledUpdate(); }
    
    else if (num == 2){
        OledClearBuffer();
        OledSetCursor(0, 0); // upper-left corner of display
        OledPutString("Security Sim");
        OledSetCursor(0, 1); // column 0, row 1 of display
        OledPutString("   HD");
        OledSetCursor(0, 2); // column 0, row 2 of display
        OledPutString("-> STATs");
        OledUpdate(); }
    
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
        OledUpdate(); }
    
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
        OledUpdate(); }
    
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
        OledUpdate(); }
    
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
        OledUpdate(); }
    
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
        OledUpdate(); }
    
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
        OledUpdate(); }

} //end Menu

void statsDisplay(int num, int result){ 
    char bit4buf1[20];
    char bit4buf2[20];
    char bit4buf3[20];
    char bit8buf1[20];
    char bit8buf2[20];
    char bit8buf3[20];


    if (num ==1){
        //display 4 bit best 3 records;
        sprintf(bit4buf1, "1. %d min %d sec\n", stat4bit1/600, stat4bit1/10%60);
        sprintf(bit4buf2, "2. %d min %d sec\n", stat4bit2/600, stat4bit2/10%60);
        sprintf(bit4buf3, "3. %d min %d sec\n", stat4bit3/600, stat4bit3/10%60);
        OledClearBuffer();
        OledSetCursor(0, 0);          
        OledPutString("4-Bit Stats");
        OledSetCursor(0, 1);          
        OledPutString(bit4buf1);//first
        OledSetCursor(0, 2);          
        OledPutString(bit4buf2); //second 
        OledSetCursor(0, 3);          
        OledPutString(bit4buf3); //third 
        OledUpdate();
        
        while (!getBTN2());
        Menu(6);    
        menu=Stats4Bit; 
    }
        else if (num == 2){
        //display 4 bit best 3 records;
        sprintf(bit8buf1, "1. %d min %d sec\n", stat8bit1/600, stat8bit1/10%60);
        sprintf(bit8buf2, "2. %d min %d sec\n", stat8bit2/600, stat8bit2/10%60);
        sprintf(bit8buf3, "3. %d min %d sec\n", stat8bit3/600, stat8bit3/10%60);
        OledClearBuffer();
        OledSetCursor(0, 0);          
        OledPutString("8-Bit Stats");
        OledSetCursor(0, 1);          
        OledPutString(bit8buf1);//first
        OledSetCursor(0, 2);          
        OledPutString(bit8buf2); //second 
        OledSetCursor(0, 3);          
        OledPutString(bit8buf3); //third 
        OledUpdate();
        
        while (!getBTN2());
        Menu(7);    
        menu=Stats8Bit;

        }

    if(num ==3){
        if(stat4bit3 !=0){
            if (result < stat4bit1){
            stat4bit3 = stat4bit2;
            stat4bit2 = stat4bit1;
            stat4bit1 = result;
            }
            else if (result < stat4bit2){
                stat4bit3 = stat4bit2;
                stat4bit2 = result;
            }
            else if(result < stat4bit3){
                stat4bit3 = result;
            }
         }
         else if (stat4bit3 == 0) {
                if (stat4bit2 !=0){
                    if (stat4bit2 < result)
                        stat4bit3 = result;      //end if
                    else if (stat4bit1 > result){
                        stat4bit3 = stat4bit2;
                        stat4bit2 = stat4bit1;
                        stat4bit1 = result; } //end else if
                    else {
                        stat4bit3 = stat4bit2;
                        stat4bit2 = result; } //end else 
                } //end else if 
                else if (stat4bit2 ==0){
                    if (stat4bit1 != 0){
                        if (result < stat4bit1) {
                            stat4bit2 = stat4bit1;
                            stat4bit1 = result; } //end if
                        
                        else 
                            stat4bit2 = result;  //end else
                    } //end if
                    else if (stat4bit1 ==0){
                        stat4bit1 =result;   
                    } //end else if
                    
                }
                
            }
    }
    if(num ==4){
        if(stat8bit3 !=0){
            if (result < stat8bit1){
            stat8bit3 = stat8bit2;
            stat8bit2 = stat8bit1;
            stat8bit1 = result;
            }
            else if (result < stat8bit2){
                stat8bit3 = stat8bit2;
                stat8bit2 = result;
            }
            else if(result < stat8bit3){
                stat8bit3 = result;
            }
         }
         else if (stat8bit3 == 0) {
                if (stat8bit2 !=0){
                    if (stat8bit2 < result)
                        stat8bit3 = result;      //end if
                    else if (stat8bit1 > result){
                        stat8bit3 = stat8bit2;
                        stat8bit2 = stat8bit1;
                        stat8bit1 = result; } //end else if
                    else {
                        stat8bit3 = stat8bit2;
                        stat8bit2 = result; } //end else 
                } //end else if 
                else if (stat8bit2 ==0){
                    if (stat8bit1 != 0){
                        if (result < stat4bit1) {
                            stat8bit2 = stat8bit1;
                            stat8bit1 = result; } //end if
                        
                        else 
                            stat8bit2 = result;  //end else
                    } //end if
                    else if (stat8bit1 ==0){
                        stat8bit1 =result;   
                    } //end else if
                    
                }
                
            }
    }





    

    
}
void HDisplay(int num)
{
	Time = Time*rand();

	char newchar;
	int passwd;
	unsigned int i=0;
	unsigned int j;
	unsigned int k;
	unsigned int countDiff =0;
	char pwd_uart4[4];
	char pwd_uart8[8];
	char pwd4bit[4];
	char pwd8bit[8];
	int random;
	int timeCount=0;   
	int enter = 13;



    if (num ==1){


       for (j=0; j<4; j++){
            Time = Time/2;
            random = Time%2;
            if (random & 0x01)
                pwd4bit[j] = '1';
            else 
                pwd4bit[j] = '0'; }
  
        char buf[20];               // Temporary string for OLED display  
        OledClearBuffer();          //display 4 bit best 3 records;
        OledSetCursor(0, 0);          // upper-left corner of display
        OledPutString("4-Bit HD");
        OledSetCursor(0, 1);          
        OledPutString("****");
        OledUpdate();
        
        
        while(i<4){ 


            if (INTGetFlag(INT_T3)){            
                INTClearFlag(INT_T3);
                timeCount++; }
            
                // Display elapsed time in units of seconds, with decimal point
            sprintf(buf, "%d: %d.%d", timeCount/600, timeCount/10%60, timeCount%10);
            OledSetCursor(0, 3);
            OledPutString(buf);

            if((UARTReceivedDataIsAvailable (UART1))&&(i<4)) {
               newchar = UARTGetDataByte (UART1);
               if((newchar == '0')||(newchar == '1')) {
                    pwd_uart4[i] = newchar;
                    OledSetCursor(i, 1);
                    OledPutChar(newchar);
                    OledUpdate();
                    i++;
                }
                OledSetCursor(0,2);
                OledPutChar(newchar);
                OledUpdate();
          
            }
            //while(newchar != enter){} while not enter
            while ((i==4)) {
                countDiff = 0;
                for (k=0; k<4; k++){
                    if (pwd_uart4[k] != pwd4bit[k]){
                        countDiff++;
                    }
                }


                if (countDiff ==1) {
                    LATGCLR = 0XF000;
                    LATGSET = 0x1000;
                    i = 0;
                    OledSetCursor(0, 1);
                    OledPutString("****"); //add some buf here.
                    OledUpdate();
                }
                else if (countDiff ==2) {
                    LATGCLR = 0XF000;
                    LATGSET = 0x2000;
                    i = 0;
                    OledSetCursor(0, 1);
                    OledPutString("****"); //add some buf here.
                    OledUpdate();
                }
                else if (countDiff ==3) {
                    LATGCLR = 0XF000;
                    LATGSET = 0x3000;  
                    i = 0;
                    OledSetCursor(0, 1);
                    OledPutString("****"); //add some buf here.
                    OledUpdate();
                }
                else if (countDiff ==4){
                    LATGCLR = 0XF000;
                    LATGSET = 0x4000;
                    i = 0;
                    OledSetCursor(0, 1);
                    OledPutString("****"); //add some buf here.
                    OledUpdate();
                }

                
                if (countDiff == 0){
                    LATGCLR = 0XF000;
                    statsDisplay(3,timeCount);
                    while (!getBTN2()){}
                    menu = hd;
                    Menu(1);
                    i=5;
                    }
            }
        }
 
    }     
    
    else if (num == 2){
       for (j=0; j<8; j++){
            Time = Time/2;
            random = Time%2;
            if (random & 0x01)
                pwd8bit[j] = '1';
            else 
                pwd8bit[j] = '0'; }
        OledClearBuffer();
        char buf[20];               // Temporary string for OLED display  
      
        OledSetCursor(0, 0);          // upper-left corner of display
        OledPutString("8-Bit HD");
        OledSetCursor(0, 1);          
        OledPutString("********");
        OledUpdate();
        
        while(i<8) {
            if (INTGetFlag(INT_T3)){            
                // Timer2 has rolled over, so increment count of elapsed time
                INTClearFlag(INT_T3);
                timeCount++;

            }

                // Display elapsed time in units of seconds, with decimal point
            sprintf(buf, "%d: %d.%d", timeCount/600, timeCount/10%60, timeCount%10);
            OledSetCursor(0, 3);
            OledPutString(buf);
            OledUpdate();
            
            if((UARTReceivedDataIsAvailable (UART1))&&(i<8)) {
               newchar = UARTGetDataByte (UART1);
               if((newchar == '0')||(newchar == '1')) {
                    pwd_uart8[i] = newchar;
                    OledSetCursor(i, 1);
                    OledPutChar(newchar);
                    OledUpdate();
                    i++;
                }
                OledSetCursor(0,2);
                OledPutChar(newchar);
                OledUpdate();
         
        while ((i==8)) {
                countDiff = 0;
                for (k=0; k<8; k++){
                    if (pwd_uart8[k] != pwd8bit[k]){
                        countDiff++;
                    }
                }


                if (countDiff ==1) {
                    LATGCLR = 0XF000;
                    LATGSET = 0x1000;
                    i = 0;
                    OledSetCursor(0, 1);
                    OledPutString("********");
                    OledUpdate();
                }
                else if (countDiff ==2) {
                    LATGCLR = 0XF000;
                    LATGSET = 0x2000;
                    i = 0;
                    OledSetCursor(0, 1);
                    OledPutString("********");
                    OledUpdate();
                }
                else if (countDiff ==3) {
                    LATGCLR = 0XF000;
                    LATGSET = 0x3000;  
                    i = 0;
                    OledSetCursor(0, 1);
                    OledPutString("********");
                    OledUpdate();
                }
                else if (countDiff ==4){
                    LATGCLR = 0XF000;
                    LATGSET = 0x4000;
                    i = 0;
                    OledSetCursor(0, 1);
                    OledPutString("********");
                    OledUpdate();
                }
                else if (countDiff ==5) {
                    LATGCLR = 0XF000;
                    LATGSET = 0x5000;
                    i = 0;
                    OledSetCursor(0, 1);
                    OledPutString("********");
                    OledUpdate();
                }
                else if (countDiff ==6) {
                    LATGCLR = 0XF000;
                    LATGSET = 0x6000;  
                    i = 0;
                    OledSetCursor(0, 1);
                    OledPutString("********");
                    OledUpdate();
                }
                else if (countDiff ==7){
                    LATGCLR = 0XF000;
                    LATGSET = 0x7000;
                    i = 0;
                    OledSetCursor(0, 1);
                    OledPutString("********");
                    OledUpdate();
                }
                else if (countDiff ==8){
                    LATGCLR = 0XF000;
                    LATGSET = 0x8000;
                    i = 0;
                    OledSetCursor(0, 1);
                    OledPutString("********");
                    OledUpdate();
                }

                
                if (countDiff == 0){
                    LATGCLR = 0XF000;
                    statsDisplay(4,timeCount);
                    while (!getBTN2()){}
                    menu = hd;
                    Menu(1);
                    i=9;
                    }
            }
               
                
            }
        
        }
    }//end while
}
/*
int Time(){
    int time =0;
    while(Timer4Input())
        time++;

    return time;
}*/




// srand(Time());
