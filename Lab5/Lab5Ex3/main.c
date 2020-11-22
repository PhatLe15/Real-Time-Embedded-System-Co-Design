/* --COPYRIGHT--,BSD
 * Copyright (c) 2017, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * --/COPYRIGHT--*/
/******************************************************************************
 * MSP432 Empty Project
 *
 * Description: An empty project that uses DriverLib
 *
 *                MSP432P401
 *             ------------------
 *         /|\|                  |
 *          | |                  |
 *          --|RST               |
 *            |                  |
 *            |                  |
 *            |                  |
 *            |                  |
 *            |                  |
 * Author: 
*******************************************************************************/
/* DriverLib Includes */
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>

/* Standard Includes */
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

void delay_ms(uint32_t count);

int main(void)
{
    bool state;
    uint32_t t0, notouch, percentage;
    /* Stop Watchdog  */
    MAP_WDT_A_holdTimer();

    //counter timerA for counting frequency
    Timer_A_ContinuousModeConfig timer_continuous_obj;
    timer_continuous_obj.clockSource = TIMER_A_CLOCKSOURCE_INVERTED_EXTERNAL_TXCLK;
    timer_continuous_obj.clockSourceDivider = TIMER_A_CLOCKSOURCE_DIVIDER_1;
    timer_continuous_obj.timerInterruptEnable_TAIE = TIMER_A_TAIE_INTERRUPT_DISABLE;
    timer_continuous_obj.timerClear = TIMER_A_DO_CLEAR;
    MAP_Timer_A_configureContinuousMode(TIMER_A2_BASE, &timer_continuous_obj);
    MAP_Timer_A_startCounter(TIMER_A2_BASE, TIMER_A_CONTINUOUS_MODE);


    //timer for delay function
    MAP_Timer32_initModule(TIMER32_0_BASE, TIMER32_PRESCALER_1, TIMER32_32BIT, TIMER32_FREE_RUN_MODE);
    MAP_Timer32_startTimer(TIMER32_0_BASE, 0);
    printf("MCLK: %u\n", MAP_CS_getMCLK());

    CAPTIO0CTL = 0;
    CAPTIO0CTL |= (1 << 8);    // Enable CAPTIO
    CAPTIO0CTL |= 0b0100 << 4; // Choose Port 4
    CAPTIO0CTL |= 0b0001 << 1; // Choose Pin 1

    //calibration
    printf("Calibrating...Please do not touch Pin\n");
    delay_ms(3000);
    MAP_Timer_A_clearTimer(TIMER_A2_BASE);
    delay_ms(2);
    notouch = (uint32_t)((unsigned long long)MAP_Timer_A_getCounterValue(TIMER_A2_BASE)*1000/2);
    printf("Calibration finished\n");

    //set up LED
    MAP_GPIO_setAsOutputPin(GPIO_PORT_P1, GPIO_PIN0);     //LED 1 set as output
    GPIO_setOutputLowOnPin(GPIO_PORT_P1, GPIO_PIN0);     //set LED to low

    while(1)
    {

        MAP_Timer_A_clearTimer(TIMER_A2_BASE);
        delay_ms(2);
        t0 = (uint32_t)((unsigned long long)MAP_Timer_A_getCounterValue(TIMER_A2_BASE)*1000/2);
        percentage = (uint32_t)((unsigned long long)abs(notouch-t0)*100/notouch); // control parameter

        printf("Control percentage difference: %u%%\n", percentage);
        if(percentage<=5){
            printf("No Touch! ");
            GPIO_setOutputLowOnPin(GPIO_PORT_P1, GPIO_PIN0);     //set LED to low
        }else{
            printf("Touch! ");
            GPIO_setOutputHighOnPin(GPIO_PORT_P1, GPIO_PIN0);     //set LED to high
        }

        printf("Frequency: %uHz \n\n", t0);
        delay_ms(500);

    }
}

void delay_ms(uint32_t count)
{
    uint32_t t0 = MAP_Timer32_getValue(TIMER32_0_BASE);
    uint32_t clock_rate = MAP_CS_getMCLK();
    uint32_t t_final = t0 - (uint32_t)((unsigned long long)clock_rate * count / 1000);
    while (1)
        if (MAP_Timer32_getValue(TIMER32_0_BASE) < t_final)
            break;
}
