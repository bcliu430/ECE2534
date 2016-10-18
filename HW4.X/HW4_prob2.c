////////////////////////////////////////////////////////////////////////////////////
// ECE 2534:        Timer ISR Program Example
//
// Description:     This program use the Timer2 ISR to measure milliseconds.
//                  The example follows the five-step  "Grandma's ISR recipe"
//                  as introduced in class.
//
//                  The debugging is set up to show the following information:
//                      PORTE bit 0: Timer2 ISR
//                      PORTE bit 1: When LED1 is on
//                      PORTE bit 2: The OLED being updated
//                      PORTE bit 3: When the PWM duty cycle (in the LED code) resets
//
// Last modified:   9/23/2016 PEP

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

int main() {

    // Initialize PmodOLED
    DelayInit();
    OledInit();



    // Send a welcome message to the OLED display
    OledClearBuffer();
    OledSetCursor(0, 1); 
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

void initTimer23() {
    // Configure Timer 2 to request a real-time interrupt once per millisecond.
    // The period of Timer 2 is (256 * 39062)/(10 MHz) = 1s.
    // 0x9895 =39061
    OpenTimer23(T2_ON | T2_IDLE_CON | T2_SOURCE_INT | T2_PS_1_256 | T2_32BIT_MODE_ON, 0x00009895);
    INTSetVectorPriority(INT_TIMER_2_VECTOR, INT_PRIORITY_LEVEL_4);
    INTClearFlag(INT_T3);



}

void __ISR(_TIMER_2_VECTOR, IPL4AUTO) _Timer2Handler(void) {
    DBG_ON(MASK_DBG0); // Use debugging to see when the ISR is called on the LA/osc
    timer_count++; // Increment the millisecond counter.
    INTClearFlag(INT_T3); // Clear the Timer2 interrupt flag.
}


