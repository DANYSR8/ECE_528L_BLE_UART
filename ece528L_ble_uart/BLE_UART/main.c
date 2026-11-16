/**
 * @file main.c
 *
 * @brief Main source code for the BLE_UART program.
 *
 * This file contains the main entry point for the BLE_UART program,
 * which is used to demonstrate the BLE_UART driver.
 *
 * It interfaces with the Adafruit Bluefruit LE UART Friend Bluetooth Low Energy (BLE) module, which uses the UART communication protocol.
 *  - Product Link: https://www.adafruit.com/product/2479
 *
 * @note For more information regarding the Enhanced Universal Serial Communication Interface (eUSCI),
 * refer to the MSP432Pxx Microcontrollers Technical Reference Manual
 *
 * @Daniel Silva Rios
 *
 */

#include "msp.h"
#include <stdint.h>
#include <math.h>

#include "inc/Clock.h"
#include "inc/CortexM.h"
#include "inc/GPIO.h"
#include "inc/EUSCI_A0_UART.h"
#include "inc/Motor.h"
#include "inc/BLE_UART.h"

#define PWM_NOMINAL 3000


void TX_Msg_A(void)
{

    BLE_UART_OutChar(170);
    Clock_Delay1ms(10);
    BLE_UART_OutChar(85);
    Clock_Delay1ms(10);
    BLE_UART_OutChar(255);
    Clock_Delay1ms(10);
    BLE_UART_OutChar(0);
    Clock_Delay1ms(10);

}


void Process_BLE_UART_Data(char BLE_UART_Buffer[])
{

    if(Check_BLE_UART_Data(BLE_UART_Buffer, "RGB LED GREEN"))
        {
            LED2_Output(RGB_LED_GREEN);

        }
    else if (Check_BLE_UART_Data(BLE_UART_Buffer,"RGB LED OFF"))
        {

            LED2_Output(RGB_LED_OFF);

        }
    else if(Check_BLE_UART_Data(BLE_UART_Buffer,"DANY"))
    {
            TX_Msg_A();
            printf("Sending TX Msg A \n");
    }

    else
        {

        printf("BLE UART Command Not Found \n");

        }

}







int main(void)
{

    DisableInterrupts();
    Clock_Init48MHz();
    LED2_Init();
    EUSCI_A0_UART_Init_Printf();
    BLE_UART_Init();
    Motor_Init();

    char BLE_UART_Buffer[BLE_UART_BUFFER_SIZE] = {0};

    EnableInterrupts();

    Clock_Delay1ms(1000);
    BLE_UART_Reset();

    BLE_UART_OutString("BLE UART Active\r \n");
    Clock_Delay1ms(1000);


    while(1)
    {

        int string_size = BLE_UART_InString(BLE_UART_Buffer, BLE_UART_BUFFER_SIZE);

        printf("BLE UART Data: ");

        for (int i = 0 ; i < string_size; i++)
        {
            printf("%c", BLE_UART_Buffer[i]);
        }

        printf("\n");

        Process_BLE_UART_Data(BLE_UART_Buffer);



    }
}
