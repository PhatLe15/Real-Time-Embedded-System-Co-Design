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
 * MSP432 DMA - CRC32 calculation using DMA
 *
 * Description: This code examples shows how to use the DMA module on MSP432 
 * to feed data into the CRC32 module for a CRC32 signature calculation. This
 * use case is particularly useful when the user wants to calculate the CRC32
 * signature of a large data array (such as a firmware image) but still wants
 * to maximize power consumption. After the DMA transfer is setup, a software
 * initiation occurs and the MSP432 device is put to sleep. Once the transfer
 * completes, the DMA interrupt occurs and the CRC32 result is placed into
 * a local variable for the user to examine.
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
/* DriverLib Includes */
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>

/* Standard Includes */
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#define CRC32_SEED      0XFFFFFFFF

/* Statics */
static volatile uint32_t crcSignature;

/* DMA Control Table */
#if defined(__TI_COMPILER_VERSION__)
#pragma DATA_ALIGN(controlTable, 1024)
#elif defined(__IAR_SYSTEMS_ICC__)
#pragma data_alignment=1024
#elif defined(__GNUC__)
__attribute__ ((aligned (1024)))
#elif defined(__CC_ARM)
__align(1024)
#endif

uint8_t controlTable[1024];

/* Extern */
//extern uint8_t data_array[];

uint8_t data_array [10240];                              //data array
int size_array[] = {512,1024,1030,1824,2048,2049,2303,10240};     //array of sizes
volatile int dma_done;                                  //done flag
volatile int size;
volatile int start_addr;

int main(void)
{
    uint32_t hwTime, dmaTime, t0, t1;
    int i;


    //initialize timer
    MAP_Timer32_initModule(TIMER32_0_BASE, TIMER32_PRESCALER_1, TIMER32_32BIT, TIMER32_FREE_RUN_MODE);
    MAP_Timer32_startTimer(TIMER32_0_BASE, 0);
    printf("%u\n", MAP_CS_getMCLK());

    /* Halting Watchdog */
    MAP_WDT_A_holdTimer();

    MAP_DMA_enableModule();  //only need to call once
    MAP_DMA_setControlBase(controlTable);

    /* Assigning/Enabling Interrupts */
    MAP_DMA_assignInterrupt(DMA_INT1, 0);
    MAP_Interrupt_enableInterrupt(INT_DMA_INT1);
    MAP_Interrupt_enableMaster();

    MAP_DMA_setChannelControl(UDMA_PRI_SELECT,
                              UDMA_SIZE_8 | UDMA_SRC_INC_8 | UDMA_DST_INC_NONE | UDMA_ARB_1024);

    for(i = 0; i < 8; i++){         //only 9 different sizes
        start_addr = 0;
        size = size_array[i];
        dma_done = 0;  //doneflag off before the transfer

        //HW method:
        t0 = MAP_Timer32_getValue(TIMER32_0_BASE);
        MAP_CRC32_setSeed(CRC32_SEED, CRC32_MODE);
        int ii;
        for (ii = 0; ii < size_array[i]; ii++)
            MAP_CRC32_set8BitData(data_array[ii], CRC32_MODE);
        uint32_t hwCRC = MAP_CRC32_getResult(CRC32_MODE);
        t1 = MAP_Timer32_getValue(TIMER32_0_BASE);
        hwTime = (t0-t1)/3;

        //DMA method:
        /* Configuring DMA module */
        t0 = MAP_Timer32_getValue(TIMER32_0_BASE);



        if(size_array[i] < 1024){ //transfer entire array
            MAP_DMA_setChannelTransfer(UDMA_PRI_SELECT,
                                       UDMA_MODE_AUTO, data_array, (void*) (&CRC32->DI32), size_array[i]);
        }
        else {// transfer 1024 data for every transfer
            MAP_DMA_setChannelTransfer(UDMA_PRI_SELECT,
                                       UDMA_MODE_AUTO, data_array, (void*) (&CRC32->DI32), 1024);
        }

        /* Enabling DMA Channel 0 */
        MAP_DMA_enableChannel(0);

        //reinitialize the result register in the CRC-32 accelerator
        MAP_CRC32_setSeed(CRC32_SEED, CRC32_MODE);

        /* Forcing a software transfer on DMA Channel 0 */
        MAP_DMA_requestSoftwareTransfer(0);

        while(1)
        {
            //MAP_PCM_gotoLPM0();
            if(dma_done == 1){  //if done flag is set
                break;
            }
        }

        t1 = MAP_Timer32_getValue(TIMER32_0_BASE);
        dmaTime = (t0 - t1)/3;

        //calculate speed up time
        float speedup = ((float)(hwTime)/dmaTime);

        printf("For array size of %u :\n", size_array[i]);
        printf("                 Time(us)                   Checksums             Speedup\n");
        printf("hwMethod         %6d                     %08x \n", hwTime, hwCRC);
        printf("dmaMethod        %6d                     %08x              %.2f       \n\n", dmaTime, crcSignature, speedup );

    }//size variable for loop
}

/* Completion interrupt for DMA */
void DMA_INT1_IRQHandler(void)
{
    MAP_DMA_disableChannel(0);
    size = size - 1024; // to view the remaining data size after transfered
    start_addr += 1024;

    if(size <= 0){ // if all data is transfered
        crcSignature = MAP_CRC32_getResult(CRC32_MODE);
        dma_done = 1;  //done flag on

    }else if(size > 1024){ // if the size is still greater setup another DMA transfer
        //set up DMA
        //MAP_DMA_setChannelControl(UDMA_PRI_SELECT,
         //                         UDMA_SIZE_8 | UDMA_SRC_INC_8 | UDMA_DST_INC_NONE | UDMA_ARB_1024);

        MAP_DMA_setChannelTransfer(UDMA_PRI_SELECT,
                                   UDMA_MODE_AUTO, data_array + start_addr, (void*) (&CRC32->DI32), 1024);

        /* Enabling DMA Channel 0 */
        MAP_DMA_enableChannel(0);

        /* Forcing a software transfer on DMA Channel 0 */
        MAP_DMA_requestSoftwareTransfer(0);

    }else if (size <= 1024){
        //MAP_DMA_setControlBase(controlTable);

        /* Setting Control Indexes. In this case we will set the source of the
         * DMA transfer to our random data array and the destination to the
         * CRC32 data in register address*/
       // MAP_DMA_setChannelControl(UDMA_PRI_SELECT,
        //                          UDMA_SIZE_8 | UDMA_SRC_INC_8 | UDMA_DST_INC_NONE | UDMA_ARB_1024);

        MAP_DMA_setChannelTransfer(UDMA_PRI_SELECT,
                                   UDMA_MODE_AUTO, data_array + start_addr, (void*) (&CRC32->DI32), size); //transfer the rest of data

        /* Enabling DMA Channel 0 */
        MAP_DMA_enableChannel(0);

        /* Forcing a software transfer on DMA Channel 0 */
        MAP_DMA_requestSoftwareTransfer(0);
    }

}
