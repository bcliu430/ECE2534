////////////////////////////////////////////////////////////////////////////////////
// ECE 2534:        hw4_prob2.c
//
// Description:     This program is written to simulate a simple timmer using 
//                  Interrupt Service Routine by configureing Timer23 to trigger 
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


unsigned int timer_count = 0;



void __ISR(_TIMER_3_VECTOR, IPL4AUTO) _Timer3Handler(void) {
    DBG_ON(MASK_DBG0);
    timer_count++; // Increment the millisecond counter.
    INTClearFlag(INT_T3); // Clear the Timer2 interrupt flag.
     DBG_OFF(MASK_DBG0);
}

void initTimer() {
    // Configure Timer 2 to request a real-time interrupt once per millisecond.
    // The period of Timer 2 is (256 * 39062)/(10 MHz) = 1s.
    // 0x9895 =39061
    OpenTimer23(T2_ON | T2_PS_1_256 | T2_32BIT_MODE_ON, 0x00009895);
    INTSetVectorPriority(INT_TIMER_3_VECTOR, INT_PRIORITY_LEVEL_4);
    INTClearFlag(INT_T3);
    INTEnable(INT_T3, INT_ENABLED);
}

int main() {

    // Initialize PmodOLED
    DelayInit();
    OledInit();
    DBG_INIT();

    initTimer();
    INTConfigureSystem(INT_SYSTEM_CONFIG_MULT_VECTOR);
    INTEnableInterrupts();

    // Send a welcome message to the OLED display
    OledClearBuffer();
    OledSetCursor(0, 0); 
    OledPutString("Time Elapsed = ");
    OledUpdate();
    while (1){
   
        char buf[17];
        sprintf(buf, "%d seconds", timer_count);
        OledSetCursor(5,2);
        OledPutString(buf); 
        OledUpdate();

    

    }
}
