/*
 * File:   Zad2_175508.c
 * Author: local
 *
 * Created on April 22, 2026, 12:47 PM
 */

// PIC24FJ128GA010 Configuration Bit Settings

// 'C' source line config statements

// CONFIG2
#pragma config POSCMOD = XT             // Primary Oscillator Select (XT Oscillator mode selected)
#pragma config OSCIOFNC = ON            // Primary Oscillator Output Function (OSC2/CLKO/RC15 functions as port I/O (RC15))
#pragma config FCKSM = CSDCMD           // Clock Switching and Monitor (Clock switching and Fail-Safe Clock Monitor are disabled)
#pragma config FNOSC = PRI              // Oscillator Select (Primary Oscillator (XT, HS, EC))
#pragma config IESO = ON                // Internal External Switch Over Mode (IESO mode (Two-Speed Start-up) enabled)

// CONFIG1
#pragma config WDTPS = PS32768          // Watchdog Timer Postscaler (1:32,768)
#pragma config FWPSA = PR128            // WDT Prescaler (Prescaler ratio of 1:128)
#pragma config WINDIS = ON              // Watchdog Timer Window (Standard Watchdog Timer enabled,(Windowed-mode is disabled))
#pragma config FWDTEN = ON              // Watchdog Timer Enable (Watchdog Timer is enabled)
#pragma config ICS = PGx2               // Comm Channel Select (Emulator/debugger uses EMUC2/EMUD2)
#pragma config GWRP = OFF               // General Code Segment Write Protect (Writes to program memory are allowed)
#pragma config GCP = OFF                // General Code Segment Code Protect (Code protection is disabled)
#pragma config JTAGEN = OFF             // JTAG Port Enable (JTAG port is disabled)

// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.

#include "xc.h"
#include "libpic30.h"
#include "stdio.h"
#include "adc.h"

void init(void){
    ADC_SetConfiguration(ADC_CONFIGURATION_DEFAULT);
    ADC_ChannelEnable(ADC_CHANNEL_POTENTIOMETER); //config potencjometru
    TRISA = 0x0000; //diody na wyj?cie
    TRISD |= (1<<6);   // RD6 jako wej?cie (NEXT)
    TRISD |= (1<<13);  // RD13 jako wej?cie (PREV)
}

int main(void) {
    init();
    
    unsigned long adc_value;
    unsigned char tryb = 0;
    unsigned char poprzedni_RD6 = 1;
    unsigned char poprzedni_RD13 = 1;
    unsigned long opoznienie;

    while(1){
        adc_value = ADC_Read10bit(ADC_CHANNEL_POTENTIOMETER);
        if(adc_value == 0xFFFF){
            continue;
        }

        unsigned char poziom = adc_value / 205;

        switch(poziom){
            case 0: opoznienie = 200000; break; // najwolniej
            case 1: opoznienie = 120000; break;
            case 2: opoznienie = 80000;  break;
            case 3: opoznienie = 40000;  break;
            case 4: opoznienie = 20000;  break; // najszybciej
        }

        if (tryb == 0){
            bcd_w_gore();
        }
        else{
            wenszyk();
        }

        unsigned long i;
        for(i = 0; i < opoznienie; i += 4000){

            unsigned char stan_RD6 = PORTDbits.RD6;
            unsigned char stan_RD13 = PORTDbits.RD13;

            // RD6 ? nast?pny program
            if(poprzedni_RD6 == 1 && stan_RD6 == 0){
                tryb++;
                if(tryb > 1) tryb = 0;
                __delay32(20000);
            }

            // RD13 ? poprzedni program
            if(poprzedni_RD13 == 1 && stan_RD13 == 0){
                if(tryb == 0) tryb = 1;
                else tryb--;
                __delay32(20000);
            }

            poprzedni_RD6 = stan_RD6;
            poprzedni_RD13 = stan_RD13;

            __delay32(40000);
        }
    }
    return 0;
}

void bcd_w_gore() {
    static unsigned char jednosci = 0;  
    static unsigned char dziesiatki = 0; 
    
    LATA = (dziesiatki << 4) | jednosci;

    jednosci++;
    if(jednosci > 9) {
        jednosci = 0;
        dziesiatki++;
        if(dziesiatki > 9) dziesiatki = 0; 
    }
}

void wenszyk() {
    static unsigned char numer = 0x07;
    static int kierunek = 1; 

    LATA = numer;

    if(kierunek == 1) {
        numer <<= 1;
        if(numer == 0xE0) kierunek = -1;
    } else {
        numer >>= 1;
        if(numer == 0x07) kierunek = 1;
    }
}
