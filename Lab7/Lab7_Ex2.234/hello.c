#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

/* XDC module Headers */
#include <xdc/std.h>
#include <xdc/runtime/System.h>

/* BIOS module Headers */
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Clock.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/knl/Semaphore.h>

#include <ti/drivers/Board.h>
#define __MSP432P4XX__
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>

#include <ti/drivers/UART.h>
#include "ti_drivers_config.h"

#define TASKSTACKSIZE   2048

Void task1(UArg arg0, UArg arg1);
Void task2(UArg arg0, UArg arg1);
Void task3(UArg arg0, UArg arg1);

Task_Struct task1Struct, task2Struct, task3Struct;
Char task1Stack[TASKSTACKSIZE], task2Stack[TASKSTACKSIZE], task3Stack[TASKSTACKSIZE];

UART_Handle uart;
UART_Params uartParams;

int main()
{
    /* Construct BIOS objects */
    Task_Params taskParams;

    /* Call driver init functions */
    Board_init();


    UART_init();    // Driver init

    // Set up communication parameters and open the device
    UART_Params_init(&uartParams);
    uartParams.readEcho = UART_ECHO_OFF;
    uart = UART_open(CONFIG_UART_0, &uartParams);

    if (uart == NULL) {
        printf("Failed to open UART.\n");
        while (1);
    }

    /* Construct task threads */
    Task_Params_init(&taskParams);
    taskParams.stackSize = TASKSTACKSIZE;
    taskParams.stack = &task1Stack;
    Task_construct(&task1Struct, (Task_FuncPtr)task1, &taskParams, NULL);

    taskParams.stack = &task2Stack;
    Task_construct(&task2Struct, (Task_FuncPtr)task2, &taskParams, NULL);

    taskParams.stack = &task3Stack;
    Task_construct(&task3Struct, (Task_FuncPtr)task3, &taskParams, NULL);

    BIOS_start();    /* Does not return */
    return(0);
}

Void task1(UArg arg0, UArg arg1)
{
    //GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P1, GPIO_PIN4); //S2
    GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P1, GPIO_PIN1); //S1

    printf("Task1\n");

    char buffer[50];
    unsigned int total = 0;
    uint8_t previous = 2; //make sure the first one are different

    while (1) {
        //GPIO_getInputPinValue(GPIO_PORT_P1, GPIO_PIN4);//S2
        uint8_t current = GPIO_getInputPinValue(GPIO_PORT_P1, GPIO_PIN1);//S1
        if(current == previous){//if nothing change
            total = total + 100;
            if(total >= 5000){ //if nothing change within 5s
                sprintf(buffer, "S1 has not update in 5s\n");
                UART_write(uart, buffer, strlen(buffer));
                total = 0; //reset the counter
            }

        }else if(current != previous){//if the value update
            sprintf(buffer, "S1%d\n",current);
            UART_write(uart, buffer, strlen(buffer));
            total = 0;
        }

        previous = current;
        Task_sleep(100);
    }
}

Void task2(UArg arg0, UArg arg1)
{
    GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P1, GPIO_PIN4); //S2
    //GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P1, GPIO_PIN1); //S1

    printf("Task2\n");


    char buffer[50];
    unsigned int total = 0;
    uint8_t previous = 2; //make sure the first one are different

    while (1) {
        uint8_t current = GPIO_getInputPinValue(GPIO_PORT_P1, GPIO_PIN4);//S2
        //uint8_t current = GPIO_getInputPinValue(GPIO_PORT_P1, GPIO_PIN1);//S1
        if(current == previous){//if nothing change
            total = total + 100;
            if(total >= 5000){ //if nothing change within 5s
                sprintf(buffer, "S2 has not update in 5s\n");
                UART_write(uart, buffer, strlen(buffer));
                total = 0; //reset the counter
            }

        }else if(current != previous){//if the value update
            sprintf(buffer, "S2%d\n",current);
            UART_write(uart, buffer, strlen(buffer));
            total = 0;
        }

        previous = current;
        Task_sleep(100);
    }
}

Void task3(UArg arg0, UArg arg1)
{
    MAP_GPIO_setAsOutputPin(GPIO_PORT_P2, GPIO_PIN2);     //BLUE   P2.2 as output
    GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN2);     //BLUE set to low

    printf("Task3\n");


    char a;
    //unsigned int total = 0;
    //uint8_t previous = 2; //make sure the first one are different

    while (1) {
        UART_read(uart,&a,1);
        //printf("%d\n",a);
        //uint32_t current = GPIO_getInputPinValue(GPIO_PORT_P1, GPIO_PIN1);//S1
        if(a == '0'){//if nothing change
            GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN2);     //BLUE set to low

        }else if (a == '1'){//if the value update
            GPIO_setOutputHighOnPin(GPIO_PORT_P2, GPIO_PIN2);     //BLUE set to low
        }

        //previous = current;
        Task_sleep(100);
    }
}

