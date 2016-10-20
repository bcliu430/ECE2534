////////////////////////////////////////////////////////////////////////////////////
// ECE 2534:        hw4_prob1.c
//
// Description:     This program is written to simulate a simple timmer using 
//                  INTGetFlag() by configureing Timer23 to trigger 
//                  the interrupt.
//
// Last modified:   10/20/2016 Beichen Liu

#include <stdio.h>                      // for sprintf()
#include <plib.h>                       // Peripheral Library
#include <stdbool.h>
#include "PmodOLED.h"
#include "OledChar.h"
#include "OledGrph.h"
#include "delay.h"
#include "myDebug.h"
#include "myBoardConfigFall2016.h"
#include "myLEDUpdate.h"


unsigned int timer2_count = 0;

int main() {

    // Initialize PmodOLED
    DelayInit();
    OledInit();

    // Grandma's Step #3
    // Initial Timer2
    initTimer();


    // Send a welcome message to the OLED display
    OledClearBuffer();
    OledSetCursor(0, 1); 
    OledPutString("Time Elapsed = ");
    OledUpdate();
    while (1){
        if(INTGetFlag(INT_T3)){
            timer_count++;
            INTClearFlag(INT_T3);
        }
    
        char buf[17];
        sprintf(buf, "%d seconds", timer_count);
        OledSetCursor(5,2);
        OledPutString(buf); 
        OledUpdate();

    

    }
}

void initTimer() {
    // Configure Timer 2 to request a real-time interrupt once per millisecond.
    // The period of Timer 2 is (256 * 39062)/(10 MHz) = 1ms.
    // 0x9895 =39061
    OpenTimer23(T2_ON | T2_IDLE_CON | T2_SOURCE_INT | T2_PS_1_256 | T2_32BIT_MODE_ON, 0x00009895);
    INTClearFlag(INT_T3);



}


