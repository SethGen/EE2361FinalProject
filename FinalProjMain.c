/*
 * File:   FinalProjMain.c
 * Author: Seth
 *
 * Created on December 2, 2022, 6:01 AM
 */


//Boilerplate Code
#include "xc.h" 
#include "colors.h"
#include "st7735.h"
// CW1: FLASH CONFIGURATION WORD 1 (see PIC24 Family Reference Manual 24.1)
#pragma config ICS = PGx1          // Comm Channel Select (Emulator EMUC1/EMUD1 pins are shared with PGC1/PGD1)
#pragma config FWDTEN = OFF        // Watchdog Timer Enable (Watchdog Timer is disabled)
#pragma config GWRP = OFF          // General Code Segment Write Protect (Writes to program memory are allowed)
#pragma config GCP = OFF           // General Code Segment Code Protect (Code protection is disabled)
#pragma config JTAGEN = OFF        // JTAG Port Enable (JTAG port is disabled)


// CW2: FLASH CONFIGURATION WORD 2 (see PIC24 Family Reference Manual 24.1)
#pragma config I2C1SEL = PRI       // I2C1 Pin Location Select (Use default SCL1/SDA1 pins)
#pragma config IOL1WAY = OFF       // IOLOCK Protection (IOLOCK may be changed via unlocking seq)
#pragma config OSCIOFNC = ON       // Primary Oscillator I/O Function (CLKO/RC15 functions as I/O pin)
#pragma config FCKSM = CSECME      // Clock Switching and Monitor (Clock switching is enabled, 
                                       // Fail-Safe Clock Monitor is enabled)
#pragma config FNOSC = FRCPLL      // Oscillator Select (Fast RC Oscillator with PLL module (FRCPLL))
//End of Boilerplate Code
volatile unsigned int ADC1BUFMAX[plotSamples];
volatile unsigned int ADCdata[plotSamples]; 
volatile unsigned int i = 0;



void setup(void){
    CLKDIVbits.RCDIV = 0;  //Set RCDIV=1:1 (default 2:1) 32MHz or FCY/2=16M
    //Digital Port Initialization
    AD1PCFG = 0x9fff;
    TRISA = 0xffff;
    TRISB = 0x9fff;
    TRISAbits.TRISA0 = 0;
    TRISAbits.TRISA1 = 0;
    TRISBbits.TRISB5 = 0;
    TRISBbits.TRISB6 = 0;
    TRISBbits.TRISB7 = 0;
    T3CON = 0x8030; //Stop Timer, Tcy clk source, PRE 1:8
    TMR3 = 0;     // Initialize to zero
    PR3 = 61; //  (20 ms with 8:1 PRE )
}



void initArray(unsigned int arr[]){
    int l;
    for(l=0;l<plotSamples;l++){
        arr[l] = 0;
    }
}


void updateTimer(unsigned int i){
    PR3 = 61*(50-i);
}

void __attribute__((interrupt,auto_psv)) _T2Interrupt(void)
{
    IFS0bits.T2IF = 0;
    ADCdata[i] = ADC1BUFMAX[i];
    i++;
}

int main(void) {
    setup();
    ST7735_initR();
    ST7735_fillScreen(colBlack);
    //initArray(ADC1BUFMAX);
    //initArray(ADCdata);
    unsigned int data[150];                    //data of values between 0 and 120
    int q = 0;
    while(q<plotSamples){
        data[q] = 1023-20*q;
        if(data[q]<=0)
            data[q]=0;
        q++;
    }
    makeDBV(data);
    plotData(data);
    menu();
    while(1){
        
    }
    return 0;
}
