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
/*******************************************************************************
 * MSP432 CRC32 - CRC32 Calculation
 *
 * Description: In this example, the CRC32 module is used to calculate a CRC32
 * checksum in CRC32 mode. This value is compared versus a software calculated
 * checksum. The idea here is to have the user use the debugger to step through
 * the code and observe the CRC calculation in the debugger. Note that this
 * code example was made to use the standard CRC32 polynomial value of
 * 0xCBF43926.
 *
 *              MSP432P401
 *             ------------------
 *         /|\|                  |
 *          | |                  |
 *          --|RST               |
 *            |                  |
 *            |                  |
 *            |                  |
 *            |                  |
 *
 ******************************************************************************/
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>
#include <stdio.h>
#include <stdlib.h>

#define CRC32_POLY              0xEDB88320
#define CRC32_INIT              0xFFFFFFFF

static  uint8_t myData[10240];
//{ 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39 };
static uint32_t calculateCRC32(uint8_t* data, uint32_t length);
void EvenOddAlterRand (uint8_t* data, uint32_t length);
uint32_t compute_simple_checksum(uint8_t* data, uint32_t length);

volatile uint32_t hwCalculatedCRC, swCalculatedCRC, simpleCalculatedCRC;

//![Simple CRC32 Example] 
int main(void)
{
    uint32_t ii, t0, t1;   //Time before and after
    uint32_t method1, method2, method3;

    EvenOddAlterRand ((uint8_t*) myData, 10240); //insert random data

    //initialize timer
    MAP_Timer32_initModule(TIMER32_0_BASE, TIMER32_PRESCALER_1, TIMER32_32BIT, TIMER32_FREE_RUN_MODE);
    MAP_Timer32_startTimer(TIMER32_0_BASE, 0);
    printf("%u\n", MAP_CS_getMCLK());

    /* Stop WDT */
    MAP_WDT_A_holdTimer();

 //method 1:

    t0 = MAP_Timer32_getValue(TIMER32_0_BASE);
    MAP_CRC32_setSeed(CRC32_INIT, CRC32_MODE);

    for (ii = 0; ii < 10240; ii++)
        MAP_CRC32_set8BitData(myData[ii], CRC32_MODE);

    /* Getting the result from the hardware module */
    hwCalculatedCRC = MAP_CRC32_getResultReversed(CRC32_MODE) ^ 0xFFFFFFFF;
    t1 = MAP_Timer32_getValue(TIMER32_0_BASE);
    method1 = (t0 - t1)/3;

////method 2:
//    /* Calculating the CRC32 checksum through software */
//    t0 = MAP_Timer32_getValue(TIMER32_0_BASE);
//
//    swCalculatedCRC = calculateCRC32((uint8_t*) myData, 10240);
//
//    t1 = MAP_Timer32_getValue(TIMER32_0_BASE);
//    method2 = (t0 - t1)/3;

//method 3:
    t0 = MAP_Timer32_getValue(TIMER32_0_BASE);

    simpleCalculatedCRC = compute_simple_checksum((uint8_t*) myData, 10240);

    t1 = MAP_Timer32_getValue(TIMER32_0_BASE);
    method3 = (t0 - t1)/3;

//print out result
    printf("Before any changes\n");
    printf("               Time(us)                   Checksums\n");
    printf("Method1        %6u                     %10u \n", method1, hwCalculatedCRC);
    printf("Method3        %6u                     %10u \n", method3, simpleCalculatedCRC);

////////////////////////////////////////////////////////////////

    myData[20] ^= 1; //invert bit 1 of data 20th
    //method 1:

       t0 = MAP_Timer32_getValue(TIMER32_0_BASE);
       MAP_CRC32_setSeed(CRC32_INIT, CRC32_MODE);

       for (ii = 0; ii < 10240; ii++)
           MAP_CRC32_set8BitData(myData[ii], CRC32_MODE);

       /* Getting the result from the hardware module */
       hwCalculatedCRC = MAP_CRC32_getResultReversed(CRC32_MODE) ^ 0xFFFFFFFF;
       t1 = MAP_Timer32_getValue(TIMER32_0_BASE);
       method1 = (t0 - t1)/3;

   //method 3:
       t0 = MAP_Timer32_getValue(TIMER32_0_BASE);

       simpleCalculatedCRC = compute_simple_checksum((uint8_t*) myData, 10240);

       t1 = MAP_Timer32_getValue(TIMER32_0_BASE);
       method3 = (t0 - t1)/3;

    printf("\nAfter inverting the LSB of the 20th value:\n");
    printf("               Time(us)                   Checksums \n");
    printf("Method1        %6u                     %10u \n", method1, hwCalculatedCRC);
    printf("Method3        %6u                     %10u  \n", method3, simpleCalculatedCRC);

////////////////////////////////////////////////////////////////
        myData[20] ^= 1;  //revert bit 1 of data 20th
        myData[21] ^= 1;  //invert bit 1 of data 21st
        //method 1:

           t0 = MAP_Timer32_getValue(TIMER32_0_BASE);
           MAP_CRC32_setSeed(CRC32_INIT, CRC32_MODE);

           for (ii = 0; ii < 10240; ii++)
               MAP_CRC32_set8BitData(myData[ii], CRC32_MODE);

           /* Getting the result from the hardware module */
           hwCalculatedCRC = MAP_CRC32_getResultReversed(CRC32_MODE) ^ 0xFFFFFFFF;
           t1 = MAP_Timer32_getValue(TIMER32_0_BASE);
           method1 = (t0 - t1)/3;

       //method 3:
           t0 = MAP_Timer32_getValue(TIMER32_0_BASE);

           simpleCalculatedCRC = compute_simple_checksum((uint8_t*) myData, 10240);

           t1 = MAP_Timer32_getValue(TIMER32_0_BASE);
           method3 = (t0 - t1)/3;

        printf("\nAfter inverting the LSB of the 21th value:\n");
        printf("               Time(us)                   Checksums \n");
        printf("Method1        %6u                     %10u \n", method1, hwCalculatedCRC);
        printf("Method3        %6u                     %10u  \n", method3, simpleCalculatedCRC);



    /* Pause for the debugger */
    __no_operation();



}
//![Simple CRC32 Example] 

/* Standard software calculation of CRC32 */
static uint32_t calculateCRC32(uint8_t* data, uint32_t length)
{
    uint32_t ii, jj, byte, crc, mask;;

    crc = 0xFFFFFFFF;

    for(ii=0;ii<length;ii++)
    {
        byte = data[ii];
        crc = crc ^ byte;

        for (jj = 0; jj < 8; jj++)
        {
            mask = -(crc & 1);
            crc = (crc >> 1) ^ (CRC32_POLY & mask);
        }

    }

    return ~crc;
}

void EvenOddAlterRand (uint8_t* data, uint32_t length){
    uint8_t value;
    bool even = false;
    int i;

    for(i = 0; i < length; i = i+1){ //odd and even alternate random generator
        value = rand();
        if(value%2 == 0 && !even){//if even and previously odd
            data[i] = value;
            even = true;
        }
        else if(value%2 != 0 && even){//if odd and previously even
            data[i] = value;
            even = false;
        }
        else{//skip if the nexr number remain odd or even the same as previous
            i--; //undo the increment
            continue;
        }
        //printf("%u\n", data[i]);
    }
}

uint32_t compute_simple_checksum(uint8_t* data, uint32_t length){
    uint32_t sum = 0;
    int i;
    for(i =0; i<length; i++){
        sum += data[i];      //sum all the data
    }
    return ~sum;   //reverse the result
}
