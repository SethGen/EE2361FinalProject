/*
 * File:   DAC_test.c
 * Author: rafaelavelar
 *
 * Created on November 18, 2022, 11:09 AM
 */


//#include <p24Fxxxx.h>

// #include <PPS.h>  // older versions of the XC16 compiler

#include "xc.h"

//#include <stdlib.h>

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


volatile unsigned char buffer[32];
volatile unsigned char front = 0;
volatile unsigned char back = 0;

// This code is for interfacing with the MCP4821 DAC unit, which has an SPI interface.
// /LDAC => GROUND
// /SHDN => VDD
// SCK and SDI => RP2 and RP3
// /CS => RB6


#define DACMASK 0x3FFF //0011 1111 1111 1111
#define DACBITS 0x3000 //0011 0000 0000 0000

volatile unsigned int sample = DACBITS;



//void __attribute__((__interrupt__,__auto_psv__)) _T2Interrupt(void)
//{
//    
//    //PORTBbits.RB7 = ~PORTBbits.RB7;
//    
//    sample = 0xAAAA;
//
//    
//    sample &= DACMASK;
//    sample |= DACBITS;
//
//    IFS0bits.T2IF = 0;
//    SPI1BUF = sample;   // Kickstart the SPI transaction. Do not worry about
//                        // LDAC' now. We will get the SPI interrupt once
//                        // the last bit is out. We will generate a pulse 
//                        // on the LDAC' line then.
//    
//    
//    
//
////    while(_SPI1IF == 0);
////    _SPI1IF = 0;
//    //
//    //PORTBbits.RB7 = ~PORTBbits.RB7;
//  
//    //sample += 41;       // the next "step" value.
//}



//void __attribute__((__interrupt__,__auto_psv__)) _SPI1Interrupt(void)
//{
//    
//    PORTBbits.RB7 = ~PORTBbits.RB7;
//    
//    _SPI1IF = 0;
//    
//    
//    PORTBbits.RB8 = 0;  // SPI transaction complete, give a low pulse on LDAC'
//        // to load the value from the input latch register to the DAC register.
//    // The DAC datasheet says that the pulse has to remain low for at least
//    // 100ns (p6 of the manual, the second table)
//    
//    asm("nop");
//    asm("nop");
//
//    PORTBbits.RB8 = 1;
//}


void setupSPI(void){
    
    __builtin_write_OSCCONL(OSCCON & 0xbf); // unlock PPS
    RPOR1bits.RP2R = 8;   //RB2->SPI1:SCK1OUT; See Table 10-3 on P109 of the datasheet
    RPOR1bits.RP3R = 7;   //RB3->SPI1:SDO1;
   // RPOR3bits.RP6R = 9;
    __builtin_write_OSCCONL(OSCCON | 0x40); // lock   PPS
    

    SPI1CON1 = 0;
    SPI1CON1bits.MSTEN = 1;  // master mode
    SPI1CON1bits.MODE16 = 1; // sixteen bits
    SPI1CON1bits.CKE = 1;
    SPI1CON1bits.CKP = 0;
    SPI1CON1bits.SPRE = 0b110; // secondary prescaler = 2
    SPI1CON1bits.PPRE = 0b11;  // primary prescaler = 1;
    //SPI1CON1bits.SSEN = 1;
    // SPI1CON1bits.PPRE = 0b01;  // primary prescaler = 16;
    
    
    
    SPI1CON2 = 0;
    SPI1STAT = 0;
    SPI1STATbits.SISEL = 0b101; // IF set when last bit is shifted out
                                // That means the SPI xfer is complete.
    
    _SPI1IF = 0;
    _SPI1IP = 1;
    _SPI1IE = 1;
    
    SPI1STATbits.SPIEN = 1;

}



void setupTMR(void){
    T3CON = 0;
    PR3 = 4000; 
    TMR3 = 0;
  
    IFS0bits.T3IF = 0;
    IPC2bits.T3IP = 4;
    IEC0bits.T3IE = 1;
    
    T3CONbits.TON = 1;
}


void setup(void)
{
    CLKDIVbits.RCDIV = 0;
    AD1PCFG = 0x9fff;  // For digital I/O.  If you want to use analog, you'll
                       // need to change this.
    TRISB = 0xffff;    // make all PORTBbits inputs initially


    
    
   
    TRISBbits.TRISB6 = 0; // this will be the DAC /CS
    PORTBbits.RB6 = 1;
}

void delay(unsigned int delay_in_ms){
    while(delay_in_ms-- > 0){
        asm("repeat #15998");
        asm("nop");
    }
 }




int main()
{
    
    setup();
    setupSPI();
    
    //setupTMR();
    
    
    int i;
    while(1){
        

        
        delay(100);
        
        sample = 4000; //Maximum voltage is sample = 4095, min is sample = 0
        
        PORTBbits.RB6 = 0;
      
        asm("nop");
        asm("nop");


        
        sample &= DACMASK;
        sample |= DACBITS;

        SPI1BUF = sample; 
        
        asm("repeat #35");
        asm("nop");
        //delay(1);
        
        
        PORTBbits.RB6 = 1;
        
        
  
  

    }
    

    return 0; // never reached (we hope)
}
