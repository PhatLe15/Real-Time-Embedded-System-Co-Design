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
#include <stdio.h>

#include <string.h>
#include <stdbool.h>

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

#define CRC32_SEED 0xFFFFFFFF
/* Extern */
//extern uint8_t data_array[];
uint8_t data_array[10240];
int size, address;

volatile int dma_done;
int size_array[] = {512, 1024, 1030, 1824, 2048, 2049, 2303, 10240};

int main(void)
{
    double freq;
    volatile uint32_t t1;
    volatile uint32_t t2;
    volatile uint32_t t3;
    volatile uint32_t t4;
    int ii;
    int j;
    uint32_t hw, dma;
    float ratio;
    MAP_DMA_enableModule();
    dma_done = 0;


    for (j=0; j<8; j++)
    {
        size = size_array[j];
        address = 0;
        MAP_Timer32_initModule(TIMER32_0_BASE, TIMER32_PRESCALER_1, TIMER32_32BIT, TIMER32_FREE_RUN_MODE);
        MAP_Timer32_startTimer(TIMER32_0_BASE, 0);
        freq = MAP_CS_getMCLK()/1000000;;

        /* Halting Watchdog */
        MAP_WDT_A_holdTimer();

        //Hardware Method

        t1 = MAP_Timer32_getValue(TIMER32_0_BASE);
        MAP_CRC32_setSeed(CRC32_SEED, CRC32_MODE);

        for (ii=0; ii < size; ii++)
        {
            MAP_CRC32_set8BitData(data_array[ii], CRC32_MODE);
        }

        uint32_t hwCRC = MAP_CRC32_getResult(CRC32_MODE);
        t2 = MAP_Timer32_getValue(TIMER32_0_BASE);
        printf("Size = %d hwCRC = %08x\n", size, hwCRC);
        hw = (t1-t2)/freq;
        printf("CRC-32 running time from hardware method is: %d us\n", hw);



        /* Configuring DMA module */


        t3 = MAP_Timer32_getValue(TIMER32_0_BASE);

        MAP_DMA_setControlBase(controlTable);

        /* Setting Control Indexes. In this case we will set the source of the
         * DMA transfer to our random data array and the destination to the
         * CRC32 data in register address*/
        MAP_DMA_setChannelControl(UDMA_PRI_SELECT,
                UDMA_SIZE_8 | UDMA_SRC_INC_8 | UDMA_DST_INC_NONE | UDMA_ARB_1024);
        if (size <= 1024)
        {
            MAP_DMA_setChannelTransfer(UDMA_PRI_SELECT,
                UDMA_MODE_AUTO, data_array,
                (void*) (&CRC32->DI32), size_array[j]);
        } else
        {
            MAP_DMA_setChannelTransfer(UDMA_PRI_SELECT,
                            UDMA_MODE_AUTO, data_array,
                            (void*) (&CRC32->DI32), 1024);
        }

        /* Assigning/Enabling Interrupts */
        MAP_DMA_assignInterrupt(DMA_INT1, 0);
        MAP_Interrupt_enableInterrupt(INT_DMA_INT1);
        MAP_Interrupt_enableMaster();

        /* Enabling DMA Channel 0 */
        MAP_DMA_enableChannel(0);
        MAP_CRC32_setSeed(CRC32_SEED, CRC32_MODE);
        /* Forcing a software transfer on DMA Channel 0 */

        MAP_DMA_requestSoftwareTransfer(0);

        while(dma_done == 0)
        {

        }
        t4 = MAP_Timer32_getValue(TIMER32_0_BASE);
        dma = (t3-t4)/freq;
        printf("Size = %d crcSignature = %08x\n", size_array[j], crcSignature);
        printf("CRC-32 running time from dma method is: %d us\n", dma);
        ratio = (((float)hw)/dma);
        printf("Ratio hw/dma =  %f\n\n", ratio);
    }
}

/* Completion interrupt for DMA */
void DMA_INT1_IRQHandler(void)
{
    size = size - 1024;
    address = address + 1024;
    MAP_DMA_disableChannel(0);

    if (size <= 0)
    {
        crcSignature = MAP_CRC32_getResult(CRC32_MODE);
        dma_done = 1;

    } else if (size <= 1024)
    {
        MAP_DMA_setChannelControl(UDMA_PRI_SELECT,
                UDMA_SIZE_8 | UDMA_SRC_INC_8 | UDMA_DST_INC_NONE | UDMA_ARB_1024);
        MAP_DMA_setChannelTransfer(UDMA_PRI_SELECT,
            UDMA_MODE_AUTO, data_array + address,
            (void*) (&CRC32->DI32), size);
        /* Assigning/Enabling Interrupts */
  //      MAP_DMA_assignInterrupt(DMA_INT1, 0);
  //      MAP_Interrupt_enableInterrupt(INT_DMA_INT1);
  //      MAP_Interrupt_enableMaster();

        /* Enabling DMA Channel 0 */
        MAP_DMA_enableChannel(0);
  //      MAP_CRC32_setSeed(CRC32_SEED, CRC32_MODE);
        /* Forcing a software transfer on DMA Channel 0 */

        MAP_DMA_requestSoftwareTransfer(0);
    } else if (size > 1024)
    {
        MAP_DMA_setChannelControl(UDMA_PRI_SELECT,
                UDMA_SIZE_8 | UDMA_SRC_INC_8 | UDMA_DST_INC_NONE | UDMA_ARB_1024);
        MAP_DMA_setChannelTransfer(UDMA_PRI_SELECT,
            UDMA_MODE_AUTO, data_array + address,
            (void*) (&CRC32->DI32), 1024);
        /* Assigning/Enabling Interrupts */
   //     MAP_DMA_assignInterrupt(DMA_INT1, 0);
   //     MAP_Interrupt_enableInterrupt(INT_DMA_INT1);
   //     MAP_Interrupt_enableMaster();

        /* Enabling DMA Channel 0 */
        MAP_DMA_enableChannel(0);
    //    MAP_CRC32_setSeed(CRC32_SEED, CRC32_MODE);
        /* Forcing a software transfer on DMA Channel 0 */

        MAP_DMA_requestSoftwareTransfer(0);
    }
    //crcSignature = MAP_CRC32_getResult(CRC32_MODE);
}


