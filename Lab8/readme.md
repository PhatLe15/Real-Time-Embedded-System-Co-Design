# Introduction
The purpose of this lab assignment is to have a better understanding of the semaphore system object in the TI-RTOS by creating tasks to control LED.

## Exercise 1
For this exercise, the job is to create two separate tasks to control the red LED using semaphore. Specifically, Task 1 will control the delay time at 0.4 Hz and Task 2 will initialize and toggle the LED.

```c
Void task1(UArg arg0, UArg arg1)
{

    while(1){
        printf("Task1\n");
        Task_sleep(1250);
        Semaphore_post(semaphoreHandle);
    }
}

Void task2(UArg arg0, UArg arg1)
{

    MAP_GPIO_setAsOutputPin(GPIO_PORT_P1, GPIO_PIN0);     //RED   P1.0 as output
    GPIO_setOutputLowOnPin(GPIO_PORT_P1, GPIO_PIN0);     //RED set to low

    while(1){
        printf("Task2\n");
        Semaphore_pend(semaphoreHandle, BIOS_WAIT_FOREVER);
        MAP_GPIO_toggleOutputOnPin(GPIO_PORT_P1, GPIO_PIN0);
    }
}

```
According to the code snippet above, task 1 function will increment the semaphore count value after it finishes the delay at 0.4Hz(2500ms) by calling Semaphore_post(). In this case, the delay time will be 1250ms per ON and OFF (period/2). At the same time, Task 2 function will wait and receive the increment signal using the Semaphore_pend(). Then, it will decrement the signal and continue executing the next instructions. 

## Exercise 2
For this exercise, the task is to create 3 tasks that use the same function implementation from the given parameters below. Furthermore, each task will have to pass different arguments to differentiate the output.

```c
#define NTASKS 3
Task_Struct
taskStructs[NTASKS];
Char taskStacks[NTASKS][TASKSTACKSIZE];

```

```
[CORTEX_M4_0] Task: arg0=0, arg1=1
Task: arg0=1, arg1=0
Task: arg0=1, arg1=1
Task: arg0=0, arg1=1
Task: arg0=1, arg1=0
Task: arg0=1, arg1=1
Task: arg0=0, arg1=1
Task: arg0=1, arg1=0
Task: arg0=1, arg1=1
Task: arg0=0, arg1=1
Task: arg0=1, arg1=0
Task: arg0=1, arg1=1
Task: arg0=0, arg1=1
Task: arg0=1, arg1=0
Task: arg0=1, arg1=1
Task: arg0=0, arg1=1
Task: arg0=1, arg1=0
Task: arg0=1, arg1=1
Task: arg0=0, arg1=1
Task: arg0=1, arg1=0
Task: arg0=1, arg1=1
Task: arg0=0, arg1=1
Task: arg0=1, arg1=0
Task: arg0=1, arg1=1
Task: arg0=0, arg1=1
Task: arg0=1, arg1=0
```
*Figure 1: Output read from console*

As shown in Figure 1 above, each task has been successfully created and periodically output its own arguments as indicated in the code snippet below.

```c
  // Task 1
    taskParams.stack = &taskStacks[0];
    taskParams.arg0 = 0;
    taskParams.arg1 = 1;
    Task_construct(&taskStructs[0], (Task_FuncPtr)taski, &taskParams, NULL);

    // Task 2
    taskParams.stack = &taskStacks[1];
    taskParams.arg0 = 1;
    taskParams.arg1 = 0;
    Task_construct(&taskStructs[1], (Task_FuncPtr)taski, &taskParams, NULL);

    // Task 3
    taskParams.stack = &taskStacks[2];
    taskParams.arg0 = 1;
    taskParams.arg1 = 1;
    Task_construct(&taskStructs[2], (Task_FuncPtr)taski, &taskParams, NULL);
```

## Exercise 3
Similar to exercise 1, this exercise will control 3 LEDs red(P1.0), green(2.1) and blue(2.2). Furthermore, all the LED will have to blink synchronously at a random rate ranging from 0.2Hz to 1Hz(1000ms to 5000ms). Therefore, the delay per ON and OFF period should range from 500ms to 2500ms. As shown in the code snippet below, the common LED control task function is Taski and the timing task function is called task1. To make sure each task is running in synchronized, I used semaphore to increment 3 times using Semaphore_post() in task 1. Then, taski will decrement one time and wait for others taski to decrement until the semaphore count value has reached 0. We can obtain the count value using Semaphore_getCount() and can pause the current task using Task_yield() as shown below. 

```c
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

```

