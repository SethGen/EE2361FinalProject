//Program explanation


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
volatile int max = 0;
volatile int ADCBufVal = 0;

#define Buffsize 128
#define numSamples 16


void adc_init()
{
    TRISBbits.TRISB12 = 1;       //input by default, but just for safety
    AD1PCFGbits.PCFG12 = 0;      //setup I/O
    AD1CON1bits.SSRC = 0b010; //Sample on timer 3 events
    AD1CON1bits.FORM = 0b00; //data output form -- recommended unsigned int

    AD1CON1bits.ASAM = 1;//1 = auto sampling, i assume thats what we want)
    AD1CON2bits.VCFG = 0b000;   //use AVDD (3.3v) and AVSS (0v) as max/min
    AD1CON2bits.SMPI = 0b0;//0 = starts conversion as soon as we get a value
    AD1CON2bits.BUFM = 0; //This wasnt in prelab but it sets it up as 16 bit buffer
    AD1CON3bits.ADCS = 1;//You want TAD >= 75ns (Tcy = 62.5 ns)  This bitfield has to do with prescaler. 2 sets 2Tad = 125ns
    AD1CON3bits.SAMC = 1;//You want at least 1 auto sample time bit
    AD1CON1bits.ADON = 1;       //turn on the ADC
   
    _AD1IF = 0;                 //clear IF
    _AD1IE = 1;                 //enable interrupts
   
    TMR3 = 0;                   //set up timer 3
    T3CON = 0;                  
    T3CONbits.TCKPS = 0b10;
    PR3 = 15624;
    T3CONbits.TON = 1;
   
}


void __attribute__((interrupt,auto_psv)) _ADC1Interrupt(void){

    IFS0bits.AD1IF = 0;
    ADCBufVal = ADC1BUF0; 
    
     if (ADCBufVal >= max)
     {
         max = ADCBufVal;

     }

}

void setup_ADC(void) {
    
    AD1PCFGbits.PCFG12 = 0; // AN12 is the only analog pin we are using

    // Setup on A/D
    AD1CON1 = 0;
    AD1CON2 = 0;
    AD1CON3 = 0;
    AD1CHS = 0;

    AD1CON1bits.ASAM = 1; // auto-sampling
    AD1CON1bits.SSRC = 0b111; // Internal counter

    
    AD1CON3bits.SAMC = 31; // auto-sample time
    AD1CON3bits.ADCS = 63; // clock = 64 * Tcy 
    AD1CHSbits.CH0SA = 0b01100;
    
   

    // Configure A/D interrupt
    _AD1IF = 0; // clear flag
    _AD1IE = 1; // enable interrupt

    AD1CON1bits.ADON = 1;
}





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
    TRISBbits.TRISB15 = 0;
    TRISBbits.TRISB14 = 0;
    T2CON = 0x8030; //Stop Timer, Tcy clk source, PRE 1:256
    TMR2 = 0;     // Initialize to zero
    PR2 = 61; //  
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


int main(void) {
                                           
    setup();
    ST7735_initR();
    ST7735_fillScreen(colBlack);
    initArray(ADCdata);
    menu();
    setup_ADC();
    
   
    
    PORTBbits.RB14 = 0;         //Signal Sent to other PIC24
    delay_ms(100);

    while(1){
        
        for(i = 0; i<50; i++){
            
            delay_ms(1000);
            
            ADCdata[i] = max;

            _RB15 = 0;
            delay_ms(1);
            _RB15 = 1;
            max=0;
       
        }
        
            makeDBV(ADCdata);
            plotData(ADCdata);
            break;
        
    }
    
    
    
    while(1){}
    
    
    
    
    
    return 0;
}
