////////////////////////////////////////////////////////////////////////////////////
// ECE 2534:        HW6
// File name:       main.c
// Description:     This main file is uesd to read the data from alc, find the 
//                  min and max, and update the screen every 500 ms
// Resources:       
// Written by:      Beichen Liu 刘北辰 
// Version:         ver.11-14-2016 create 
//                  ver.11-17-2016 add spi specs, add ISR for updating values
//
// TODO: 
// state machine for changing the display using button 2
// 
#include <stdio.h>                      // for sprintf()
#include <stdbool.h>
#include <plib.h>                       // Peripheral Library
#include "PmodOLED.h"
#include "OledChar.h"
#include "OledGrph.h"
#include "delay.h"

// Digilent board configuration
#pragma config ICESEL       = ICS_PGx1  // ICE/ICD Comm Channel Select
#pragma config DEBUG        = OFF       // Debugger Disabled for Starter Kit
#pragma config FNOSC        = PRIPLL	// Oscillator selection
#pragma config POSCMOD      = XT	    // Primary oscillator mode
#pragma config FPLLIDIV     = DIV_2	    // PLL input divider
#pragma config FPLLMUL      = MUL_20	// PLL multiplier
#pragma config FPLLODIV     = DIV_1	    // PLL output divider
#pragma config FPBDIV       = DIV_8	    // Peripheral bus clock divider
#pragma config FSOSCEN      = OFF	    // Secondary oscillator enable

#define _PLIB_DISABLE_LEGACY
#define PBClock 10000000
#define BaudRate 9600

void initSPI(SpiChannel chn, unsigned int srcClkDiv);
void initAccelerometer(SpiChannel chn);
void setAccelReg(SpiChannel chn, unsigned int address, unsigned int data);
int  getAccelReg(SpiChannel chn, unsigned int address);
void getAccelData(SpiChannel chn, short accelData[]);
void init();

static short x = 0, \
             y = 0, \
             z = 0; // global varible for readings 
static short axis [6]; // 2 bit for each axis x, y, z;
static short x_min, x_max, \
             y_min, y_max, \
             z_min, z_max;
BYTE source;

int main() {
    
    init();
    unsigned int ID;
    ID = getAccelReg(SPI_CHANNEL3, 0x00);
    char buf[17];
    while(1){
	if( INTGetFlag(INT_T3) ) { //500 ms roll over
	    INTClearFlag(INT_T3);
	    if(x < x_min)
	    	x_min = x;
	    if (x > x_max)
		    x_max = x;
	    if (y < y_min)
		    y_min = y;
	    if (y > y_max)
		    y_max = y;
	    if (z < z_min)
		    z_min = z;
	    if (z > z_max)
		    z_max = z;

	    OledSetCursor(0,0);
	    sprintf(buf, "%02X MIN  CUR  MAX", ID ); 
	    OledPutString(buf);
	    OledSetCursor(0,1);
	    sprintf(buf, "X %+04d %+04d %+04d", x_min, x, x_max);
	    OledPutString(buf);
	    OledSetCursor(0,2);
	    sprintf(buf, "Y %+04d %+04d %+04d", y_min, y, y_max);
	    OledPutString(buf);
	    OledSetCursor(0,3);
	    sprintf(buf, "Z %+04d %+04d %+04d", z_min, z, z_max);
	    OledPutString(buf);
	    OledUpdate();

	    x_min = 0;
	    y_min = 0;
	    z_min = 0;
	    x_max = 0;
	    y_max = 0;
	    z_max = 0;
	}       

   
    }

    
    
    return (EXIT_SUCCESS);
}



void __ISR(_EXTERNAL_1_VECTOR, ipl7_AUTO) EXTERN_1_ISR(void) {
        source = getAccelReg(3,0x30);
        if(source&0x80){
            getAccelData(SPI_CHANNEL3, axis);
            source = getAccelReg(3,0X30);
            x = axis[0] | axis[1] << 8;
            y = axis[2] | axis[3] << 8;
            z = axis[4] | axis[5] << 8;
            INTClearFlag(INT_INT1);
        }
}

void init(){
       
    initSPI(SPI_CHANNEL3,1024);
    initAccelerometer(SPI_CHANNEL3);
    DelayInit();
    OledInit();
    OledClearBuffer();
    TRISESET = 1<<8;
    PORTESET = 1<<8;
    INTConfigureSystem(INT_SYSTEM_CONFIG_MULT_VECTOR);
    INTEnable(INT_INT1, INT_ENABLED);
    INTSetVectorPriority(INT_EXTERNAL_1_VECTOR, INT_PRIORITY_LEVEL_7);
    INTEnableInterrupts();
    INTClearFlag(INT_INT1);
    OpenTimer3(T3_ON | T3_IDLE_CON | T3_SOURCE_INT | T3_PS_1_256 | T3_GATE_OFF, 19530); //500 ms
    INTClearFlag(INT_T3);
}


void initSPI(SpiChannel chn, unsigned int srcClkDiv) {
    SpiChnOpen(chn, \
               SPI_OPEN_MSSEN | \
               SPI_OPEN_MSTEN | \
               SPI_OPEN_MODE8 | \
               SPI_OPEN_CKP_HIGH | \
               SPI_OPEN_ENHBUF, \
               srcClkDiv);
}

void initAccelerometer(SpiChannel chn) {
    setAccelReg(chn, 0x2E, 0x00); // INT_ENABLE
    setAccelReg(chn, 0x2C, 0x06); // addr_bw_rate
    setAccelReg(chn, 0x31, 0x2B); // addr_data_format
    setAccelReg(chn, 0x2D, 0x28); // addr_power_ctl
    setAccelReg(chn, 0x24, 0x03); // THRESH_ACT
    setAccelReg(chn, 0x25, 0x02); // THRESH_INACT 
    setAccelReg(chn, 0x26, 0x01); // TIME_INACT
    setAccelReg(chn, 0x2E, 0x98); // INT_ENABLE
    setAccelReg(chn, 0x2F ,0x98); // INT_MAP
    setAccelReg(chn, 0x27 ,0xFF); // ACT_INACT_CTL
}

void setAccelReg(SpiChannel chn, unsigned int address, unsigned int data) {
    SpiChnPutC (chn, address);
    SpiChnPutC (chn, data);
    SpiChnGetC (chn);
    SpiChnGetC (chn);
}

int getAccelReg(SpiChannel chn, unsigned int address) {
    SpiChnPutC(chn, 0x80 + address);
    SpiChnPutC(chn, 0);
    SpiChnGetC(chn);
    int value = SpiChnGetC(chn);
    
    return value;
}

void getAccelData(SpiChannel chn, short accelData[]) {
    SpiChnPutC(chn,(0x80 | 0x40 | 0x32));
    SpiChnPutC(chn,0);
    SpiChnPutC(chn,0);
    SpiChnPutC(chn,0);
    SpiChnPutC(chn,0);
    SpiChnPutC(chn,0);
    SpiChnPutC(chn,0);
    SpiChnGetC(chn);
    int i;
    for(i = 0; i < 6; i++) 
        accelData[i] = SpiChnGetC(chn);

}



