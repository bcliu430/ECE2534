////////////////////////////////////////////////////////////////////////////////////
// ECE 2534:        Timer ISR Program Example
//
// Description:     This program use the Timer2 ISR to measure milliseconds.
//                  The example follows the five-step  "Grandma's ISR recipe"
//                  as introduced in class.
//
//                  The timer ISR is also used to update an LED using a 
//                  simple PWM (look it up!) scheme. The LED update is done
//                  in separate files (a definition and implementation).
//
//                  The program uses the OLED to display a counter that updates
//                  every NUMBER_OF_MILLISECONDS_PER_OLED_UPDATE milliseconds.
//
//                  The program also uses an instrumentation
//                  scheme (implemented in "myDebug.h") that allows one to see
//                  program events on an oscilloscope or logic analyzer.
//                  See the "myDebug.h" file for details on how to use this
//                  important feature.
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
        OledSetCursor(2,2);
        OledPutString(buf); 
        OledUpdate();

    

    }
}
