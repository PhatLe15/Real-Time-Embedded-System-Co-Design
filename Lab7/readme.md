# Introduction
The purpose of this lab assignment is to have a better understanding of the **TI-RTOS** by creating `tasks` and the TI driver to perform `UART` communication with external terminal programs.

## Exercise 1
For this exercise, the job is to create two separate tasks to control `2` LEDs at different blinking rates. Specifically, the red LED(`P2.0`) will blink at 1Hz and the green LED(`P2.1`) will blink at `0.25Hz`. 
```c
Void task1(UArg arg0, UArg arg1)
{
    MAP_GPIO_setAsOutputPin(GPIO_PORT_P2, GPIO_PIN0);     //RED   P2.0 as output
    GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN0);     //RED set to low
    while(1){
        Task_sleep(500);
        GPIO_setOutputHighOnPin(GPIO_PORT_P2, GPIO_PIN0);
        Task_sleep(500);
        GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN0);
    }

}

Void task2(UArg arg0, UArg arg1)
{
    MAP_GPIO_setAsOutputPin(GPIO_PORT_P2, GPIO_PIN1);     //GREEN  P2.1 as output
    GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN1);     //GREEN set to low
    while(1){
        Task_sleep(2000);
        GPIO_setOutputHighOnPin(GPIO_PORT_P2, GPIO_PIN1);
        Task_sleep(2000);
        GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN1);
    }
}
```
To find the delay blinking time, we invert the frequency into time(`T = 1/f`). According to the Task1 and Task2 function above, `1Hz` and `0.25Hz` can be converted to `1000ms(1/1Hz)` and `4000ms(1/0.25)` per blink. Since a blink time is defined as the total of an `ON` time and an `OFF` time, the delay value per `ON` and `OFF` is `500` and `2000`.

## Exercise 2.1
For this exercise, the task is to read and display the state of button S1(`P1.1`) on `Task1` function. According to the terminal window below, the pressed message is `‘S10’` and the not pressed message is `‘S11’`.


![sadf](https://lh6.googleusercontent.com/mbkJZspsfw-n3B8KpWpSE28S55mIMqpUiF5RajoOhIs9gccsqg3yDPxBut1dt5lITD9z3Dj7ddQL07aAx4LAhhoueQ3Kcu593LsdfOYYkbPLWFHQ5mXJaO3ZuH0VX0g0TeizXEyJ)
*Figure 1: Output read from terminal window*

```c
Void task1(UArg arg0, UArg arg1)
{
    //GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P1, GPIO_PIN4); //S2
    GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P1, GPIO_PIN1); //S1
    printf("Task1\n");
    UART_Handle uart;
    UART_Params uartParams;
    UART_init();    // Driver init
    // Set up communication parameters and open the device
    UART_Params_init(&uartParams);
    uartParams.readEcho = UART_ECHO_OFF;
    uart = UART_open(CONFIG_UART_0, &uartParams);
    if (uart == NULL) {
        printf("Failed to open UART.\n");
        while (1);
    }
    unsigned int count = 0;
    char buffer[10];
    while (1) {
        //GPIO_getInputPinValue(GPIO_PORT_P1, GPIO_PIN4);//S2
        uint8_t S1 = GPIO_getInputPinValue(GPIO_PORT_P1, GPIO_PIN1);//S1
        sprintf(buffer, "S1%d\n",S1);
        UART_write(uart, buffer, strlen(buffer));
        Task_sleep(100);
    }
}
```

## Exercise 2.2
  For this task, we will try to save the communication bandwidth by only sending the message to the terminal only when a different message is updated. In addition, if nothing is updated within `5s`, we will print a message with no state change.
  
  
  ![sad](https://lh6.googleusercontent.com/H0DzwHMQkTdl9RgfqSlrPTsBeiPWoo8CrgnHaSR9bVQaHUSQ7b2E5qcHFkc6m7N7iFYhqO7ZKVxEX2oVnthXejZf7eg5IYmfDjBc6rI0ok02N9WpTTOdEpXXkZ3KwxFS2KhGgrnR)
  *Figure 2: Output read from terminal window*
  
```c
Void task1(UArg arg0, UArg arg1)
{
    GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P1, GPIO_PIN1); //S1
    printf("Task1\n");
    char buffer[50];
    unsigned int total = 0;
    uint8_t previous = 2; //make sure the first one are different
    while (1) {
        uint8_t current = GPIO_getInputPinValue(GPIO_PORT_P1, GPIO_PIN1);//S1
        if(current == previous){//if nothing change
            total = total + 100;
            if(total >= 5000){ //if nothing change within 5s
                sprintf(buffer, "S1 has not update in 5s\n");
                UART_write(uart, buffer, strlen(buffer));
                total = 0; //reset the counter
            }
        }else{//if the value update
            sprintf(buffer, "S1%d\n",current);
            UART_write(uart, buffer, strlen(buffer));
            total = 0;
        }
        previous = current;
        Task_sleep(100);
    }
}  
```

## Exercise 2.3
 Similar to the above exercise, we will control button S2(`P1.4`) on `Task2` function which sends pressed messages as `‘S20’`, not pressed messages as `‘S21’`, and no state change message after 5 second. However, the `UART` initialization functions can only call once which have already been created in the Task1 function. To handle the issue, we bring all necessary `UART` initialization functions to main as well as bringing variables such as `UART_handle` and `UART_param` to global so both tasks can share the communication line.  
 
 
![asdsa](https://lh6.googleusercontent.com/VMbE76kFzxyPe3OLApa2BviVLNIiTxl4oE9P9hY0KgQCWMKJ16SC7m5nqHieAxkvaf6qWpdfhhdy9loeLj9vjbRaaVjYSyZM3dkiPGNGDfaOpGNcUTlsmbHOVwvEbXz5_XXPLsbr)
*Figure 6: Output read from Terminal Window*

According to *Figure 6* above, both tasks have been successfully compiled and sent the similar message to the same `Terminal Window`.

```c
Void task1(UArg arg0, UArg arg1)
{
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
    printf("Task2\n");
    char buffer[50];
    unsigned int total = 0;
    uint8_t previous = 2; //make sure the first one are different
    while (1) {
        uint8_t current = GPIO_getInputPinValue(GPIO_PORT_P1, GPIO_PIN4);//S2
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
```

## Exercise 2.3
For this last task, we will create another `Task3` function to control the blue LED(`P2.2`) using the input keyboard to send control messages from **Terminal Window** to the **LaunchPad**. Specifically, we can send a `‘1’` message to the terminal to turn on the LED and `‘0’` to turn it off. To receive the signal from the terminal, **UART_read()** function is used with a single character variable as the buffer to pass the message as shown below.


```c
Void task3(UArg arg0, UArg arg1)
{
    MAP_GPIO_setAsOutputPin(GPIO_PORT_P2, GPIO_PIN2);     //BLUE   P2.2 as output
    GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN2);     //BLUE set to low
    printf("Task3\n");
    char a;
    while (1) {
        UART_read(uart,&a,1);
        if(a == '0'){//if nothing change
            GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN2);     //BLUE set to low
        }else if (a == '1'){//if the value update
            GPIO_setOutputHighOnPin(GPIO_PORT_P2, GPIO_PIN2);     //BLUE set to low
        }
        Task_sleep(100);
    }
}
```

