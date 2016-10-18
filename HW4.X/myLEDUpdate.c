#include <plib.h>                       // Peripheral Library
#include <stdbool.h>
#include "myDebug.h"
#include "myLEDUpdate.h"

// Mask for LED1 GPIO
#define LED1_MASK 0x1000

// Define the length (number of iterations) for the PWM period
// This must be fast enough that your eye doesn't see the flashes
#define PWM_PERIOD 12

// Define how many times to repeat at a particular brightness 
// This is so that the cycle doesn't happen too rapidly
#define DUTY_CYCLE_REPEAT 10

void initLED1() {
    TRISGCLR = LED1_MASK; // For LED1: configure PortG pin for output
    ODCGCLR = LED1_MASK; // For LED1: configure as normal output (not open drain)
}

// Helper function for the PWM implementation
// Meant to accomplish one complete cycle of the PWM and then return "true"
bool updatePWMCycle(int dutyCycle) {
    static int internalCounter = 0;
    bool returnValue = false;
    if (internalCounter > dutyCycle) {
        LATGCLR = LED1_MASK; // turn LEDs off
        DBG_ON(MASK_DBG1);
    }
    internalCounter++;
    if (internalCounter >= PWM_PERIOD) {
        returnValue = true; // set returnValue true as done with complete period
        internalCounter = 0; // reset internal counter
        if (dutyCycle > 0) {
            LATGSET = LED1_MASK; // turn LEDs on at beginning of cycle
            DBG_OFF(MASK_DBG1);
        }
    }
    return (returnValue);
}

void updateLED1() {
    static int PWM_duty_cycle = 0;
    static int PWM_duty_cycle_repeat_counter = 0;
    static int PWM_duty_cycle_direction = 1;
    bool cycle_done;
    cycle_done = updatePWMCycle(PWM_duty_cycle);
    if (cycle_done) {
        PWM_duty_cycle_repeat_counter++;
        if (PWM_duty_cycle_repeat_counter >= DUTY_CYCLE_REPEAT) {
            DBG_ON(MASK_DBG3);
            PWM_duty_cycle_repeat_counter = 0;
            if (PWM_duty_cycle > PWM_PERIOD) {
                PWM_duty_cycle_direction = -1;
            } else if (PWM_duty_cycle < 0) {
                PWM_duty_cycle_direction = 1;
                PWM_duty_cycle_repeat_counter = -50;
            }
            PWM_duty_cycle += PWM_duty_cycle_direction;
            DBG_OFF(MASK_DBG3);
        }
    }
}
