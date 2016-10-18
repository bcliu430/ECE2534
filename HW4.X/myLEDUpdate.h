/* 
 * File:   myLEDUpdate.h
 * Author: PEP
 *
 * Created on September 23, 2016, 8:19 AM
 */

#ifndef MYLEDUPDATE_H
#define	MYLEDUPDATE_H

#ifdef	__cplusplus
extern "C" {
#endif

/**
 * Update LED1-based on a PWM scheme.<br>
 * 
 * The parameters for the PWM scheme are hard-wired in the implementation.<br>
 * 
 * Does a sort of "triangle wave" output to the LED where your eye integrates
 * the LED output and sees the LED a brightening and dimming.
 * 
 * *
 */
void updateLED1();

/**
 * Initialize LED1
 */
void initLED1();
 
#ifdef	__cplusplus
}
#endif

#endif	/* MYLEDUPDATE_H */

