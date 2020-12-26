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

#define TASKSTACKSIZE   2048



Void task1(UArg arg0, UArg arg1);
//Void task2(UArg arg0, UArg arg1);

Void taski(UArg arg0, UArg arg1);

//Task_Struct task1Struct, task2Struct;
//Char task1Stack[TASKSTACKSIZE], task2Stack[TASKSTACKSIZE];


#define NTASKS 4
Task_Struct taskStructs[NTASKS]; //create 4 tasks structure
Char taskStacks[NTASKS][TASKSTACKSIZE]; //create 4 stack for each task

Semaphore_Struct semaStruct;
Semaphore_Handle semaphoreHandle = NULL;

int upper = 2500; //0.2Hz
int lower = 500; //1Hz


int main()
{
    // Init drivers
    Board_init();

    srand(time(NULL));      // Set seed for random number generator

    // Construct tasks
    Task_Params taskParams;
    Task_Params_init(&taskParams);
    taskParams.stackSize = TASKSTACKSIZE;

    // Task RED
    taskParams.stack = &taskStacks[0];
    taskParams.arg0 = 0;
    taskParams.arg1 = 1;
    Task_construct(&taskStructs[0], (Task_FuncPtr)taski, &taskParams, NULL);

    // Task GREEN
    taskParams.stack = &taskStacks[1];
    taskParams.arg0 = 1;
    taskParams.arg1 = 0;
    Task_construct(&taskStructs[1], (Task_FuncPtr)taski, &taskParams, NULL);

    // Task BLUE
    taskParams.stack = &taskStacks[2];
    taskParams.arg0 = 1;
    taskParams.arg1 = 1;
    Task_construct(&taskStructs[2], (Task_FuncPtr)taski, &taskParams, NULL);

    // Task 1
    taskParams.stack = &taskStacks[3];
    Task_construct(&taskStructs[3], (Task_FuncPtr)task1, &taskParams, NULL);

    // Construct a semaphore object
    Semaphore_Params semaParams;
    Semaphore_Params_init(&semaParams);                 // Initialize structure with default parameters
    Semaphore_construct(&semaStruct, 0, &semaParams);   // Create an instance of semaphore object
    semaphoreHandle = Semaphore_handle(&semaStruct);

    BIOS_start();    // Jump to the OS and won't return
    return(0);
}

Void task1(UArg arg0, UArg arg1)
{

    while(1){
        int num = (rand()% (upper - lower + 1)) + lower; // random number generate in range
        printf("Random No.: %d\n", num);
        Task_sleep(num);
        Semaphore_post(semaphoreHandle); //+1
        Semaphore_post(semaphoreHandle); //+1
        Semaphore_post(semaphoreHandle); //+1
    }
}

Void taski(UArg arg0, UArg arg1){
    uint_fast8_t  port;
    uint_fast16_t pin;
    if(arg0 == 0 && arg1 == 1){ //RED
        port = GPIO_PORT_P1;
        pin = GPIO_PIN0;
    }else if(arg0 == 1 && arg1 == 0){//GREEN
        port = GPIO_PORT_P2;
        pin = GPIO_PIN1;
    }else if(arg0 == 1 && arg1 == 1){ //BLUE
        port = GPIO_PORT_P2;
        pin = GPIO_PIN2;
    }

    MAP_GPIO_setAsOutputPin(port, pin);     //RED   P1.0 as output
    GPIO_setOutputLowOnPin(port, pin);     //RED set to low

    while(1){
        //printf("Task: arg0=%u, arg1=%u\n", (uint32_t)arg0, (uint32_t)arg1);

        Semaphore_pend(semaphoreHandle, BIOS_WAIT_FOREVER); //-1

        while(Semaphore_getCount(semaphoreHandle) != 0){ // waiting for other task
            Task_yield();
        }

        MAP_GPIO_toggleOutputOnPin(port, pin);
    }
}

//Void task2(UArg arg0, UArg arg1)
//{
//
//    MAP_GPIO_setAsOutputPin(GPIO_PORT_P1, GPIO_PIN0);     //RED   P1.0 as output
//    GPIO_setOutputLowOnPin(GPIO_PORT_P1, GPIO_PIN0);     //RED set to low
//
//    while(1){
//        printf("Task2\n");
//        Semaphore_pend(semaphoreHandle, BIOS_WAIT_FOREVER);
//        MAP_GPIO_toggleOutputOnPin(GPIO_PORT_P1, GPIO_PIN0);
//    }
//
//
//}
