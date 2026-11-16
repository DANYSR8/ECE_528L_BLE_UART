/**
 * @file BLE_UART.c
 *
 * @brief Source code for the BLE_UART driver.
 *
 * This file contains the function definitions for the BLE_UART driver.
 *
 * It interfaces with the Adafruit Bluefruit LE UART Friend Bluetooth Low Energy (BLE) module, which uses the UART communication protocol.
 *  - Product Link: https://www.adafruit.com/product/2479
 *
 * The following connections must be made:
 *  - BLE UART MOD  (Pin 1)     <-->  MSP432 LaunchPad Pin P1.6
 *  - BLE UART CTS  (Pin 2)     <-->  MSP432 LaunchPad GND
 *  - BLE UART TXO  (Pin 3)     <-->  MSP432 LaunchPad Pin P9.6 (PM_UCA3RXD)
 *  - BLE UART RXI  (Pin 4)     <-->  MSP432 LaunchPad Pin P9.7 (PM_UCA3TXD)
 *  - BLE UART VIN  (Pin 5)     <-->  MSP432 LaunchPad VCC (3.3V)
 *  - BLE UART RTS  (Pin 6)     <-->  Not Connected
 *  - BLE UART GND  (Pin 7)     <-->  MSP432 LaunchPad GND
 *  - BLE UART DFU  (Pin 8)     <-->  Not Connected
 *
 * @note For more information regarding the Enhanced Universal Serial Communication Interface (eUSCI),
 * refer to the MSP432Pxx Microcontrollers Technical Reference Manual
 *
 * @author
 *
 */

#include "../inc/BLE_UART.h"



/*Initializes eUSCI_A3 in UART mode to talk to the Adafruit Bluefruit LE UART Friend module.
 *
 * Sets P9.6 and P9.7 to their primary UART function and P1.6 as a GPIO output for the MOD pin.
 * Configures the UART for 8 data bits, no parity, 1 stop bit, LSB first, clocked from SMCLK at 9600 baud.
*/
void BLE_UART_Init()
{

    P9->SEL0 |= 0xC0;
    P9->SEL1 &= ~0xC0;

    P1->SEL0 &= ~0x40;
    P1->SEL1 &= ~0x40;
    P1->DIR |= 0x40;


    EUSCI_A3->CTLW0 |= 0x01;
    EUSCI_A3->MCTLW &= ~0xFF;
    EUSCI_A3->CTLW0 &= ~0x8000;
    EUSCI_A3->CTLW0 &= ~0x4000;
    EUSCI_A3->CTLW0 &= ~0x2000;
    EUSCI_A3->CTLW0 &= ~0x1000;
    EUSCI_A3->CTLW0 &= ~0x0800;
    EUSCI_A3->CTLW0 &= ~0x0600;
    EUSCI_A3->CTLW0 &= ~0x0100;
    EUSCI_A3->CTLW0 |= 0x00C0;

    EUSCI_A3->BRW = 1250;
    EUSCI_A3->IE &= ~0x0C;
    EUSCI_A3->IE |= 0x03;

    EUSCI_A3->CTLW0 &= ~0x01;



}




// Waits until a byte arrives in the eUSCI_A3 receive buffer, then returns it.
// The 8-bit character read from the RXBUF register.
uint8_t BLE_UART_InChar()
{

    while ((EUSCI_A3->IFG & 0x01) == 0 );
    return EUSCI_A3->RXBUF;
}






//Waits until the eUSCI_A3 transmit buffer is ready, then sends one byte to the BLE module.
// Take in the 8-bit character to transmit.
void BLE_UART_OutChar(uint8_t data)
{

    while ((EUSCI_A3->IFG & 0x02) == 0 );
    EUSCI_A3->TXBUF = data;


}





/*
Reads characters from the BLE module into a buffer until a line feed (LF) is received.
 
Handles backspace (BS) by removing the last character, ignores characters once the buffer is full,
and null-terminates the string.

Takes the pointer to the character buffer that stores the received string.
and maximum number of characters the buffer can hold.

It retuns the number of characters stored in the buffer.
*/

int BLE_UART_InString(char *buffer_pointer, uint16_t buffer_size)
{

    int length = 0;
    int string_size = 0;

    char character = BLE_UART_InChar();

    while(character != LF)
    {
      if (character == BS)
      {
        if(length)
        {

            buffer_pointer--;
            length--;
            BLE_UART_OutChar(BS);

        }
    }

    else if (length < buffer_size)
        {
            *buffer_pointer = character;
            buffer_pointer++;
            length++;
            string_size++;
        }
        character = BLE_UART_InChar();
    }
    *buffer_pointer = 0;
    return string_size;
}






//Sends a null-terminated string to the BLE module one character at a time.
// It takes in pointer to the string to transmit.

void BLE_UART_OutString(char *pt)
{

    while(*pt)
    {
        BLE_UART_OutChar(*pt);
        pt++;
    }


}







/*
Checks whether a given string appears anywhere in the BLE UART data buffer.

Takes in the buffer holding the received BLE data and the string to search for.
*/

uint8_t Check_BLE_UART_Data(char BLE_UART_Data_Buffer[], char *data_string)
{
    if(strstr(BLE_UART_Data_Buffer,data_string) != NULL)
    {
        return 0x01;
    }
    else
    {
        return 0x00;
    }

}







/*
Performs a software reset of the BLE module using the "ATZ" AT command.

Drives the MOD pin (P1.6) high to enter command mode, sends "ATZ", waits for the module to reset,
then drives MOD low to return to data mode.
*/


void BLE_UART_Reset()
{

    P1->OUT |= 0x40;
    Clock_Delay1ms(1000);

    BLE_UART_OutString("ATZ\r\n");
    Clock_Delay1ms(3000);

    P1->OUT &= ~0x40;


}
