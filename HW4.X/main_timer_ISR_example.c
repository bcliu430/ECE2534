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

// Use preprocessor definitions for program constants
// The use of these definitions makes your code more readable!

#define NUMBER_OF_MILLISECONDS_PER_OLED_UPDATE 100

// *** Grandma's Step #1 ***
// Global variable to count number of times in timer2 ISR
// Changed in the ISR so declared as "volatile"
volatile unsigned int timer2_ms_value = 0;

// *** Grandma's Step #2 ***
// The interrupt handler for timer2
// IPL4 is a medium interrupt priority
// SOFT|AUTO|SRS refers to the shadow register use (see the RM)

void __ISR(_TIMER_2_VECTOR, IPL4AUTO) _Timer2Handler(void) {
    DBG_ON(MASK_DBG0); // Use debugging to see when the ISR is called on the LA/osc
    timer2_ms_value++; // Increment the millisecond counter.
    updateLED1(); // We use the Timer ISR to do something clever with LED1
    INTClearFlag(INT_T2); // Clear the Timer2 interrupt flag.
    DBG_OFF(MASK_DBG0);
}

// *** Grandma's Step #3 ***
// Initialize timer2 and set up the interrupts

void initTimer2() {
    // Configure Timer 2 to request a real-time interrupt once per millisecond.
    // The period of Timer 2 is (16 * 625)/(10 MHz) = 1ms.
    OpenTimer2(T2_ON | T2_IDLE_CON | T2_SOURCE_INT | T2_PS_1_16 | T2_GATE_OFF, 624);
    INTSetVectorPriority(INT_TIMER_2_VECTOR, INT_PRIORITY_LEVEL_4);
    INTClearFlag(INT_T2);
    INTEnable(INT_T2, INT_ENABLED);
}

// update OLED with latest data on button presses

void updateOLED(unsigned int current_ms_time) {
    char oledstring[17]; // string for OLED display
    unsigned int tenths_of_a_second;
    unsigned int seconds;
    DBG_ON(MASK_DBG2); // Use debugging to see how long the OLED update takes
    seconds = current_ms_time / 1000;
    tenths_of_a_second = (current_ms_time / 100) % 10;
    sprintf(oledstring, "Time (s): %4d.%1d", seconds, tenths_of_a_second);
    OledSetCursor(0, 1);
    OledPutString(oledstring);
    DBG_OFF(MASK_DBG2);
}

int main() {
    unsigned int last_oled_update = 0;
    unsigned int ms_since_last_oled_update;

    // Initialize LED1
    initLED1();

    // Initialize PmodOLED
    DelayInit();
    OledInit();

    // Grandma's Step #3
    // Initial Timer2
    initTimer2();

    // Initialize GPIO (Port E) for debugging
    DBG_INIT();

    // *** Grandma's Step #4 ***
    // Configure the system for vectored interrupts and enable controller
    INTConfigureSystem(INT_SYSTEM_CONFIG_MULT_VECTOR);
    INTEnableInterrupts();

    // Send a welcome message to the OLED display
    OledClearBuffer();
    OledSetCursor(0, 0); // upper-left corner of display
    OledPutString("TMR ISR Example");
    OledUpdate();

    // Main processing loop
    while (1) {
        // *** Grandma's Step #5, use the timer counter in our "main" ***
        ms_since_last_oled_update = timer2_ms_value - last_oled_update;
        if (ms_since_last_oled_update >= NUMBER_OF_MILLISECONDS_PER_OLED_UPDATE) {
            last_oled_update = timer2_ms_value;
            updateOLED(timer2_ms_value);
        }
    } // end while

    return EXIT_SUCCESS; // This return should never occur
} // end main

