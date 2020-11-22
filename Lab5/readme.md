# Introduction
The purpose of this lab assignment is to have a better understanding of the GPIO's capacitive-touch caoacity and the Timer_A module on the MSP432 MCU. Specifically, We will use the capacitive-touch capability as an input method to control an LED.

## Exercise 1
The task for this exercise is to print out the state of the oscillator which should print out a stream of random `1’s` and `0’s` as shown in the debug console below.

```
[CORTEX_M4_0] MCLK: 3000000
10110111001111110100111110000100101011101001100010100100011111000000010111001101110100011001101000111010110111110110011001100
```
*Figure 1: Output read from debug console for Exercise 1*

```c
int main(void)
{
    bool state;
    /* Stop Watchdog  */
    MAP_WDT_A_holdTimer();

    MAP_Timer32_initModule(TIMER32_0_BASE, TIMER32_PRESCALER_1, TIMER32_32BIT, TIMER32_FREE_RUN_MODE);
    MAP_Timer32_startTimer(TIMER32_0_BASE, 0);
    printf("MCLK: %u\n", MAP_CS_getMCLK());

    CAPTIO0CTL = 0;
    CAPTIO0CTL |= (1 << 8);    // Enable CAPTIO
    CAPTIO0CTL |= 0b0100 << 4; // Choose Port 4
    CAPTIO0CTL |= 0b0001 << 1; // Choose Pin 1


    while(1)
    {
        state = CAPTIO0CTL & 0x200;
        printf("%d",state);
        fflush(stdout);
        delay_ms(10);
    }
}
```

## Exercise 2
Since the counter ticks is recorded in 2ms, the frequency will be calculated by taking the value read by **MAP_Timer_A_getCounterValue(TIMER_A2_BASE)** divided by `0.002s`. The counter will then be reset at the end and sample every `500ms` as shown in the while loop below. By repeatedly touching or slightly putting your finger close to the `Pin 4.1`, the frequency can slightly change. To make the frequency change drastically, the finger needs to fully contact and hold at the pin for a short period of time. According to the debug console below, the touch is when the frequency is greater than `6000000Hz` and the no touch is when the frequency is much lower range of `1000000Hz` to `2000000Hz`. Calibration at the beginning is necessary to record the frequency at no touch moment. To determine and print out the touch or no touch to the console, we need to calculate the changes percentage compared to the initial calibration value.
```
[CORTEX_M4_0] MCLK: 3000000
Calibrating...Please do not touch the Pin
Calibration finished
No Touch!Frequency: 6048000Hz 
No Touch!Frequency: 6047500Hz 
No Touch!Frequency: 6047000Hz 
No Touch!Frequency: 6047500Hz 
No Touch!Frequency: 6047000Hz 
No Touch!Frequency: 6048000Hz 
No Touch!Frequency: 6047500Hz 
No Touch!Frequency: 6048500Hz 
No Touch!Frequency: 6047500Hz 
No Touch!Frequency: 6047000Hz 
Touch!Frequency: 2365500Hz 
Touch!Frequency: 1997500Hz 
Touch!Frequency: 1846500Hz 
No Touch!Frequency: 6046000Hz 
No Touch!Frequency: 6046000Hz 
Touch!Frequency: 2140000Hz 
Touch!Frequency: 1679500Hz 
Touch!Frequency: 2663000Hz 
```
*Figure 2: Output read from debug console for Exercise 2*
```c
 //calibration
    printf("Calibrating...Please do not touch the Pin\n");
    delay_ms(3000);
    MAP_Timer_A_clearTimer(TIMER_A2_BASE);
    delay_ms(2);
    notouch = (uint32_t)((unsigned long long)MAP_Timer_A_getCounterValue(TIMER_A2_BASE)*1000/2);
    printf("Calibration finished\n");

    while(1)
    {
        MAP_Timer_A_clearTimer(TIMER_A2_BASE);
        delay_ms(2);
        t0 = (uint32_t)((unsigned long long)MAP_Timer_A_getCounterValue(TIMER_A2_BASE)*1000/2);

        if((uint32_t)((unsigned long long)abs(notouch-t0)*100/notouch)<=5){
            printf("No Touch!");
        }else{
            printf("Touch!");
        }
        printf("Frequency: %uHz \n", t0);
        delay_ms(500);
    }
```

## Exercise 3
Similar to previous exercise, we calculated the touch or no touch by measuring the initial calibration and calculating the percentage difference. As shown in the debug console below, about `74%` difference between touch and no touch frequency. 
```
[CORTEX_M4_0] MCLK: 3000000
Calibrating...Please do not touch Pin
Calibration finished
Control percentage difference: 0%
No Touch! Frequency: 6049500Hz 

Control percentage difference: 0%
No Touch! Frequency: 6049000Hz 

Control percentage difference: 0%
No Touch! Frequency: 6050500Hz 

Control percentage difference: 0%
No Touch! Frequency: 6049000Hz 

Control percentage difference: 74%
Touch! Frequency: 1524500Hz 

Control percentage difference: 73%
Touch! Frequency: 1605500Hz 

Control percentage difference: 0%
No Touch! Frequency: 6045000Hz 

Control percentage difference: 0%
No Touch! Frequency: 6049000Hz 

Control percentage difference: 77%
Touch! Frequency: 1360000Hz 
```
*Figure 4: Output read from debug console for Exercise 3*

The control parameters are no-touch and percentage. To control the LED we use **Driverlib** functions as shown below.
```c
    //calibration
    printf("Calibrating...Please do not touch Pin\n");
    delay_ms(3000);
    MAP_Timer_A_clearTimer(TIMER_A2_BASE);
    delay_ms(2);
    notouch = (uint32_t)((unsigned long long)MAP_Timer_A_getCounterValue(TIMER_A2_BASE)*1000/2);
    printf("Calibration finished\n");

    //set up LED
    MAP_GPIO_setAsOutputPin(GPIO_PORT_P1, GPIO_PIN0);     //LED 1 set as output
    GPIO_setOutputLowOnPin(GPIO_PORT_P1, GPIO_PIN0);     //set LED to low

    while(1)
    {
        MAP_Timer_A_clearTimer(TIMER_A2_BASE);
        delay_ms(2);
        t0 = (uint32_t)((unsigned long long)MAP_Timer_A_getCounterValue(TIMER_A2_BASE)*1000/2);
        percentage = (uint32_t)((unsigned long long)abs(notouch-t0)*100/notouch); // control parameter

        printf("Control percentage difference: %u%%\n", percentage);
        if(percentage<=5){
            printf("No Touch! ");
            GPIO_setOutputLowOnPin(GPIO_PORT_P1, GPIO_PIN0);     //set LED to low
        }else{
            printf("Touch! ");
            GPIO_setOutputHighOnPin(GPIO_PORT_P1, GPIO_PIN0);     //set LED to high
        }
        printf("Frequency: %uHz \n\n", t0);
        delay_ms(500);

    }
```

## Exercise 4
The port changing from `Pin 4.1` to `3.4` for this task can be performed using `CAPTIO0CTL` register. According to the output console, the no touch frequency for `Pin 3.4` is more than double the frequency of `Pin 4.1`. The reason is due to the difference of `RC` circuit between the Pin. According to the **MSP432 schematic**, `Pin 4.1` is connected to connector `J1` while `Pin 3.4` is not connected to anything outside of the chip. Therefore, `Pin 3.4` does not connect to the capacitor which will make it charge and discharge faster than `4.1` which takes time for the capacitor to charge up and discharge. At the result, the no-touch frequency at `Pin 3.4` is much faster than `Pin 4.1`.
```c
CAPTIO0CTL = 0;
    CAPTIO0CTL |= (1 << 8);    // Enable CAPTIO
    CAPTIO0CTL |= 0b0011 << 4; // Choose Port 3
    CAPTIO0CTL |= 0b0100 << 1; // Choose Pin 4
```
*Figure 5: Relevant code to select Pin 3.4*

```c
[CORTEX_M4_0] MCLK: 3000000
TFrequency: 12957500Hz 
TFrequency: 12917500Hz 
TFrequency: 12917500Hz 
TFrequency: 12916000Hz 
TFrequency: 12919000Hz 
TFrequency: 12914000Hz 
```
*Figure 6: Output read from debug console for Exercise 4*
