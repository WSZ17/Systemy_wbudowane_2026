/*
 * File:   Zad_5_175508.c
 * Author: local
 *
 * Created on May 6, 2026, 12:19 PM
 */

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
#include "lcd.h"
#include "adc.h"
#include "buttons.h"
#include "stdio.h"

void setup(void)
{
    LCD_Initialize();

    BUTTON_Enable(BUTTON_S3); // przycisk gracza 1
    BUTTON_Enable(BUTTON_S4); // przycisk gracza 2

    ADC_SetConfiguration(ADC_CONFIGURATION_DEFAULT);
    ADC_ReadPercentage(ADC_CHANNEL_POTENTIOMETER);
}

void delay_1s(void)
{
    for(volatile long i=0;i<300000;i++);
}

int getStartTime(void)
{
    static uint8_t lastVal = 0;
    uint8_t val = ADC_ReadPercentage(ADC_CHANNEL_POTENTIOMETER);

    val = (val + lastVal) / 2;
    lastVal = val;

    if(val <= 25) return 60;       // 1 min
    else if(val <= 70) return 180; // 3 min
    else return 300;               // 5 min
}

void displayTime(int t1, int t2)
{
    char buf[17];

    int m1 = t1 / 60;
    int s1 = t1 % 60;

    int m2 = t2 / 60;
    int s2 = t2 % 60;

    LCD_ClearScreen();

    sprintf(buf, "P1: %02d:%02d", m1, s1);
    LCD_PutString(buf, 16);

    LCD_PutChar('\n');

    sprintf(buf, "P2: %02d:%02d", m2, s2);
    LCD_PutString(buf, 16);
}

void showLose(int player)
{
    LCD_ClearScreen();

    if(player == 1)
        LCD_PutString("P1 KONIEC CZASU", 15);
    else
        LCD_PutString("P2 KONIEC CZASU", 15);

    while(1);
}

int main(void)
{
    setup();

    int time1 = getStartTime();
    int time2 = time1;

    int active = 0;

    displayTime(time1, time2);

    while(active == 0)
    {
        time1 = getStartTime();
        time2 = time1;

        displayTime(time1, time2);

        if(BUTTON_IsPressed(BUTTON_S3) || BUTTON_IsPressed(BUTTON_S4))
        {
            while(BUTTON_IsPressed(BUTTON_S3) || BUTTON_IsPressed(BUTTON_S4));
            active = 0; 
            break;
        }
    }

    while(1)
    {
        if(BUTTON_IsPressed(BUTTON_S3))
        {
            active = 2;
            while(BUTTON_IsPressed(BUTTON_S3));
        }

        if(BUTTON_IsPressed(BUTTON_S4))
        {
            active = 1; 
            while(BUTTON_IsPressed(BUTTON_S4));
        }

        if(active == 1 && time1 > 0)
            time1--;
        else if(active == 2 && time2 > 0)
            time2--;

        displayTime(time1, time2);

        if(time1 <= 0) showLose(1);
        if(time2 <= 0) showLose(2);

        delay_1s();
    }

    return 0;
}