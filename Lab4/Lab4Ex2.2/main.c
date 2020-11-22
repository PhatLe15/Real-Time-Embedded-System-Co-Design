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

uint32_t t0, t1, initial,current;
uint32_t duration, count_us;

void delay_ms(uint32_t count);

int main(void)
{
    printf("MCLK: %u\n", MAP_CS_getMCLK());

    /* Stop Watchdog  */
    MAP_WDT_A_holdTimer();

    //initialize the 32-bit timer
    MAP_Timer32_initModule(TIMER32_0_BASE, TIMER32_PRESCALER_1, TIMER32_32BIT, TIMER32_FREE_RUN_MODE);
    MAP_Timer32_startTimer(TIMER32_0_BASE, 0);

    MAP_GPIO_setAsOutputPin(GPIO_PORT_P1, GPIO_PIN0);     //LED 1 set as output
    GPIO_setOutputLowOnPin(GPIO_PORT_P1, GPIO_PIN0);     //set LED to low

    while(1){
        delay_ms(1000);
        MAP_GPIO_toggleOutputOnPin(GPIO_PORT_P1, GPIO_PIN0); //toggle the LED
    }

}

// 60s      30 blinks
// 120s     60 blinks
// 300s    150 blinks

void delay_ms(uint32_t count){
    duration = 0; //initialize the delay time
    count_us = count*(MAP_CS_getMCLK()/1000);// convert to # of cycles needed
    initial = MAP_Timer32_getValue(TIMER32_0_BASE); //initial time stamp

    while(count_us >= duration){//if the actual delay time meet the time requirement
        current = MAP_Timer32_getValue(TIMER32_0_BASE);// current time stamp
        duration = initial - current; // update the duration
    }
}



