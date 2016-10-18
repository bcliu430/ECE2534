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


unsigned int timer2_count = 0;

int main() {

    // Initialize PmodOLED
    DelayInit();
    OledInit();

    // Grandma's Step #3
    // Initial Timer2
    initTimer2();


    // Send a welcome message to the OLED display
    OledClearBuffer();
    OledSetCursor(0, 1); 
    OledPutString("Time Elapsed = ");
    OledUpdate();
    while (1){
        if(INTGetFlag(INT_T2)){
            timer2_count++;
            INTClearFlag(INT_T2);
        }
    
        char buf[17];
        sprintf(buf, "%d seconds", timer2_count);
        OledSetCursor(5,2);
        OledPutString(buf); 
        OledUpdate();

    

    }
}

void initTimer2() {
    // Configure Timer 2 to request a real-time interrupt once per millisecond.
    // The period of Timer 2 is (256 * 39062)/(10 MHz) = 1ms.
    OpenTimer2(T2_ON | T2_IDLE_CON | T2_SOURCE_INT | T2_PS_1_256 | T2_GATE_OFF, 39061);
    INTClearFlag(INT_T2);
//    OpenTimer2(T2_ON | T2_IDLE_CON | T2_SOURCE_INT | T2_PS_1_256 | T2_GATE_OFF, 39061);
//    INTClearFlag(INT_T2);


}


