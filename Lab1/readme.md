# Introduction
  The purpose of this lab assignment is to get familiarized with the MSP432P401R board and its integrated development environment. Specifically, the task is to install the TI SDK and use its provided device driver function to compose and debug on the board.
  
## Exercise 1
  After going through the instructions for this task, there were no issues occured when installing the CCS IDE on Mac.

![TI folder showing the CCS v10  ](https://lh6.googleusercontent.com/HS3AwqtEGeFPTX1tJqvwsKNWo2gpXCFplbidJxq6VenGWlBgjsGuTgWZkuJmtI0R8wcZZGF8X_3hqNn9gTVHtGhfml2FNmKOn1WRwYA4mfXhuP8J0ttpAoqTDEIOniJykMSv3J9w)
*Figure 1: TI folder showing the CCS v10*  

## Exercise 2
The task for this excercise is to download the driver package and able to compile a sameple `hello world` program. When attempting to download MSP432P4 SDK to the computer, the package did not appear on the folder even after waiting for an hour. The solution for this is to have several attempts to restart the application and press the install button again. If it is successful, the TI folder does not contain the SDK; however, it still appears inside CSS as shown in *Figure 2* below.

```c
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>
/* Standard Includes */
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
int main(void)
{
    printf("Hello World!!!\n");
}
```

![TI folder showing the CCS v10  ](https://lh4.googleusercontent.com/J5aJZPhvzmtx67S-rL_MesyDzhB47pwLMcormBOgNPGg8zgjgWljtOrJlmP5bH-3yAkppF4MLQ1CmyfU96w2rQP2qC_WB0xQ9S9BbLWetrCGPmkzTF9TU-TXvxyN8BShL1Wae5CZ)

*Figure 2: SDK folder appears in CCS*

## Exercise 3
The task for this excercise is to import a small example project to blink an LED. TI provides many example projects for various microcontrollers. Go Resource Explorer Offline. On the left pane, expand the tree structure from Software to **<Software><SimpleLink MSP432P4 SDK ...><Examples><Development tools><MSP432P401R LaunchPad - Red...><DriverLib><gpio_toggle_output><No RTOS><CCS Compiler><gpio_toggle_output>**. Import the project the same way in the previous exercise. Build and execute the program.

Luckily, no issue occurs in this task.


```c
/* DriverLib Includes */
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>
/* Standard Includes */
#include <stdint.h>
#include <stdbool.h>
//![Simple GPIO Config]
int main(void)
{
    volatile uint32_t ii;
    /* Halting the Watchdog */
    MAP_WDT_A_holdTimer();
    /* Configuring P1.0 as output */
    MAP_GPIO_setAsOutputPin(GPIO_PORT_P1, GPIO_PIN0);
    while (1)
    {
        /* Delay Loop */
        for(ii=0;ii<50000;ii++)
        {
        }
        MAP_GPIO_toggleOutputOnPin(GPIO_PORT_P1, GPIO_PIN0);
    }
}
//![Simple GPIO Config]

```

![TI folder showing the CCS v10  ](https://lh5.googleusercontent.com/0SX6zHP1jiInUE-Fi6aJ8dyQ1Pf2-r3UP7DATiY57-eI426hCiOsMtJ2EUsdisuEoWo03c1awKjJSlREv2uVh8Inoq4iWuKSK_a8GYWTBVuOIgXcb0GPTMda0cgSTxLdacIPE1uP)
*Figure 3: The red LED on the right was blinking slower when changing the index range from 5000 to 50000*

## Exercise 4
For this excercise, the task is to find out some device information about the MCU. There is some specific information about the MCU that is stored in the device’s non-volatile memory. They are contained in a table called **Device Descriptor Table**. Consult the MCU datasheet for its address. Create a new project (by importing the example empty project) to read and print the information to the debug console. Just print out the information from the top 3 blocks of the table, i.e., up to the Die Record block (the 3rdblock). There should be 17 data items. Note that each data item is stored as a 32-bit word. 

This task gave a little struggle with the pointer type as well as the printing format that I need to take a good amount of time to review. Overall, this exercise was successful as shown in *Figure 4* below.

```c
//* DriverLib Includes */
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
```

```
[CORTEX_M4_0]      Description                   Address                 Value
                   TLV checksum                   00201000h               2CE512F2h
                   Device Info Tag                00201004h               0000000Bh
                   Device Info Length             00201008h               00000004h
                   Device ID                      0020100Ch               0000A000h
                   Hardware Revision              00201010h               00000044h
                   Boot-Code Revision             00201014h               00420044h
                   ROM Driver Library Revision    00201018h               03000010h
                   Die Record Tag                 0020101Ch               0000000Ch
                   Die Record Length              00201020h               00000008h
                   Die X Position                 00201024h               0000002Dh
                   Die Y Position                 00201028h               00000024h
                   Wafer ID                       0020102Ch               00000016h
                   Lot ID                         00201030h               00011C67h
                   Reserved                       00201034h               FFFFFC42h
                   Reserved                       00201038h               00000000h
                   Reserved                       0020103Ch               00000000h
                   Test Results                   00201040h               00474345h
```

_Figure 4: The code was able to print to console 17 datas from the register._
