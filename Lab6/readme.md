# Introduction
The purpose of this lab assignment is to have a better understanding of the power consumption of the MCU using **EnergyTrace** tool. In addition, the lab also introduced the use of ADC and the temperature sensor of the MCU.

## Exercise 1.1
The task for this exercise is to obtain the measurement of energy and the mean current of the **software method** using **EnergyTrace** tool. The `3` different measurements for this method are described below.

*Table 1: Output read from EnergyTrace tool for Exercise 1.1*
Measurement No.|Energy(mJ)|Mean current(mA)
:---:|:---:|:---:
1|145.999|1.0733
2|138.504|1.0229
3|138.976|1.0246

## Exercise 1.2
The task for this exercise is to obtain the measurement of energy and the mean current of the **hardware method** using **EnergyTrace** tool. The 3 different measurements for this method are described below.

*Table 2: Output read from EnergyTrace tool for Exercise 1.2*
Measurement No.|Energy(mJ)|Mean current(mA)
:---:|:---:|:---:
1|18.395|0.9396
2|18.8|0.9422
3|18.996|0.9409

## Exercise 1.3
The task for this exercise is to obtain the measurement of energy and mean current of the **DMA method** using **EnergyTrace** tool. The 3 different measurements for this method are described below.
Measurement No.|Energy(mJ)|Mean current(mA)
:---:|:---:|:---:
1|1.383|1.0171
2|1.356|1.0135
3|1.282|1.0147

## Exercise 1.4
The task for this exercise is to obtain the measurement of energy and mean current of the **DMA with the low power mode method** using **EnergyTrace tool**. The 3 different measurements for this method are described below.
*Table 4: Output read from EnergyTrace tool for Exercise 1.4*
Measurement No.|Energy(mJ)|Mean current(mA)
:---:|:---:|:---:
1|1.247|0.7495
2|0.949|0.7473
3|1.197|0.7494

## Exercise 1.5
The task for this exercise is to display all the measurements from all the methods above to compute the averages for each method. Then, we compare the measurement from the energy and mean current bar graph as shown below. 

*Table 5: Output read from EnergyTrace tool for Exercise 1.5*
Method|Measurement No.|Energy(mJ)|Mean current(mA)|Avg. Energy(mJ)|Avg. Current(mA)
:---:|:---:|:---:|:---:|:---:|:---:|
SW|1|145.999|1.0733|141.1596667|1.040266667
||2|138.504|1.0229|
||3|138.976|1.0246
HW|1|18.395|0.9396|18.73033333|0.9409
||2|18.8|0.9422
||3|18.996|0.9409
DMA|1|1.383|1.0171|1.340333333|1.0151
||2|1.356|1.0135
||3|1.282|1.0147
DMA LPM|1|1.247|0.7495|1.131|0.7487333333
||2|0.949|0.7473
||3|1.197|0.7494

![asdf](https://lh4.googleusercontent.com/iyewGdkWeYPNoss30Xwu4h7YkXUF5efRUJvRgEf4Kdefq7hHhanbqVLa6GHc2fGfrlKJDF0NZJYv11G3PsmTAK7Ko2hkxYh8B8AVdThCZEV9P4gCOJO3XbzcrjFskmeKlE8i_Rvx)
*Figure 1: Average energy comparison chart*

![asdf](https://lh3.googleusercontent.com/IHaX7tOk45PzEDyYjD0l9pw41qpIFVHoJwxMChV9vWqDD2uK-MVzSgoQOyFp7KLYzQN8-5cPviLppNyHLnkd8iuC7q8gmJH2WngoTEP2aG8_9I8bdEJkTJp4WLN5WIXeOq35yn0Z)
*Figure 2: Average mean current chart*

According to *Figure 1* and *Figure 2* above, the energy consumption is significantly reduced due to the efficiency of using hardware accelerators instead of using the processor. Having the `accelerators` means less time to perform tasks, therefore, the energy is lower when using the `DMA` transfer and slightly lower for `DMA low power mode` since the process is efficiently managed and took less time to compute. In terms of mean current, the highest is still the software method because the processor is used for the entire process. For `hardware methods`, using the DMA will result in higher current since extra circuitry is used such as when both the `DMA accelerator` and the `processor` is used. Since the `DMA LPM` has the processor to sleep when not needed, less circuitry is used so the mean current is reduced for this method.

## Exercise 2.1
The task for this exercise is to sample the digitized value given from `ADC` and convert it to voltage by multiplying with the calculated resolution(`2.5V/16384`).  
```
[CORTEX_M4_0] MCLK: 3000000
Digitized value: 4758, Corresponding voltage: 726 mV
Digitized value: 4759, Corresponding voltage: 726 mV
Digitized value: 4766, Corresponding voltage: 727 mV
Digitized value: 4759, Corresponding voltage: 726 mV
Digitized value: 4764, Corresponding voltage: 726 mV
Digitized value: 4756, Corresponding voltage: 725 mV
Digitized value: 4758, Corresponding voltage: 726 mV
Digitized value: 4760, Corresponding voltage: 726 mV
Digitized value: 4750, Corresponding voltage: 724 mV
Digitized value: 4754, Corresponding voltage: 725 mV
Digitized value: 4766, Corresponding voltage: 727 mV
Digitized value: 4763, Corresponding voltage: 726 mV
```
*Figure 4: Output read from debug console for Exercise 2.1*

```c
while(1){
        // Trigger conversion with software
        adc_done = false;
        ADC14_toggleConversionTrigger();
        while (!adc_done)
        {
        }
        uint32_t adc_value = ADC14_getResult(ADC_MEM0);
        //print out the digitized value and calculate the voltage in mV based on the resolution = 2.5v/16384
        printf("Digitized value: %u, Corresponding voltage: %u mV\n", adc_value, ((long long)(adc_value * 2500)/ 16384));
        delay_ms(500);
    }
```

## Exercise 2.2
Similar to previous exercise, we measured the digitized value and converted it to voltage. In addition, we converted the voltage to actual temperature in degree Celsius and degree Fahrenheit based on the reference digitized value at `30` and `85` degree **Celsius** stored in the registers of the Device Descriptor(`0x002010EC` and `0x002010F0`). According to *Figure 4* below, the temperature slowly increases when the MCU is `touched` and slowly decreases when there is `no touch` after that.
**`Initially No Touched`**
```
[CORTEX_M4_0] MCLK: 3000000
Digitized value: 4751, Corresponding voltage: 724.95 mV, Temperature: 24.89 Degree Celsius, 76.81 Degree Fahrenheit
Digitized value: 4744, Corresponding voltage: 723.88 mV, Temperature: 24.35 Degree Celsius, 75.84 Degree Fahrenheit
Digitized value: 4743, Corresponding voltage: 723.72 mV, Temperature: 24.28 Degree Celsius, 75.70 Degree Fahrenheit
Digitized value: 4736, Corresponding voltage: 722.66 mV, Temperature: 23.73 Degree Celsius, 74.72 Degree Fahrenheit
Digitized value: 4742, Corresponding voltage: 723.57 mV, Temperature: 24.20 Degree Celsius, 75.56 Degree Fahrenheit
Digitized value: 4742, Corresponding voltage: 723.57 mV, Temperature: 24.20 Degree Celsius, 75.56 Degree Fahrenheit
Digitized value: 4745, Corresponding voltage: 724.03 mV, Temperature: 24.43 Degree Celsius, 75.97 Degree Fahrenheit
Digitized value: 4746, Corresponding voltage: 724.18 mV, Temperature: 24.51 Degree Celsius, 76.11 Degree Fahrenheit
Digitized value: 4745, Corresponding voltage: 724.03 mV, Temperature: 24.43 Degree Celsius, 75.97 Degree Fahrenheit
Digitized value: 4749, Corresponding voltage: 724.64 mV, Temperature: 24.74 Degree Celsius, 76.53 Degree Fahrenheit
Digitized value: 4745, Corresponding voltage: 724.03 mV, Temperature: 24.43 Degree Celsius, 75.97 Degree Fahrenheit
Digitized value: 4744, Corresponding voltage: 723.88 mV, Temperature: 24.35 Degree Celsius, 75.84 Degree Fahrenheit
Digitized value: 4742, Corresponding voltage: 723.57 mV, Temperature: 24.20 Degree Celsius, 75.56 Degree Fahrenheit
```
**`Touched`**
```
Digitized value: 4754, Corresponding voltage: 725.40 mV, Temperature: 25.13 Degree Celsius, 77.23 Degree Fahrenheit
Digitized value: 4757, Corresponding voltage: 725.86 mV, Temperature: 25.36 Degree Celsius, 77.65 Degree Fahrenheit
Digitized value: 4765, Corresponding voltage: 727.08 mV, Temperature: 25.98 Degree Celsius, 78.76 Degree Fahrenheit
Digitized value: 4766, Corresponding voltage: 727.23 mV, Temperature: 26.05 Degree Celsius, 78.90 Degree Fahrenheit
Digitized value: 4767, Corresponding voltage: 727.39 mV, Temperature: 26.13 Degree Celsius, 79.04 Degree Fahrenheit
Digitized value: 4770, Corresponding voltage: 727.84 mV, Temperature: 26.36 Degree Celsius, 79.46 Degree Fahrenheit
Digitized value: 4779, Corresponding voltage: 729.22 mV, Temperature: 27.06 Degree Celsius, 80.71 Degree Fahrenheit
Digitized value: 4769, Corresponding voltage: 727.69 mV, Temperature: 26.29 Degree Celsius, 79.32 Degree Fahrenheit
Digitized value: 4778, Corresponding voltage: 729.06 mV, Temperature: 26.98 Degree Celsius, 80.57 Degree Fahrenheit
Digitized value: 4775, Corresponding voltage: 728.61 mV, Temperature: 26.75 Degree Celsius, 80.15 Degree Fahrenheit
Digitized value: 4771, Corresponding voltage: 728.00 mV, Temperature: 26.44 Degree Celsius, 79.59 Degree Fahrenheit
```
**`No Touched`**
```
Digitized value: 4766, Corresponding voltage: 727.23 mV, Temperature: 26.05 Degree Celsius, 78.90 Degree Fahrenheit
Digitized value: 4772, Corresponding voltage: 728.15 mV, Temperature: 26.52 Degree Celsius, 79.73 Degree Fahrenheit
Digitized value: 4772, Corresponding voltage: 728.15 mV, Temperature: 26.52 Degree Celsius, 79.73 Degree Fahrenheit
Digitized value: 4769, Corresponding voltage: 727.69 mV, Temperature: 26.29 Degree Celsius, 79.32 Degree Fahrenheit
Digitized value: 4761, Corresponding voltage: 726.47 mV, Temperature: 25.67 Degree Celsius, 78.20 Degree Fahrenheit
Digitized value: 4764, Corresponding voltage: 726.93 mV, Temperature: 25.90 Degree Celsius, 78.62 Degree Fahrenheit
Digitized value: 4760, Corresponding voltage: 726.32 mV, Temperature: 25.59 Degree Celsius, 78.06 Degree Fahrenheit
Digitized value: 4764, Corresponding voltage: 726.93 mV, Temperature: 25.90 Degree Celsius, 78.62 Degree Fahrenheit
Digitized value: 4760, Corresponding voltage: 726.32 mV, Temperature: 25.59 Degree Celsius, 78.06 Degree Fahrenheit
Digitized value: 4761, Corresponding voltage: 726.47 mV, Temperature: 25.67 Degree Celsius, 78.20 Degree Fahrenheit
```
*Figure 4: Output read from debug console for Exercise 2.2*
Since the change in temperature and change in the voltage are linear relationships, it is easy to find the constant `slope` and `offset` using the equation `y = slope* x + b` from two given reference digitized values. Furthermore, to make the temperature calculation more precise, all of the division computation uses `float` data type.
```c
 uint32_t* ref30 = (uint32_t*)0x002010EC; // ADC 2.5V reference digitzed value at 30 degree C
    uint32_t* ref85 = (uint32_t*)0x002010F0; // ADC 2.5V reference digitized value at  85 degree C
    //convert to voltage
    float voltage30 = ((float)(*ref30 * 2500)/ 16384);
    float voltage85 = ((float)(*ref85 * 2500)/ 16384);
    // since this is linear function it should be in the form y = slope*x + offset
    float slope = (float)(voltage85 - voltage30)/(85-30); //find slope
    float offset = voltage30 - slope*30;                      //find offset
   while(1){
        // Trigger conversion with software
        adc_done = false;
        ADC14_toggleConversionTrigger();
        while (!adc_done)
        {
        }
        uint32_t adc_value = ADC14_getResult(ADC_MEM0);
        float voltage = ((float)(adc_value * 2500)/ 16384);
        float C = (float)(voltage-offset)/slope;
        float F = ((float)(C * 9)/ 5) + 32;
        printf("Digitized value: %u, Corresponding voltage: %.2f mV, Temperature: %.2f Degree Celsius, %.2f Degree Fahrenheit \n", adc_value, voltage, C, F);
        delay_ms(500);
    }
```

