
#include <p24FJ64GA002.h>

#include "xc.h"

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <math.h>

#include "DAC_test.h"




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
volatile int k = 100; //Determines the frequency of the output
volatile unsigned int dir = 1; // Determines direction of frequency sweep (increasing or decreasing) (just for demo)

int sinLookup[1024] = {2048,2060,2073,2085,2098,2110,2123,2135,2148,2160,
2173,2185,2198,2210,2223,2236,2248,2260,2273,2285,
2298,2310,2323,2335,2348,2360,2372,2385,2397,2410,
2422,2434,2447,2459,2471,2483,2496,2508,2520,2532,
2545,2557,2569,2581,2593,2605,2617,2629,2641,2653,
2665,2677,2689,2701,2713,2725,2737,2748,2760,2772,
2784,2795,2807,2819,2830,2842,2853,2865,2876,2888,
2899,2911,2922,2933,2945,2956,2967,2978,2990,3001,
3012,3023,3034,3045,3056,3067,3078,3088,3099,3110,
3121,3131,3142,3153,3163,3174,3184,3194,3205,3215,
3225,3236,3246,3256,3266,3276,3286,3296,3306,3316,
3326,3336,3345,3355,3365,3374,3384,3393,3403,3412,
3421,3431,3440,3449,3458,3467,3476,3485,3494,3503,
3512,3521,3529,3538,3546,3555,3563,3572,3580,3589,
3597,3605,3613,3621,3629,3637,3645,3653,3661,3668,
3676,3683,3691,3698,3706,3713,3720,3728,3735,3742,
3749,3756,3763,3770,3776,3783,3790,3796,3803,3809,
3815,3822,3828,3834,3840,3846,3852,3858,3864,3870,
3875,3881,3887,3892,3897,3903,3908,3913,3918,3923,
3928,3933,3938,3943,3948,3952,3957,3961,3966,3970,
3974,3979,3983,3987,3991,3995,3999,4002,4006,4010,
4013,4017,4020,4023,4027,4030,4033,4036,4039,4042,
4045,4047,4050,4053,4055,4058,4060,4062,4064,4067,
4069,4071,4072,4074,4076,4078,4079,4081,4082,4084,
4085,4086,4087,4088,4089,4090,4091,4092,4092,4093,
4093,4094,4094,4095,4095,4095,4095,4095,4095,4095,
4094,4094,4094,4093,4093,4092,4091,4091,4090,4089,
4088,4087,4085,4084,4083,4081,4080,4078,4077,4075,
4073,4071,4070,4068,4065,4063,4061,4059,4056,4054,
4051,4049,4046,4043,4040,4038,4035,4031,4028,4025,
4022,4018,4015,4011,4008,4004,4001,3997,3993,3989,
3985,3981,3977,3972,3968,3964,3959,3955,3950,3945,
3941,3936,3931,3926,3921,3916,3911,3905,3900,3895,
3889,3884,3878,3873,3867,3861,3855,3849,3843,3837,
3831,3825,3819,3812,3806,3799,3793,3786,3780,3773,
3766,3759,3752,3745,3738,3731,3724,3717,3709,3702,
3695,3687,3680,3672,3664,3657,3649,3641,3633,3625,
3617,3609,3601,3593,3584,3576,3568,3559,3551,3542,
3534,3525,3516,3507,3499,3490,3481,3472,3463,3454,
3445,3435,3426,3417,3407,3398,3389,3379,3369,3360,
3350,3340,3331,3321,3311,3301,3291,3281,3271,3261,
3251,3241,3231,3220,3210,3200,3189,3179,3168,3158,
3147,3137,3126,3115,3105,3094,3083,3072,3061,3050,
3039,3028,3017,3006,2995,2984,2973,2962,2950,2939,
2928,2916,2905,2894,2882,2871,2859,2848,2836,2825,
2813,2801,2790,2778,2766,2754,2743,2731,2719,2707,
2695,2683,2671,2659,2647,2635,2623,2611,2599,2587,
2575,2563,2551,2538,2526,2514,2502,2490,2477,2465,
2453,2440,2428,2416,2403,2391,2379,2366,2354,2341,
2329,2317,2304,2292,2279,2267,2254,2242,2229,2217,
2204,2192,2179,2167,2154,2142,2129,2117,2104,2091,
2079,2066,2054,2041,2029,2016,2004,1991,1978,1966,
1953,1941,1928,1916,1903,1891,1878,1866,1853,1841,
1828,1816,1803,1791,1778,1766,1754,1741,1729,1716,
1704,1692,1679,1667,1655,1642,1630,1618,1605,1593,
1581,1569,1557,1544,1532,1520,1508,1496,1484,1472,
1460,1448,1436,1424,1412,1400,1388,1376,1364,1352,
1341,1329,1317,1305,1294,1282,1270,1259,1247,1236,
1224,1213,1201,1190,1179,1167,1156,1145,1133,1122,
1111,1100,1089,1078,1067,1056,1045,1034,1023,1012,
1001,990,980,969,958,948,937,927,916,906,
895,885,875,864,854,844,834,824,814,804,
794,784,774,764,755,745,735,726,716,706,
697,688,678,669,660,650,641,632,623,614,
605,596,588,579,570,561,553,544,536,527,
519,511,502,494,486,478,470,462,454,446,
438,431,423,415,408,400,393,386,378,371,
364,357,350,343,336,329,322,315,309,302,
296,289,283,276,270,264,258,252,246,240,
234,228,222,217,211,206,200,195,190,184,
179,174,169,164,159,154,150,145,140,136,
131,127,123,118,114,110,106,102,98,94,
91,87,84,80,77,73,70,67,64,60,
57,55,52,49,46,44,41,39,36,34,
32,30,27,25,24,22,20,18,17,15,
14,12,11,10,8,7,6,5,4,4,
3,2,2,1,1,1,0,0,0,0,
0,0,0,1,1,2,2,3,3,4,
5,6,7,8,9,10,11,13,14,16,
17,19,21,23,24,26,28,31,33,35,
37,40,42,45,48,50,53,56,59,62,
65,68,72,75,78,82,85,89,93,96,
100,104,108,112,116,121,125,129,134,138,
143,147,152,157,162,167,172,177,182,187,
192,198,203,208,214,220,225,231,237,243,
249,255,261,267,273,280,286,292,299,305,
312,319,325,332,339,346,353,360,367,375,
382,389,397,404,412,419,427,434,442,450,
458,466,474,482,490,498,506,515,523,532,
540,549,557,566,574,583,592,601,610,619,
628,637,646,655,664,674,683,692,702,711,
721,730,740,750,759,769,779,789,799,809,
819,829,839,849,859,870,880,890,901,911,
921,932,942,953,964,974,985,996,1007,1017,
1028,1039,1050,1061,1072,1083,1094,1105,1117,1128,
1139,1150,1162,1173,1184,1196,1207,1219,1230,1242,
1253,1265,1276,1288,1300,1311,1323,1335,1347,1358,
1370,1382,1394,1406,1418,1430,1442,1454,1466,1478,
1490,1502,1514,1526,1538,1550,1563,1575,1587,1599,
1612,1624,1636,1648,1661,1673,1685,1698,1710,1723,
1735,1747,1760,1772,1785,1797,1810,1822,1835,1847,
1859,1872,1885,1897,1910,1922,1935,1947,1960,1972,
1985,1997,2010,2022,2035}; //Lookup table to generate a sine wave



// This code is for interfacing with the MCP4821 DAC unit, which has an SPI interface.
// /LDAC => GROUND
// /SHDN => VDD
// SCK and SDI => RP2 and RP3 respectively
// /CS => RB6
// The code sweeps from low to high frequencies and then back.


#define SIGMASK 0x0FFF //0000 1111 1111 1111
#define CONBITS 0x3000 //0011 0000 0000 0000

// Create a variable to store the output signal
volatile uint16_t signal = 0;

// Create a flag to store if SPI is transmitting
volatile uint8_t SPI_TRMT = 0;
volatile uint8_t parity = 0;

int main()
{
    
    setup();
    setupSPI();
    setupTMR(); //Timer will control the rate at which frequency sweeps
    
    
    uint32_t i = 0;
   
    
    while(1) {
     
        signal = sinLookup[i]; //Maximum output is 4095, min is 0
        i = i + k;
        i&=1023;
     
        LATBbits.LATB6 = 0;
      
        asm("nop");
        asm("nop");
        
        signal &= SIGMASK;
        signal |= CONBITS;

        SPI_TRMT = 1;
        
        parity = !parity;
        
        SPI1BUF = signal;
        

        // Wait until SPI is done transmitting
        while(SPI_TRMT) continue;

        
        // Pull chip select high
        LATBbits.LATB6 = 1;
        
        // Wait a bit
        //delay_us(1);
        
        
        //delay_ms(2000);
    }
}


void setup(void) {
    // Set up the clock prescalar
    CLKDIVbits.RCDIV = 0;
    // Set all pins to digital
    AD1PCFG = 0x9fff;
    //  Set all pins as inputs
    TRISB = 0xffff;
    
    // Set pin RB6 as an output
    TRISBbits.TRISB6 = 0;
    // Set chip select high
    LATBbits.LATB6 = 1;
    
//    TRISBbits.TRISB8 = 0;
//    LATBbits.LATB8 = 0;
}


void setupTMR(void){
    // Set up the timer with no prescalar
    T3CON = 0x8030;
    // Set the period register to 30000
    PR3 = 4000;
    // Set the timer count register to 0
    TMR3 = 0;
  
    // CLaer the interrupt flags and enable interrupts
    IFS0bits.T3IF = 0;
    IPC2bits.T3IP = 4;
    IEC0bits.T3IE = 1;
 
}



void __attribute__((__interrupt__,__auto_psv__)) _T3Interrupt(void)
{
    
    _T3IF = 0;
    
    //Change frequency 
    k = k + dir;
    
  //Change direction of sweep
    if(k%100 == 0){
        dir = -dir;
    }
   

    
}



void __attribute__((__interrupt__,__auto_psv__)) _SPI1Interrupt(void)
{
    SPI_TRMT = 0;
    
    parity = !parity;
    
    
    SPI1STATbits.SPIROV = 0;
    
    IFS0bits.SPI1IF = 0;
}


void setupSPI(void) {
    
    IFS0bits.SPI1IF = 0;
    IPC2bits.SPI1IP = 1;
    IEC0bits.SPI1IE = 1;
    
    // Peripheral Pin Select
    __builtin_write_OSCCONL(OSCCON & 0xbf);
    RPOR1bits.RP2R = 8; // SPI Clock
    RPOR1bits.RP3R = 7; // SPI Data
    __builtin_write_OSCCONL(OSCCON | 0x40);
    

    SPI1CON1 = 0x0000; // Clear the SPI Control Register
    SPI1CON1bits.MSTEN = 1;  // Master Mode
    SPI1CON1bits.MODE16 = 1; // 16-bit
    SPI1CON1bits.CKE = 1;
    SPI1CON1bits.CKP = 0;
    SPI1CON1bits.SPRE = 0b111; // Secondary Prescalar = 8;
    SPI1CON1bits.PPRE = 0b11;  // Primary Prescalar = 64;
    
    
    SPI1CON2 = 0x0000;
    
//    // For enhanced mode
//    SPI1CON2bits.SPIBEN = 1;
//    SPI1STAT = 0x0000;
//    SPI1STATbits.SISEL = 0b101; // IF set when last bit is shifted out
//                                // That means the SPI xfer is complete.
    
    SPI1STATbits.SPIROV = 0;
    
    SPI1STATbits.SPIEN = 1;
}

void delay_ms(uint32_t ms) {
    while(ms-- > 0){
        asm("repeat #15998");
        asm("nop");
    }
    return;
}

void delay_us(uint32_t us) {
    while(us-- > 0){
        asm("repeat #12");
        asm("nop");
    }
    return;
}

