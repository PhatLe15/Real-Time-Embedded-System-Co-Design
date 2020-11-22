# Introduction
The purpose of this lab assignment is to get familiarized with the DMA and the interrupts functionality on the MSP432 MCU.

# Exercise 1
The main task for this exercise is to set up the DMA to transfer an data array of size `1024` to the CRC32 accelerator. Then, the result is printed out from the ISR when the transaction is finished. The below figure displays the output result from the console with and without using the DMA.  We can see that the `dma method` gives the same checksum as the `hardware method`.

```
[CORTEX_M4_0] hwCRC=8b0a5208
              dmaCRC=8b0a5208
```
*Figure 1: Output read from debug console for Exercise 1*

```c
int main(void)
{
    /* Halting Watchdog */
    MAP_WDT_A_holdTimer();
    //HW method:
    MAP_CRC32_setSeed(CRC32_SEED, CRC32_MODE);
    int ii;
    for (ii = 0; ii < sizeof(data_array); ii++)
        MAP_CRC32_set8BitData(data_array[ii], CRC32_MODE);
    uint32_t hwCRC = MAP_CRC32_getResult(CRC32_MODE);
    printf("hwCRC=%08x\n", hwCRC);
    //DMA method:
    /* Configuring DMA module */
    MAP_DMA_enableModule();
    MAP_DMA_setControlBase(controlTable);
    /* Setting Control Indexes. In this case we will set the source of the
     * DMA transfer to our random data array and the destination to the
     * CRC32 data in register address*/
    MAP_DMA_setChannelControl(UDMA_PRI_SELECT,
            UDMA_SIZE_8 | UDMA_SRC_INC_8 | UDMA_DST_INC_NONE | UDMA_ARB_1024);
    MAP_DMA_setChannelTransfer(UDMA_PRI_SELECT,
            UDMA_MODE_AUTO, data_array, (void*) (&CRC32->DI32), 1024);
    /* Assigning/Enabling Interrupts */
    MAP_DMA_assignInterrupt(DMA_INT1, 0);
    MAP_Interrupt_enableInterrupt(INT_DMA_INT1);
    MAP_Interrupt_enableMaster();
   
    /* Enabling DMA Channel 0 */
    MAP_DMA_enableChannel(0);
    //reinitialize the result reigster in the CRC-32 accelerator
    MAP_CRC32_setSeed(CRC32_SEED, CRC32_MODE);
    /* Forcing a software transfer on DMA Channel 0 */
    MAP_DMA_requestSoftwareTransfer(0);
    while(1)
    {
        MAP_PCM_gotoLPM0();
    }
}
/* Completion interrupt for DMA */
void DMA_INT1_IRQHandler(void)
{
    MAP_DMA_disableChannel(0);
    crcSignature = MAP_CRC32_getResult(CRC32_MODE);
    printf("dmaCRC=%08x\n", crcSignature);
}
```
## Exercise 2
Since using printf inside the ISR is a bad coding technique, this exercise will print out after the ISR finished and return to the main program. Specifically, a `done` flag is asserted from the ISR when the DMA transfer is done. In addition, the `done` flag will break the while loop from the main program when it is asserted. Lastly, we compare the elapsed time between both method and the speedup of the dma method over the `hw method`.

```
[CORTEX_M4_0] 3000000
                 Time(us)                   Checksums             Speedup
hwMethod          13007                     8b0a5208
dmaMethod          2920                     8b0a5208              4.45  
```
*Figure 2: Output read from debug console for Exercise 2*

According to *Figure 2*, the `dma method` speedup is 4.45 times compared to the `hardware method`.

```c
int main(void)
{
    uint32_t hwTime, dmaTime, t0, t1;
    //initialize timer
    MAP_Timer32_initModule(TIMER32_0_BASE, TIMER32_PRESCALER_1, TIMER32_32BIT, TIMER32_FREE_RUN_MODE);
    MAP_Timer32_startTimer(TIMER32_0_BASE, 0);
    printf("%u\n", MAP_CS_getMCLK());
    /* Halting Watchdog */
    MAP_WDT_A_holdTimer();
    //HW method:
    t0 = MAP_Timer32_getValue(TIMER32_0_BASE);
    MAP_CRC32_setSeed(CRC32_SEED, CRC32_MODE);
    int ii;
    for (ii = 0; ii < sizeof(data_array); ii++)
        MAP_CRC32_set8BitData(data_array[ii], CRC32_MODE);
    uint32_t hwCRC = MAP_CRC32_getResult(CRC32_MODE);
    t1 = MAP_Timer32_getValue(TIMER32_0_BASE);
    hwTime = (t0-t1)/3;
    //printf("hwCRC=%08x\n", hwCRC);
    //DMA method:
    /* Configuring DMA module */
    t0 = MAP_Timer32_getValue(TIMER32_0_BASE);
    MAP_DMA_enableModule();
    MAP_DMA_setControlBase(controlTable);
    /* Setting Control Indexes. In this case we will set the source of the
     * DMA transfer to our random data array and the destination to the
     * CRC32 data in register address*/
    MAP_DMA_setChannelControl(UDMA_PRI_SELECT,
            UDMA_SIZE_8 | UDMA_SRC_INC_8 | UDMA_DST_INC_NONE | UDMA_ARB_1024);
    MAP_DMA_setChannelTransfer(UDMA_PRI_SELECT,
            UDMA_MODE_AUTO, data_array, (void*) (&CRC32->DI32), 1024);
    /* Assigning/Enabling Interrupts */
    MAP_DMA_assignInterrupt(DMA_INT1, 0);
    MAP_Interrupt_enableInterrupt(INT_DMA_INT1);
    MAP_Interrupt_enableMaster();
   
    /* Enabling DMA Channel 0 */
    MAP_DMA_enableChannel(0);
    //reinitialize the result reigster in the CRC-32 accelerator
    MAP_CRC32_setSeed(CRC32_SEED, CRC32_MODE);
    dma_done = 0;  //doneflag off before the transfer
    /* Forcing a software transfer on DMA Channel 0 */
    MAP_DMA_requestSoftwareTransfer(0);
    while(1)
    {
        //MAP_PCM_gotoLPM0();
        if(dma_done == 1){
            break;
        }
    }
    t1 = MAP_Timer32_getValue(TIMER32_0_BASE);
    dmaTime = (t0 - t1)/3;
    printf("                 Time(us)                   Checksums             Speedup\n");
    printf("hwMethod         %6u                     %08x \n", hwTime, hwCRC);
    printf("dmaMethod        %6u                     %08x              %.2f       \n", dmaTime, crcSignature, (((float)hwTime)/dmaTime));
}
/* Completion interrupt for DMA */
void DMA_INT1_IRQHandler(void)
{
    MAP_DMA_disableChannel(0);
    crcSignature = MAP_CRC32_getResult(CRC32_MODE);
    dma_done = 1;  //done flag on
}
```
## Exercise 3
The requirement for this exercise is to measure the time and speedup of different array sizes for dma data transfer as well as checksum calculation. Specifically, a big for loop is implemented to swap out different sizes stored in an array(size_array). The next step is similar to exercise 2.
```
[CORTEX_M4_0] 3000000
For array size of 2 :
                 Time(us)                   Checksums             Speedup
hwMethod             68                     00b7647d
dmaMethod           187                     00b7647d              0.36      
For array size of 4 :
                 Time(us)                   Checksums             Speedup
hwMethod             98                     c704dd7b
dmaMethod           193                     c704dd7b              0.51      
For array size of 16 :
                 Time(us)                   Checksums             Speedup
hwMethod            274                     552d22c8
dmaMethod           225                     552d22c8              1.22      
For array size of 32 :
                 Time(us)                   Checksums             Speedup
hwMethod            509                     4a55af67
dmaMethod           268                     4a55af67              1.90      
For array size of 64 :
                 Time(us)                   Checksums             Speedup
hwMethod            978                     93394e51
dmaMethod           353                     93394e51              2.77      
For array size of 128 :
                 Time(us)                   Checksums             Speedup
hwMethod           1917                     46a0eabc
dmaMethod           524                     46a0eabc              3.66      
For array size of 256 :
                 Time(us)                   Checksums             Speedup
hwMethod           3794                     e55e964f
dmaMethod           865                     e55e964f              4.39      
For array size of 786 :
                 Time(us)                   Checksums             Speedup
hwMethod          11567                     30b4c8f3
dmaMethod          2278                     30b4c8f3              5.08      
For array size of 1024 :
                 Time(us)                   Checksums             Speedup
hwMethod          15058                     8b0a5208
dmaMethod          2913                     8b0a5208              5.17 
```
*Figure 3: Output read from debug console for Exercise 3*

According to *Figure 3*, the smaller the size of data the dma transferred, the longer the time it takes to obtain the checksum. The reason could come from the time it takes to set up and initialize the dma controller which is longer than the time to transfer just a few bytes of data using for loop.
```c
According to Figure 3, the smaller the size of data the dma transferred, the longer the time it takes to obtain the checksum. The reason could come from the time it takes to set up and initialize the dma controller which is longer than the time to transfer just a few bytes of data using for loop.
    for(i = 0; i < 9; i++){         //only 9 different sizes
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
            /* Setting Control Indexes. In this case we will set the source of the
             * DMA transfer to our random data array and the destination to the
             * CRC32 data in register address*/
            MAP_DMA_setChannelControl(UDMA_PRI_SELECT,
                    UDMA_SIZE_8 | UDMA_SRC_INC_8 | UDMA_DST_INC_NONE | UDMA_ARB_1024);
            MAP_DMA_setChannelTransfer(UDMA_PRI_SELECT,
                    UDMA_MODE_AUTO, data_array, (void*) (&CRC32->DI32), size_array[i]);
            /* Enabling DMA Channel 0 */
            MAP_DMA_enableChannel(0);
            //reinitialize the result reigster in the CRC-32 accelerator
            MAP_CRC32_setSeed(CRC32_SEED, CRC32_MODE);
            dma_done = 0;  //doneflag off before the transfer
            /* Forcing a software transfer on DMA Channel 0 */
            MAP_DMA_requestSoftwareTransfer(0);
            while(1)
            {
                //MAP_PCM_gotoLPM0();
                if(dma_done == 1){
                    break;
                }
            }
            t1 = MAP_Timer32_getValue(TIMER32_0_BASE);
            dmaTime = (t0 - t1)/3;
            float speedup = ((float)(hwTime)/dmaTime);
            printf("For array size of %u :\n", size_array[i]);
            printf("                 Time(us)                   Checksums             Speedup\n");
            printf("hwMethod         %6d                     %08x \n", hwTime, hwCRC);
            printf("dmaMethod        %6d                     %08x              %.2f       \n\n", dmaTime, crcSignature, speedup );
    }//for loop
```
## Exercise 4
 For this exercise, the primary task is to perform the dma transaction for large size of data arrays. The plan is to have different dma to transfer smaller blocks of 1024 bytes data. The checksums will print out only when all data from the array are transferred. If the size is greater than 1024 bytes, the ISR will set up another dma to transfer another block of 1024 bytes.  
 ```c
 [CORTEX_M4_0] 3000000
For array size of 512 :
                 Time(us)                   Checksums             Speedup
hwMethod           7549                     e151aab2
dmaMethod          1509                     e151aab2              5.00      
For array size of 1024 :
                 Time(us)                   Checksums             Speedup
hwMethod          15058                     8b0a5208
dmaMethod          2875                     8b0a5208              5.24      
For array size of 1030 :
                 Time(us)                   Checksums             Speedup
hwMethod          15146                     16d87f90
dmaMethod          2996                     16d87f90              5.06      
For array size of 1824 :
                 Time(us)                   Checksums             Speedup
hwMethod          26791                     93e9aaef
dmaMethod          5112                     93e9aaef              5.24      
For array size of 2048 :
                 Time(us)                   Checksums             Speedup
hwMethod          30076                     86a2e870
dmaMethod          5711                     86a2e870              5.27      
For array size of 2049 :
                 Time(us)                   Checksums             Speedup
hwMethod          30091                     d162dd5c
dmaMethod          5814                     d162dd5c              5.18      
For array size of 2303 :
                 Time(us)                   Checksums             Speedup
hwMethod          33817                     c5bae04d
dmaMethod          6492                     c5bae04d              5.21      
For array size of 10240 :
                 Time(us)                   Checksums             Speedup
hwMethod         150226                     a684471b
dmaMethod         28368                     a684471b              5.30 
 ```
 *Figure 4: Output read from debug console for Exercise 3*
 
 According to *Figure 4* above, as the size increases, the speedup time is also increased. For this reason, the continuous `dma transfer method` is more efficient for large data transfer. Compared to the previous exercise, for a size of `1024 bytes`, the `old` method is slightly faster since more instructions and conditional terms are added to the `new` method.
 
 ```c
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
        /* Setting Control Indexes. In this case we will set the source of the
         * DMA transfer to our random data array and the destination to the
         * CRC32 data in register address*/
        MAP_DMA_setChannelControl(UDMA_PRI_SELECT,
                                  UDMA_SIZE_8 | UDMA_SRC_INC_8 | UDMA_DST_INC_NONE | UDMA_ARB_1024);
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
 ```
