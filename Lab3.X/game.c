////////////////////////////////////////////////////////////////////////////////
// ECE 2534:        Lab3 Pong game 
//
// File name:       game.c 
//
// Description:     This file tests if the game can be run without ADC.
//
// How to use:      include in main file. Call this function if needed. 
//
//
// Written by:      Beichen Liu

#include <stdio.h>                      // for sprintf())
#include <plib.h>
#include "PmodOLED.h"
#include "OledChar.h"
#include "OledGrph.h"
#include "delay.h"

int main(){


    return 0;
}

void game(){

    enum dir {TL, TR, BL, BR}; //travel direction
    enum dir direction; 
    while(1){
        switch(dir):
        case TL: //when travel top left
            while (//xpos !=1 || ypos !=1); //not touching top or left

            if (xpos == 1) {
                dir = TR;
            } 
            if (ypos ==1) {
                dir = BL;
            }
        case TR: //when travel top right
            while (//xpos != 127 || ypos !=1) //not touching top or right

            if (xpos == 127) {
                dir = TL;
            } 
            if (ypos ==1) {
                dir = BR;
            }
        case BL: //when travel bottom left
            while (//xpos !=1 || ypos !=31); //not touching left or bottom

            if (xpos == 1) {
                dir = BR;
            } 
            if (ypos == 31) {
                dir = TL
            }
        case BR: //when travel bottom right
            while (//xpos != 127 || ypos != 31); // not touching bottom right

            if (xpos == 127) {
                dir = BL;
            } 
            if (ypos == 31) {
                dir = TR;
            }
    }
}


