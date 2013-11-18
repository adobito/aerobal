//*****************************************************************************
//
// uart_echo.c - Example for reading data from and writing data to the UART in
//               an interrupt driven fashion.
//
// Copyright (c) 2012-2013 Texas Instruments Incorporated.  All rights reserved.
// Software License Agreement
//
// Texas Instruments (TI) is supplying this software for use solely and
// exclusively on TI's microcontroller products. The software is owned by
// TI and/or its suppliers, and is protected under applicable copyright
// laws. You may not combine this software with "viral" open-source
// software in order to form a larger program.
//
// THIS SOFTWARE IS PROVIDED "AS IS" AND WITH ALL FAULTS.
// NO WARRANTIES, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT
// NOT LIMITED TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. TI SHALL NOT, UNDER ANY
// CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL, OR CONSEQUENTIAL
// DAMAGES, FOR ANY REASON WHATSOEVER.
//
// This is part of revision 1.1 of the EK-TM4C123GXL Firmware Package.
//
//*****************************************************************************

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "utils/uartstdio.h"
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "driverlib/debug.h"
#include "driverlib/fpu.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/pin_map.h"
#include "driverlib/rom.h"
#include "driverlib/sysctl.h"
#include "driverlib/uart.h"


char state = 0;
unsigned char buffer[20];
char count;
//*****************************************************************************
//
//! \addtogroup example_list
//! <h1>UART Echo (uart_echo)</h1>
//!
//! This example application utilizes the UART to echo text.  The first UART
//! (connected to the USB debug virtual serial port on the evaluation board)
//! will be configured in 115,200 baud, 8-n-1 mode.  All characters received on
//! the UART are transmitted back to the UART.
//
//*****************************************************************************


//*****************************************************************************
//
// The error routine that is called if the driver library encounters an error.
//
//*****************************************************************************
#ifdef DEBUG
void
__error__(char *pcFilename, uint32_t ui32Line)
{
}
#endif


//*****************************************************************************
//
// Send a string to the UART.
//
//*****************************************************************************
void
UARTSend(const uint8_t *pui8Buffer, uint32_t ui32Count)
{
	//
	// Loop while there are more characters to send.
	//
	while(ui32Count--)
	{
		//
		// Write the next character to the UART.
		//
		ROM_UARTCharPut(UART0_BASE, *pui8Buffer++);
	}
}

void
UARTBTSend(const uint8_t *pui8Buffer, uint32_t ui32Count)
{
	//
	// Loop while there are more characters to send.
	//
	while(ui32Count--)
	{
		//
		// Write the next character to the UART.
		//
		ROM_UARTCharPut(UART1_BASE, *pui8Buffer++);
	}
}


void evaluateBuffer() {
	int i = 0;
	//char subBuffer[20];
	for(i = 0; i < count; i++) {
		//if()
	}
	UARTBTSend(buffer,count);
	UARTBTSend("\nlol\n",5);
}
//*****************************************************************************
//
// The UART interrupt handler.
//
//*****************************************************************************
void
UARTIntHandler(void)
{
	uint32_t ui32Status;

	//
	// Get the interrupt status.
	//
	ui32Status = ROM_UARTIntStatus(UART0_BASE, true);

	//
	// Clear the asserted interrupts.
	//
	ROM_UARTIntClear(UART0_BASE, ui32Status);

	//
	// Loop while there are characters in the receive FIFO.
	//

	while(ROM_UARTCharsAvail(UART0_BASE))
	{
		//
		// Read the next character from the UART and write it back to the UART.
		//
		ROM_UARTCharPutNonBlocking(UART1_BASE,
				ROM_UARTCharGetNonBlocking(UART0_BASE));

		//
		// Blink the LED to show a character transfer is occuring.
		//
		GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2, GPIO_PIN_2);

		//
		// Delay for 1 millisecond.  Each SysCtlDelay is about 3 clocks.
		//
		SysCtlDelay(SysCtlClockGet() / (1000 * 3));

		//
		// Turn off the LED
		//
		GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2, 0);

	}
}

void
UARTBTIntHandler(void)
{
	uint32_t ui32Status;

	//
	// Get the interrupt status.
	//
	ui32Status = ROM_UARTIntStatus(UART1_BASE, true);

	//
	// Clear the asserted interrupts.
	//
	ROM_UARTIntClear(UART1_BASE, ui32Status);

	//
	// Loop while there are characters in the receive FIFO.
	//
	char reset = 0;
	while(ROM_UARTCharsAvail(UART1_BASE))
	{
		//
		// Read the next character from the UART and write it back to the UART.
		//
		char ch = ROM_UARTCharGetNonBlocking(UART1_BASE);


		switch(state) {
		case 0:
			if(ch == 'b') {
				state = 1;
				break;
			}
			reset = 1;
			break;
		case 1:
			if(ch == 't') {
				state = 2;
				break;
			}
			reset = 1;
			break;
		case 2:
			if(ch == ':') {
				state = 3;
				break;
			}
			reset = 1;
			break;
		case 3:
			if(ch == '\r') {
				reset = 1;
				evaluateBuffer();
				break;
			}
			break;
		}

		if(reset) {
			state = 0;
			if(ch == 'b') {
				state = 1;
			}
			count = 0;
		}
		buffer[count] = ch;
		count++;
		reset = 0;
		ROM_UARTCharPutNonBlocking(UART0_BASE,
				ch);


		//
		// Blink the LED to show a character transfer is occuring.
		//
		GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_3, GPIO_PIN_3);

		//
		// Delay for 1 millisecond.  Each SysCtlDelay is about 3 clocks.
		//
		SysCtlDelay(SysCtlClockGet() / (1000 * 3));

		//
		// Turn off the LED
		//
		GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_3, 0);

	}
}

unsigned GetNumberOfDigits (unsigned i)
{
	return i > 0 ? (int) log10 ((double) i) + 1 : 1;
}

// Yet, another good itoa implementation
void itoa(int value, char* out)
{
	int size = GetNumberOfDigits(value);
	out[size] = '\0';
	int i = size - 1;
	for(;i >= 0; i--) {
		out[i] = value%10 + '0';
		value /= 10;
	}
}

int strToInt(char* str) {
	int value = 0;
	int i;
	for(i = 0; str[i] != '\0'; i++) {
		value *= 10;
		value += str[i] - '0';
	}
	return value;
}

void concat(char* one, char* two, char* out) {
	int i;
	for(i = 0; one[i] != '\0'; i++) {
		out[i] = one[i];
	}
	int j;
	for(j = 0; two[j] != '\0'; j++) {
		out[j + i] = two[j];
	}
	out[i + j] = '\0';
}
int lengthOfString(char* str) {
	int i = 0;
	for(i = 0; str[i] != '\0'; i++);
	free(str);
	return i;
}


void send(char* name, char* value) {
	int size = strlen(name) + strlen(value) + 7;
	char* out = malloc(size*sizeof(char));
	concat("bt:",name,out);
	concat(out,"=",out);
	concat(out,value,out);
	concat(out,"\r\n",out);
	//char* str = concat(concat(concat(concat("bt:",name),"="),value),"\r\n");
	UARTBTSend((uint8_t *) out,size);
	free(out);
	SysCtlDelay(10000);
}
void sendDrag(int drag) {
	int numberOfDigits = GetNumberOfDigits(drag);
	char* num = malloc((10+numberOfDigits)*sizeof(char));
	itoa(drag,num);
	//char num[numberOfDigits];
	send("drag",num);
	free(num);

}


//*****************************************************************************
//
// This example demonstrates how to send a string of data to the UART.
//
//*****************************************************************************
int
main(void)
{
	//
	// Enable lazy stacking for interrupt handlers.  This allows floating-point
	// instructions to be used within interrupt handlers, but at the expense of
	// extra stack usage.
	//
	ROM_FPUEnable();
	ROM_FPULazyStackingEnable();

	//
	// Set the clocking to run directly from the crystal.
	//
	ROM_SysCtlClockSet(SYSCTL_SYSDIV_1 | SYSCTL_USE_OSC | SYSCTL_OSC_MAIN |
			SYSCTL_XTAL_16MHZ);

	//
	// Enable the GPIO port that is used for the on-board LED.
	//
	ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);

	//
	// Enable the GPIO pins for the LED (PF2).
	//
	ROM_GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_2);
	ROM_GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_3);

	//
	// Enable the peripherals used by this example.
	//
	ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
	ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
	ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_UART1);
	ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);

	//
	// Enable processor interrupts.
	//
	ROM_IntMasterEnable();

	//
	// Set GPIO A0 and A1 as UART pins.
	//
	GPIOPinConfigure(GPIO_PA0_U0RX);
	GPIOPinConfigure(GPIO_PA1_U0TX);
	GPIOPinConfigure(GPIO_PB0_U1RX);
	GPIOPinConfigure(GPIO_PB1_U1TX);
	ROM_GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);
	ROM_GPIOPinTypeUART(GPIO_PORTB_BASE, GPIO_PIN_0 | GPIO_PIN_1);

	//
	// Configure the UART for 115,200, 8-N-1 operation.
	//
	ROM_UARTConfigSetExpClk(UART0_BASE, ROM_SysCtlClockGet(), 115200,
			(UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE |
					UART_CONFIG_PAR_NONE));

	ROM_UARTConfigSetExpClk(UART1_BASE, ROM_SysCtlClockGet(), 9600,
			(UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE |
					UART_CONFIG_PAR_NONE));

	//
	// Enable the UART interrupt.
	//
	ROM_IntEnable(INT_UART0);
	ROM_UARTIntEnable(UART0_BASE, UART_INT_RX | UART_INT_RT);

	ROM_IntEnable(INT_UART1);
	ROM_UARTIntEnable(UART1_BASE, UART_INT_RX | UART_INT_RT);

	//
	// Prompt for text to be entered.
	//
	UARTSend((uint8_t *)"Enter text: \r\n", 14);
	//SysCtlDelay(10000000);
	//UARTBTSend((uint8_t *)"AT\r",3);

	//
	// Loop forever echoing data through the UART.
	//

	while(1)
	{
		send("drag","445");
		sendDrag(234);
	}
}
