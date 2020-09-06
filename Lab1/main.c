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
#include <stdlib.h>

int main(void)
{
    /* Stop Watchdog  */
    //MAP_WDT_A_holdTimer();
    char description_arr [17][30] = {"TLV checksum"
                                     ,"Device Info Tag"
                                     ,"Device Info Length"
                                     ,"Device ID"
                                     ,"Hardware Revision"
                                     ,"Boot-Code Revision"
                                     ,"ROM Driver Library Revision"
                                     ,"Die Record Tag"
                                     ,"Die Record Length"
                                     ,"Die X Position"
                                     ,"Die Y Position"
                                     ,"Wafer ID"
                                     ,"Lot ID"
                                     ,"Reserved"
                                     ,"Reserved"
                                     ,"Reserved"
                                     ,"Test Results"
                            };    // string array to store 17 descriptions

    uint32_t* addr_ptr = (uint32_t*)0x00201000;
    int i;
    printf("     Description                   Address                 Value\n");
    for(i = 0; i < 17; i = i + 1 ){
        printf("%30s                   %08Xh               %08Xh\n",description_arr[i], addr_ptr + (uint32_t)i, *(addr_ptr + (uint32_t)i));
    }

}


/* p2_1.c Toggling green LED in C using header file register definitions.
 * This program toggles green LED for 0.5 second ON and 0.5 second OFF.
 * The green LED is connected to P2.1.
 * The LEDs are high active (a '1' turns ON the LED).
 *
 * Tested with Keil 5.20 and MSP432 Device Family Pack V2.2.0
 * on XMS432P401R Rev C.
 */

//#include "msp.h"

//void delayMs(int n);
//
//int main(void) {
//    P2->SEL1 &= ~2;         /* configure P2.1 as simple I/O */
//    P2->SEL0 &= ~2;
//    P2->DIR |= 2;           /* P2.1 set as output pin */
//
//    while (1) {
//        P2->OUT |= 2;       /* turn on P2.1 green LED */
//        delayMs(500);
//        P2->OUT &= ~2;      /* turn off P2.1 green LED */
//        delayMs(500);
//    }
//}
//
///* delay milliseconds when system clock is at 3 MHz for Rev C MCU */
//void delayMs(int n) {
//    int i, j;
//
//    for (j = 0; j < n; j++)
//        for (i = 750; i > 0; i--);      /* Delay 1 ms*/
//}
