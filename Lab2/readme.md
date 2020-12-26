# Introduction
The purpose of this lab assignment is to get familiarized with the bit-banding feature of the MSP432 MCU, the 32-bit timer to measure the elapsed time, and the CRC-32 accelerator.

## Exercise 1.1
For this task, I need to look for the pin number of the blue LED in the schematic from User’s guide LaunchPad which is `P2.2`. To set up and control the LEDs, I use the **DriverLib User’s Guide** to find the GPIO functions as shown in *Figure 1* below.

```c
  /* Configuring 3 pin as output */
    MAP_GPIO_setAsOutputPin(GPIO_PORT_P2, GPIO_PIN0);     //RED   P2.0 as output
    MAP_GPIO_setAsOutputPin(GPIO_PORT_P2, GPIO_PIN1);     //GREEN  P2.1 as output
    MAP_GPIO_setAsOutputPin(GPIO_PORT_P2, GPIO_PIN2);     //BLUE    P2.2 as output

    //set all the pin to Low
    GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN0);     //RED
    GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN1);     //GREEN
    GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN2);     //BLUE
```
*Figure 1: DriverLib’s function to set up LEDs as output and turn off LEDs*

## Exercise 1.2
According to the datasheet, the `base port address` is `0x40004c00` and the `offset` is `0x003`. Therefore, the port address for the output register is `0x40004c03`. Since the blue LED is port `2.2`, `bit 2` is used to control the blue LED. To toggle the blue LED, I use `XOR(^)` operation between the output register and bit 2 such as:
	`*PORT2_prt ^= 0x4;`
  
## Exercise 1.3
After I found the alias address of port 2 bit 2, I used a pointer to point to that alias address. Then, the pointer is assigned 1 to set bit 2 to turn on LED, 0 to turn off LED. In addition, I used invert operation (~) to toggle the LED in while loop as shown in *Figure 2* below. According to the computation from the debugger, the alias address = `0x42098068`

```c
       //calculate alias address of port 2.2
    uint32_t alias_addr = (port_addr - peripheral_region_addr)*32 + bit_position*4 + alias_region_addr;
    uint8_t* PORT2_BIT2 = (uint8_t*) alias_addr; //pointer to the alias address

    /* Halting the Watchdog */
    MAP_WDT_A_holdTimer();

    /* Configuring P1.0 as output */
    MAP_GPIO_setAsOutputPin(GPIO_PORT_P2, GPIO_PIN2); //set pint as output

    *PORT2_BIT2 &= 0; // initial turn off LED by clearing bit 2

    while (1)
    {
        /* Delay Loop */
        for(ii=0;ii<100000;ii++)
        {
        }
        *PORT2_BIT2 = ~(*PORT2_BIT2);  // toggle bit 2
    }

```
*Figure 2: Relevant code to toggle the blue LED*

## Exercise 2
To calculate the frequency, I measure the `ON` and `OFF` duration. Then, I calculate the total period using the equation: **Tperiod = (Ton + Toff)/ frequency)**. In this case, the frequency is `3MHz`, the `ON` and `OFF` period is equal. The *Figure 3* below demonstrates the measurement.  Since the `ON` and `OFF` duration are constant, the blinking period remains constant (around `993.37 ms`). Therefore, the blinking frequency is around `1Hz`.

```c
uint32_t t0, t1;
    uint32_t elapsed_time = 0;
    while (1)
    {
        t0 = MAP_Timer32_getValue(TIMER32_0_BASE);
        /* Delay Loop */
        for(ii=0;ii<100000;ii++)
        {
        }
        *PORT2_BIT2 = ~(*PORT2_BIT2);
        t1 = MAP_Timer32_getValue(TIMER32_0_BASE);
        elapsed_time += t0-t1;
        if(*PORT2_BIT2 == 0){
            printf("T0: %u, T1: %u\n", t0, t1);
            printf("Time elapsed: %.2f miliseconds \n", ((float)elapsed_time/3000));
            elapsed_time = 0;
        }
    }
```
*Figure 3: The relevant code that does the measurement*


![sdfd](https://lh3.googleusercontent.com/-z5jlFuYDQ6sWA3eopaaC6t9WGZyadB3ll9Fx85ZfzKH9hvDcW_T0PFoUi9pj1Fo0DQtLvBGdvwNQ4mOqpMe9RFb50MsqphN0nqFfoD1lWlI-PvPnB0c2XpxrcYrdAvIrKhy-e8D)

*Figure 4: Sample of debugging console output*

## Exercise 3.1
The below code is to insert random odd and even number alternatively to the array. Specifically, I used the `rand()` function as a general random number generator. If the `rand()` produces an even number and the number before it is not even, we insert the number to the array. Same logic applies to odd cases. To indicate the previous number is odd or even, I use a bool variable to set the even and odd status.

```c
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
```
*Figure 5: The function code to generate the random data*

## Exercise 3.2
To perform the simple checksum, I used a for loop to add all the array members in a sum variable. Then, I invert its value using ~ operation.

```c
uint32_t compute_simple_checksum(uint8_t* data, uint32_t length){
    uint32_t sum = 0;
    int i;
    for(i =0; i<length; i++){
        sum += data[i];      //sum all the data
    }
    return ~sum;   //reverse the result
}
```

*Figure 6: The function code to calculate the simple checksums.*

## Exercise 3.3
For this task, I measured the elapsed time for each method and converted them into micro seconds(us) as shown below in *Figure 8*. Specifically, method 1 is hardware CRC checksums, method 2 is software CRC checksums, and method 3 is the simple checksums from the previous exercise.  According to the debug output console in *Figure 7*, although method 1 and method 2 give the same checksums value,  method 1 is `82.42%` faster than method 2. 
![asdf](https://lh4.googleusercontent.com/KWsLLfJCPKv6BcthdkllkLofqyTbFoTzVCXS9ugATfVcgvT4T3pNR1LhSHTymjzCMwzaSWBd3tdbnelX2M9Ht5yg40SP4CO70wbIh8O0n9JCz612RfEiRs_ehoabALlyvou-ihQL)
*Figure 7: The debug console outputs that include time, checksums, and speedup percentage*

```c
//method 1:
    t0 = MAP_Timer32_getValue(TIMER32_0_BASE);
    MAP_CRC32_setSeed(CRC32_INIT, CRC32_MODE);
    for (ii = 0; ii < 10240; ii++)
        MAP_CRC32_set8BitData(myData[ii], CRC32_MODE);
    /* Getting the result from the hardware module */
    hwCalculatedCRC = MAP_CRC32_getResultReversed(CRC32_MODE) ^ 0xFFFFFFFF;
    t1 = MAP_Timer32_getValue(TIMER32_0_BASE);
    method1 = (t0 - t1)/3;
//method 2:
    /* Calculating the CRC32 checksum through software */
    t0 = MAP_Timer32_getValue(TIMER32_0_BASE);
    swCalculatedCRC = calculateCRC32((uint8_t*) myData, 10240);
    t1 = MAP_Timer32_getValue(TIMER32_0_BASE);
    method2 = (t0 - t1)/3;
//method 3:
    t0 = MAP_Timer32_getValue(TIMER32_0_BASE);
    simpleCalculatedCRC = compute_simple_checksum((uint8_t*) myData, 10240);
    t1 = MAP_Timer32_getValue(TIMER32_0_BASE);
    method3 = (t0 - t1)/3;
//print out result
    printf("               Time(us)                   Checksums               Speedup\n");
    printf("Method1        %6u                     %10u              %.2f%%       \n", method1, hwCalculatedCRC,((float)(method2-method1)/method2)*100);
    printf("Method2        %6u                     %10u                    \n", method2, swCalculatedCRC);
    printf("Method3        %6u                     %10u                           \n", method3, simpleCalculatedCRC);
```
*Figure 8: The code that does the measurement*

## Exercise 3.4
The primary task is to compare the checksums quality of method 1 and 3.

```c
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
```

*Figure 9: The code that generate three checksums results*

![sdf](https://lh5.googleusercontent.com/6vfw8pLtlL5rL-5eu9pIt3i1IFVujCKrvRABQbZFwrdjAu4CJAy7Ex2V8ewjsYkOW0MAaoDNOaluDU_BP7J-VNIdROTpwktd7hM-DaE-WZpDuzQZhHmvkCopGXiQMw_lOR-800We)

*Figure 10: The debug console outputs that shows times and checksums*

According to `Figure 10`, when changing the first bit of the `20th` or `21th` value, the checksums result for method 3 were only increment or decrement by 1 compared to the initial measurement. On the other hand, the CRC32 method (method 3) gave the output that was drastically different from original checksums. For this reason, although method 3 took longer to generate, it is better than method 1 when determining the data corruption.


