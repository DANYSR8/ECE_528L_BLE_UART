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
#include <stdio.h>

#include "inc/Clock.h"
#include "inc/CortexM.h"
#include "inc/GPIO.h"
#include "inc/EUSCI_A0_UART.h"
#include "inc/Motor.h"
#include "inc/BLE_UART.h"
#include "inc/Timer_A0_PWM.h"

#define PWM_NOMINAL 3000

// --------------Part 6.1A ----------------
void TX_Msg_A(void)
{

    BLE_UART_OutChar(170);
    Clock_Delay1ms(1);
    BLE_UART_OutChar(85);
    Clock_Delay1ms(1);
    BLE_UART_OutChar(255);
    Clock_Delay1ms(1);
    BLE_UART_OutChar(0);
    Clock_Delay1ms(1);

}

// -------------------------------------




// --------------Part 6.1B ----------------
void TX_Msg_B(void)
{

    switch (Get_PMOD_SWT_Status()){

        case 0x01:
            BLE_UART_OutChar(0xAA);
            printf("SW1\n");
            break;

        case 0x02:
            BLE_UART_OutChar(0x10);
            printf("SW2\n");
            break;

        case 0x04:
            BLE_UART_OutChar(0xF0);
            printf("SW3\n");
            break;
    
        case 0x08:
            BLE_UART_OutChar(0x0F);
            printf("SW4\n");
            break;

        default:
            BLE_UART_OutChar(0x00);
            printf("Default\n");
            break;
    }

}
// -------------------------------------






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

        
    // ---------------Part 6.1A -------------------------------------
    else if(Check_BLE_UART_Data(BLE_UART_Buffer,"DANY"))
        {
                TX_Msg_A();
                printf("Sending TX Msg A \n");
        }
    // --------------------------------------------------------------

   
        
    // ---------------Part 6.1B -------------------------------------
    else if(Check_BLE_UART_Data(BLE_UART_Buffer,"SILVA"))
        {
                TX_Msg_B();
                printf("Sending TX Msg B \n");
        }

    // -------------------------------------------------------------

        
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

    // ----- Added for Task 6.1 Part B -----
    PMOD_SWT_Init(); 
    // ------------------------------------
    

    char BLE_UART_Buffer[BLE_UART_BUFFER_SIZE] = {0};

    EnableInterrupts();

    Clock_Delay1ms(1000);
    BLE_UART_Reset();

    BLE_UART_OutString("BLE UART Active\r \n");
    Clock_Delay1ms(1000);


    while(1)
    {

        /*
        int string_size = BLE_UART_InString(BLE_UART_Buffer, BLE_UART_BUFFER_SIZE);

        printf("BLE UART Data: ");

        for (int i = 0 ; i < string_size; i++)
        {
            printf("%c", BLE_UART_Buffer[i]);
        }

        printf("\n");

        Process_BLE_UART_Data(BLE_UART_Buffer);
        */



       BLE_UART_InPacket(BLE_UART_Buffer);

        printf("BLE UART Data: ");

        for (int i = 0 ; i < 4; i++)
        {
            printf("%c", BLE_UART_Buffer[i]);
        }

        printf("\n");


        Decode_Controller_Packet(BLE_UART_Buffer);






    }
}
