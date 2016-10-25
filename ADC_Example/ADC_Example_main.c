
#define GetPBCClock() 10000000
#define BaudRate 9600
#define _PLIB_DISABLE_LEGACY
#include <plib.h>
#include <stdlib.h>                
#include "PmodOLED.h"                           
#include "OledChar.h"                            
#include "timer.h"
#include "int.h"            
#include "OledGrph.h"            
#include "delay.h"                       
#include "myBoardConfigFall2016.h" 
#include "adc10.h"

#define _PLIB_DISABLE_LEGACY  //legacy code not allowed in this course

static unsigned int VoltLR = 0;//read number for L/R joystick
static unsigned int VoltUD = 0;//read number for U/D joystick
static unsigned int offset = 0;
unsigned int timer_count=0;


//Interrupt service routine

void __ISR(_TIMER_3_VECTOR, ipl7) TimerISR(void) {
    offset = 8 * ((~ReadActiveBufferADC10() & 0x01)); 
    VoltUD = ReadADC10(offset); 
    VoltLR = ReadADC10(offset + 1); 
    INTClearFlag(INT_T3);
}

// *****************************************************************************
/* initialization function

  Summary:
 Initialize Timer2 and Timer3
 * 
 * Description:
 * Initialize Oled
  
*/
void init(){
    

   // Initialize Timer1 and OLED
    DelayInit();
    OledInit();

   // Set up Timer2 to roll over every 0.1 second
    OpenTimer2(T2_ON         |
              T2_IDLE_CON    |
              T2_SOURCE_INT  |
              T2_PS_1_64     |
              T2_GATE_OFF,
              15624);  // freq = 10MHz/64/15625 = 10 Hz
    INTClearFlag(INT_T2);  //clear timer 2 rollover flag
    
    OpenTimer3(T3_ON        |          // timer 3 on
             T3_IDLE_CON    |           // don't stop on idle
             T3_SOURCE_INT  |           // Use internal clock
             T3_PS_1_64     |           // Prescale = 32
             T3_GATE_OFF,               // No gate control on timer
             0x00009895);                      // counter register
    INTClearFlag(INT_T3);
    
}





int main()
{
	char buf[17];  
    init();

    INTConfigureSystem(INT_SYSTEM_CONFIG_MULT_VECTOR);
    INTEnable(INT_T3, INT_ENABLED);
    INTSetVectorPriority(INT_T3, INT_PRIORITY_LEVEL_7);
    INTClearFlag(INT_T3);
    INTEnableInterrupts();
    srand(ReadTimer3());

    OpenADC10(
            // turn on converter, 32 bit int format, start conversions with timer, sample the signal
            ADC_FORMAT_INTG|ADC_CLK_INT0|ADC_AUTO_SAMPLING_ON,
            // standard Vref & Vdd, no scanning,  1 sample per interupt, use regular input
            ADC_VREF_AVDD_AVSS|ADC_OFFSET_CAL_DISABLE|ADC_SCAN_OFF|ADC_SAMPLES_PER_INT_2|ADC_ALT_BUF_ON|ADC_ALT_INPUT_ON,
            // fast conversion clock, use internal clock
            ADC_CONV_CLK_PB|ADC_SAMPLE_TIME_10,
            // enable AN2 for analog             4
            ENABLE_AN3_ANA|ENABLE_AN2_ANA,
            // no scanning
            SKIP_SCAN_ALL
    );    
    SetChanADC10( ADC_CH0_NEG_SAMPLEA_NVREF|ADC_CH0_POS_SAMPLEA_AN3|ADC_CH0_NEG_SAMPLEB_NVREF|ADC_CH0_POS_SAMPLEB_AN2);
    EnableADC10();
    OledClearBuffer();
    OledSetCursor(0, 0);
    OledPutString("L/R = ");
    OledSetCursor(0, 1);
    OledPutString("U/D = ");
    OledUpdate();

    while(1){
        if(INTGetFlag(INT_T2)){
            timer_count++;
            INTClearFlag(INT_T2);
        }
        sprintf(buf, "%d", timer_count);
        OledSetCursor(12,0);
        OledPutString(buf); 
        OledUpdate();
        
        sprintf(buf, "%04d", VoltUD);
        OledSetCursor(6,1);
        OledPutString(buf);
      
        sprintf(buf, "%04d", VoltLR);
        OledSetCursor(6,0);
        OledPutString(buf);
        
        if (VoltLR < 300){
        OledSetCursor(0,2);
        OledPutString("LEFT");
        OledSetCursor(10,2);
        OledPutString("      ");
        }
        
        else if (VoltLR > 700){
        OledSetCursor(10,2);
        OledPutString("RIGHT");
        OledSetCursor(0,2);
        OledPutString("      ");
        }
   
        } 
       
       
    }
    

       

