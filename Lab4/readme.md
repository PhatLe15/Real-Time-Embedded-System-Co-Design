# Introduction
The purpose of this lab assignment is to have a better understanding of the memory subsystem  on the MSP432 MCU by making changes on the contents in the flash memory. 

## Exercise 1.1
The task for this exercise is to compare the two different types of string after we modify the same content. The debug console below shows that **string 1** was unable to modify the first value.  According to the **MCU datasheet**, string 1 data address(`0x00002D70`) which is in the **Flash Main memory** region(`0x00000000` to `00040000`). Furthermore, this region is fully protected as the default. Therefore, any modification is not allowed. On the other hand, **string 2** address(`0x200009E4`) is in the **SRAM** region(`0x20000000` to `0x3FFFFFFF`) which is modifiable. 
```
[CORTEX_M4_0] 
Before: 
string1: 12345
string2: 12345
string1[0] before the attempt to change from 1 to 0: 1
After: 
string1[0] after the attempt to change from 1 to 0 : 1
string1: 12345
string2: 02345

```
*Figure 1: Output read from debug console for Exercise 1.1*

```c
/* DriverLib Includes */
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>

/* Standard Includes */
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

char* string1 = "12345";
char string2[] = "12345";


int main(void)
{
    printf("\nBefore: \n");
    printf("string1: %s\n", string1);
    printf("string2: %s\n", string2);
    printf("string1[0] before the attempt to change from 1 to 0: %c\n", string1[0]);//attempt 2: try to print only the modify char

    string1[0] = '0';
    string2[0] = '0';

    printf("After: \n");
    printf("string1[0] after the attempt to change from 1 to 0 : %c\n", string1[0]); //attempt 2: value still wont change
    printf("string1: %s\n", string1);   //attempt 1: value wont change
    printf("string2: %s\n", string2);   // value can change

}

```
## Exercise 1.2
For this exercise, we will use the **Driver Library Function** to unprotect the exact bank and sector of **string1** and **string** and then modify them using another function.  According to the **MCU datasheet**, the **Flash Main Memory** is divided into `2` banks of `128KB` each and `64` sectors with `4KB`. From exercise 1.1, we know that the **flash main** region is from `0x000000` to `0004000`, therefore, if the string data address is greater than `0x00020000`, it is in `bank 1`. Since the size of a sector is `4096 bytes`, we can calculate the address by using the following equation: **(address-base address)/4096** as implemented in *Figure 3* below. 
```
[CORTEX_M4_0] Region of string 1 is: 
Since the address of this string is: 000259E8
Sector: 5       Bank: 1

Region of string 3 is: 
Since the address of this string is: 000000E4
Sector: 0       Bank: 0

Before:
string1: 12345
string2: 12345
string3: 12345
After
string1: 00345
string2: 04345
string3: 00345
```
*Figure 2: Output read from debug console for Exercise 1.2*

We can observe that even when **string 1** and **string 3** has been programmed, value `‘4’` from **string 2** still does not write to **string 1** and **string 3**. According to the **MCU Datasheet**, we can only program the **Flash Main Memory** from `1` to `0` but not otherwise. For this case, the second byte of **string 1** and **string 3** (`‘2’ = 0x00110010`) was unable to program to be the same as the **second byte** of **string 2** ( `‘4’ = 0x00110100`) since the **2nd bit** and the **4th bit** become `0` after programmed. 

```c
void regionConverter(char* a){
   if(((uint32_t)a) > 0x20000){
       bank = 1;
       sector = ((uint32_t)a-0x20000)/4096;
   }else{
       bank = 0;
       sector = ((uint32_t)a)/4096;
   }
   printf("Since the address of this string is: %08X\n", a);
   printf("Sector: %d       Bank: %d\n\n", sector, bank);
}
Figure 3: Function to find bank and sector in memory

    FlashCtl_unprotectSector(FLASH_MAIN_MEMORY_SPACE_BANK0, FLASH_SECTOR0); //for string 1
    FlashCtl_unprotectSector(FLASH_MAIN_MEMORY_SPACE_BANK1, FLASH_SECTOR5); //for string 3
Figure 3:Function to unprotect sector memory

    ROM_FlashCtl_programMemory(string2, string1, 2);
    ROM_FlashCtl_programMemory(string2, string3, 2);
```
*Figure 4: Function to program the memory*

## Exercise 2.1
Given the `32-bit` Timer, the task for this exercise is to implement the delay function that meets the provided delay value in milliseconds(`5000, 2000, 1000, 5, 1, 0`). We can see that the output console below has successfully been delayed and printed out the time that is close to the given delay value. The extra time difference for the delay function is due to the initialization time of variable and calculation to match the frequency of the system. 
```
[CORTEX_M4_0] MCLK: 3000000
Expected delay time: 5000 ms
ACtual delay time  : 5000151 us
Time difference  : 151 us

Expected delay time: 2000 ms
ACtual delay time  : 2000150 us
Time difference  : 150 us

Expected delay time: 1000 ms
ACtual delay time  : 1000155 us
Time difference  : 155 us

Expected delay time: 5 ms
ACtual delay time  : 5152 us
Time difference  : 152 us

Expected delay time: 1 ms
ACtual delay time  : 1148 us
Time difference  : 148 us

Expected delay time: 0 ms
ACtual delay time  : 160 us
Time difference  : 160 us
```
*Figure 5: Output read from debug console for Exercise 2.1*

According to *Figure 6*, the delay function first converts the required amount to the number of clock cycles needed. Specifically, it divided by `1000` to convert to second and multiplied with the system clock frequency. Then, we record the initial timestamp and use a while loop to keep checking the current timestamp until the difference between them meet the required delay cycles. 
```c
void delay_ms(uint32_t count){
    duration = 0; //initialize the delay time
    count_us = count*(MAP_CS_getMCLK()/1000);// convert to # of cycles needed
    initial = MAP_Timer32_getValue(TIMER32_0_BASE); //initial time stamp

    while(count_us >= duration){//if the actual delay time meet the time
        requirement
        current = MAP_Timer32_getValue(TIMER32_0_BASE);// current time stamp
        duration = initial - current; // update the duration
    }
}
```
*Figure 6: Delay function implementation*

## Exercise 2.2
For this lab, we will use the delay function created above to blink the LED1 at `0.5Hz`. We will use the basic code from the previous lab to set up the LED. According to the **MCU schematic**, **LED 1** is port `P1.0`. To meet the frequency requirement of `0.5Hz`, each `ON` and `OFF` duration needs to be `1 second`.
```c
MAP_GPIO_setAsOutputPin(GPIO_PORT_P1, GPIO_PIN0);     //LED 1 set as output
    GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN1);     //set LED to low

    while(1){
        delay_ms(1000);
        MAP_GPIO_toggleOutputOnPin(GPIO_PORT_P1, GPIO_PIN0); //toggle the LED
    }
```
*Figure 7: Relevant code for blinking LED*

According to *Table 1* below, the frequency of  blinking LED measured from a phone stopwatch is as expected to be constant `0.5Hz`.

*Table 1: Data from stopwatch measurements*

| Duration(second) | Number of blinks(times) | Frequency(Hz) |
---|---|---
| 60|31|0.516|
120|59|0.491
300|148|0.493
||**Average**|0.5

## Exercise 2.3
The requirement for this task is to modify the delay function so that it can handle the wrap around situation where the **initial** timestamp is smaller than the **current** timestamp. To create the `20-bit` counter from a `32-bit` timer, we mask the timestamp by **AND**ing with `0x000ffffff`. From the debug console below, we can see that the delay function has been successfully implemented as the `actual` delay time close to the `expected` time. Similar to previous exercise, the time difference is due to the initialization, the calculation and condition checking instructions within the delay function.
```
[CORTEX_M4_0] MCLK: 3000000
Expected delay time: 10000 ms
Actual delay time  : 10000194 us
Time difference  : 194 us

Expected delay time: 5000 ms
Actual delay time  : 5000190 us
Time difference  : 190 us

Expected delay time: 2000 ms
Actual delay time  : 2000183 us
Time difference  : 183 us

Expected delay time: 1000 ms
Actual delay time  : 1000169 us
Time difference  : 169 us

Expected delay time: 10 ms
Actual delay time  : 10165 us
Time difference  : 165 us

Expected delay time: 5 ms
Actual delay time  : 5191 us
Time difference  : 191 us

Expected delay time: 2 ms
Actual delay time  : 2188 us
Time difference  : 188 us

Expected delay time: 1 ms
Actual delay time  : 1187 us
Time difference  : 187 us

Expected delay time: 0 ms
Actual delay time  : 186 us
Time difference  : 186 us

Expected delay time: 10000 ms
Actual delay time  : 10000169 us
Time difference  : 169 us
```
*Figure 8: Output read from debug console for Exercise 2.2*

To handle the wrap around issue, we can extend the timestamp to a bigger size such as `64-bit` as indicated in the function delay. To check the wrap around, we actively compare the `current` timestamp with the `previous` timestamp which is updated at every end of every `loop`. Every time a **wrap around** happens, we decrement the extended part by `1`. For this case, the extended is after the `5th byte`, therefore, we decrement `0x100000` from the `extended` value. 
```c
void delay_ms(uint32_t count){
    duration = 0; //initialize the actual delay
    count_us = count*(MAP_CS_getMCLK()/1000);// convert to # of cycles needed
    extended = 0xffffffff00000; //extend amount of clock
    initial = (MAP_Timer32_getValue(TIMER32_0_BASE) & TIMER_MAX);
    initial_extended = initial | extended; //extend the initial timestamp
    previous = initial_extended; //initialize the previous timestamp

    while(count_us >= duration){
        current = MAP_Timer32_getValue(TIMER32_0_BASE)  & TIMER_MAX;//extend the current timestamp

        if(current > previous){                 //detect the wrap around
            extended = extended - 0x00100000;   //decrement the extend
        }

        current_extended = current | extended;  //extend the current time
        duration = initial_extended - current_extended; // update the duration
        previous = current;   //update the previous time to compare in the next loop

    }
}
```
*Figure 9: The delay function*

```c
  t0 = MAP_Timer32_getValue(TIMER32_0_BASE);
        delay_ms(timeArray[i]);
        t1 = MAP_Timer32_getValue(TIMER32_0_BASE);

        printf("Expected delay time: %u ms\n",timeArray[i]);
        printf("Actual delay time  : %u us\n", ((t0-t1)/3));
        printf("Time difference  : %u us\n\n", ((t0-t1)/3)- timeArray[i]*1000);
```
*Figure 10: Relevant code to measure time and print the results*
