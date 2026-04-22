/*
 * File:   zad1_z_tego_labu.c
 * Author: local
 *
 * Created on April 22, 2026, 11:45 AM
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
    TRISD |= (1<<6); //RD6 jest jako wej?cie
}

int main(void) {
    init();
    
    unsigned long adc_value; //zmienna warto?ci potencjometru
    unsigned char tryb = 0; //0 - jedna dioda, 1 - wszystkie diody
    unsigned char poprzedni_stan_przycisku = 1;
    unsigned long opoznienie;
    
    while(1){
        adc_value = ADC_Read10bit(ADC_CHANNEL_POTENTIOMETER);
        if(adc_value == 0xFFFF){
            continue;
        } //je?li adc ma warto?? 0xffff (maks) to pomi? reszt? p?tli i przejd? do kolejnej iteracji
        
        opoznienie = 40000 + (adc_value * 4000); //40000 - podstawowa warto??, ok. 10 milisekund
        
        unsigned char stan_przycisku = PORTDbits.RD6;
        if(poprzedni_stan_przycisku == 1 && stan_przycisku == 0){
            tryb ^= 1; //zmiana trybu
            __delay32(40000);
        }
        
        poprzedni_stan_przycisku = stan_przycisku;
        
        if (tryb==0){
            LATA = 0x0001;
            __delay32(opoznienie);
            LATA = 0x0000;
            __delay32(opoznienie);
        }
        else{
            LATA = 0xFFFF;
            __delay32(opoznienie);
            LATA = 0x0000;
            __delay32(opoznienie);
        }
    };
    
    return 0;
}
