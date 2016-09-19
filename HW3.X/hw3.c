#include <stdio.h>                      // for sprintf()
#include <stdbool.h>                    // for data type bool
#include <plib.h>                       // Peripheral Library
#include "PmodOLED.h"
#include "OledChar.h"
#include "OledGrph.h"
#include "delay.h"
bool getWithoutDebounce();
bool getWithDebounce();

bool getWithoutDebounce() {
    enum Button1Position {UP, DOWN}; // Possible states of BTN1
    static enum Button1Position button1CurrentPosition = UP; // BTN1 current state
    static enum Button1Position button1PreviousPosition = UP; // BTN1 previous state
    static unsigned int button1History = 0x0; // Last 32 samples of BTN1
    button1PreviousPosition = button1CurrentPosition;
    button1History = button1History << 1; // Sample BTN1

    if(PORTG & 0x40) {
        button1History = button1History | 0x01;
    }

    if ((button1History == 0xFFFFFFFF) && (button1CurrentPosition == UP)) 
       button1CurrentPosition = DOWN;
    else if ((button1History == 0x0000) && (button1CurrentPosition == DOWN)) 
       button1CurrentPosition = UP;
    
    if((button1CurrentPosition == DOWN) && (button1PreviousPosition == UP)) 
        return TRUE; // debounced 0-to-1 transition has been detected


    return FALSE; // 0-to-1 transition not detected

}


bool getWithDeounce() {
    enum Button1Position {UP, DOWN}; // Possible states of BTN1
    static enum Button1Position button1CurrentPosition = UP; // BTN1 current state
    static enum Button1Position button1PreviousPosition = UP; // BTN1 previous state
    static unsigned char button1History = 0x0; // Last 8 samples of BTN1
    button1PreviousPosition = button1CurrentPosition;

    if(rollOver()) { //rollover return True every 1ms
        button1History = button1History << 1; // Sample BTN1
    if(PORTG & 0x40) 
        button1History = button1History | 0x01;
    if ((button1History == 0xFF) && (button1CurrentPosition == UP)) 
        button1CurrentPosition = DOWN;        
    else if ((button1History == 0x00) && (button1CurrentPosition == DOWN)) 
        button1CurrentPosition = UP;
     
    if((button1CurrentPosition == DOWN) && (button1PreviousPosition == UP))
        return TRUE; // debounced 0-to-1 transition has been detected
       
    
    }
    
    return FALSE; // 0-to-1 transition not detected
}
