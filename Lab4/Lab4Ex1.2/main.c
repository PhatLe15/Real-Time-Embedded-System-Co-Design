/* --COPYRIGHT--,BSD
 * Copyright (c) 2017, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * --/COPYRIGHT--*/
/******************************************************************************
 * MSP432 Empty Project
 *
 * Description: An empty project that uses DriverLib
 *
 *                MSP432P401
 *             ------------------
 *         /|\|                  |
 *          | |                  |
 *          --|RST               |
 *            |                  |
 *            |                  |
 *            |                  |
 *            |                  |
 *            |                  |
 * Author: 
*******************************************************************************/
/* DriverLib Includes */
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>

/* Standard Includes */
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

char* string1 = "12345";
char string2[] = "12345";
const char string3[1024*150] = {"12345"};

uint32_t sector;
uint32_t bank;

void regionConverter(char* a);

int main(void)
{
    // unprotect memory
    FlashCtl_unprotectSector(FLASH_MAIN_MEMORY_SPACE_BANK0, FLASH_SECTOR0); //for string 1
    FlashCtl_unprotectSector(FLASH_MAIN_MEMORY_SPACE_BANK1, FLASH_SECTOR5); //for string 3

    //find the exact sector and bank
    printf("Region of string 1 is: \n");
    regionConverter(string1);
    printf("Region of string 3 is: \n");
    regionConverter(string3);

    //print out the result before and after modification
    printf("Before:\n");
    printf("string1: %s\n", string1);
    printf("string2: %s\n", string2);
    printf("string3: %s\n", string3);

    //modify string 2
    string2[0] = '0';
    string2[1] = '4';

    //program 2 bytes of string 1 and 3 from string 2
    ROM_FlashCtl_programMemory(string2, string1, 2);
    ROM_FlashCtl_programMemory(string2, string3, 2);

    //print out the result after modification
    printf("After\n");
    printf("string1: %s\n", string1);   //attempt 1: value wont change
    printf("string2: %s\n", string2);   // value can change
    printf("string3: %s\n", string3);

}

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

