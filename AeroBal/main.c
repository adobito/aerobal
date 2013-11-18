/************************************************/
// System Includes - ARM
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include "inc/hw_memmap.h"
#include "driverlib/interrupt.h"
#include "driverlib/rom.h"
#include "driverlib/timer.h"
#include "driverlib/sysctl.h"
//#include "inc/tm4c123gh6pm.h"
//#include "inc/hw_nvic.h"
//#include "inc/hw_types.h"
//#include "inc/hw_ints.h"
//#include "driverlib/debug.h"
#include "driverlib/fpu.h"
#include "driverlib/gpio.h"
//#include "driverlib/systick.h"
#include "driverlib/uart.h"
#include "driverlib/pin_map.h"
//#include "utils/uartstdio.h"
/***********************************************/
// Libraries
#include "a.lib/gpio.h"
#include "a.lib/lcd.h"
/***********************************************/



char state = 0;
char buffer[20];
char count;

//*****************************************************************************
#ifdef DEBUG
void
__error__(char *pcFilename, uint32_t ui32Line)
{
}
#endif


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
		UARTCharPut(UART0_BASE, *pui8Buffer++);
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
		UARTCharPut(UART2_BASE, *pui8Buffer++);
	}
}


int strequals(char* one, char* two) {
	int eq = 1;
	int i = 0;
	for(i = 0; one[i] != '\0' || two[i] != '\0'; i++) {
		if(one[i] != two[i]) {
			eq = 0;
			break;
		}
	}
	eq = one[i] == two[i];
	return eq;
}
void turnPowerOn() {
	puts("powering on");
	UARTBTSend("On\r\n", 4);
	gpioSetData(GPIO_PORTD,0x01,0x01);
}

void turnPowerOff() {
	puts("powering off");
	UARTBTSend("Off\r\n", 5);
	gpioSetData(GPIO_PORTD,0x01,0x00);
}
void evaluateBuffer() {
	if(strequals("bt:power=on",buffer)) {
		turnPowerOn();
	}
	else if(strequals("bt:power=off",buffer)) {
		turnPowerOff();
	}
	/*else if(strequals("bt:power=off\r\n",buffer)) {
		turnPowerOn();
	}
	else if(strequals("bt:power=off\r\n",buffer)) {
		turnPowerOn();
	}
	else if(strequals("bt:power=off\r\n",buffer)) {
		turnPowerOn();
	}
	else if(strequals("bt:power=off\r\n",buffer)) {
		turnPowerOn();
	}
	else if(strequals("bt:power=off\r\n",buffer)) {
		turnPowerOn();
	}*/
}

void
UARTIntHandler(void)
{
	uint32_t ui32Status;

	//
	// Get the interrupt status.
	//
	ui32Status = UARTIntStatus(UART0_BASE, true);

	//
	// Clear the asserted interrupts.
	//
	UARTIntClear(UART0_BASE, ui32Status);

	//
	// Loop while there are characters in the receive FIFO.
	//

	while(UARTCharsAvail(UART0_BASE))
	{
		//
		// Read the next character from the UART and write it back to the UART.
		//
		UARTCharPutNonBlocking(UART2_BASE,
				UARTCharGetNonBlocking(UART0_BASE));

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
	ui32Status = UARTIntStatus(UART1_BASE, true);

	//
	// Clear the asserted interrupts.
	//
	UARTIntClear(UART2_BASE, ui32Status);

	//
	// Loop while there are characters in the receive FIFO.
	//
	char reset = 0;
	while(UARTCharsAvail(UART2_BASE))
	{
		//
		// Read the next character from the UART and write it back to the UART.
		//
		char ch = UARTCharGet(UART2_BASE);


		switch(state) {
		case 0:
			if(ch == 'b') {
				state = 1;
				count = 0;
				//buffer[0] = ch;
				//count = 1;
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
			if(ch == '\r' || ch == '\n') {
				reset = 1;
				buffer[count] = '\0';
				evaluateBuffer();
				break;
			}
			break;
		default:
			reset = 1;
			break;
		}

		if(reset) {
			state = 0;
			if(ch == 'b') {
				buffer[0] = ch;
				state = 1;
			}
			count = 0;
		}
		buffer[count] = ch;
		count++;
		reset = 0;
		UARTCharPut(UART0_BASE,
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
void ftoa(float value, char* out ) {
	int integer = (int) value;
	itoa(integer,out);
	char* pt = ".";
	concat(out,pt,out);
	char curr[6];
	itoa((int)((value - (int)value)*10000),curr);
	concat(out,curr,out);

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

int lengthOfString(char* str) {
	int i = 0;
	for(i = 0; str[i] != '\0'; i++);
	free(str);
	return i;
}

int strlen(char *string){
	int i = 0 ;
	for(i = 0; string[i]!='\0';i++);
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
void sendDrag(float drag) {
	char name[20];
	ftoa(drag,name);
	//char num[numberOfDigits];
	send("drag",name);
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
	FPUEnable();
	FPULazyStackingEnable();

	//
	// Set the clocking to run directly from the crystal.
	//
	SysCtlClockSet(SYSCTL_SYSDIV_1 | SYSCTL_USE_OSC | SYSCTL_OSC_MAIN |
			SYSCTL_XTAL_16MHZ);

	//
	// Enable the GPIO port that is used for the on-board LED.
	//
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);

	//
	// Enable the GPIO pins for the LED (PF2).
	//
	GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_2);
	GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_3);

	//
	// Enable the peripherals used by this example.
	//
	SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_UART2);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);

	//
	// Enable processor interrupts.
	//
	IntMasterEnable();

	//
	// Set GPIO A0 and A1 as UART pins.
	//
	GPIOPinConfigure(GPIO_PA0_U0RX);
	GPIOPinConfigure(GPIO_PA1_U0TX);
	GPIOPinConfigure(GPIO_PD6_U2RX);
	GPIOPinConfigure(GPIO_PD7_U2TX);
	GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);
	GPIOPinTypeUART(GPIO_PORTD_BASE, GPIO_PIN_6 | GPIO_PIN_7);

	gpioSetMasterEnable(GPIO_PORTD);
	gpioSetDirection(GPIO_PORTD,0x01,0x01);
	gpioSetDigitalEnable(GPIO_PORTD,0x01,0x01);


	//
	// Configure the UART for 115,200, 8-N-1 operation.
	//
	UARTConfigSetExpClk(UART0_BASE, SysCtlClockGet(), 115200,
			(UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE |
					UART_CONFIG_PAR_NONE));

	UARTConfigSetExpClk(UART2_BASE, SysCtlClockGet(), 9600,
			(UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE |
					UART_CONFIG_PAR_NONE));

	//
	// Enable the UART interrupt.
	//
	IntEnable(INT_UART0);
	UARTIntEnable(UART0_BASE, UART_INT_RX | UART_INT_RT);

	IntEnable(INT_UART2);
	UARTIntEnable(UART2_BASE, UART_INT_RX | UART_INT_RT);

	//
	// Prompt for text to be entered.
	//
	UARTBTSend((uint8_t *)"Enter text: \r\n", 14);
	//SysCtlDelay(10000000);
	//UARTBTSend((uint8_t *)"AT\r",3);

	//
	// Loop forever echoing data through the UART.
	//
	char floa[20];
	ftoa(34.5,floa);
	puts(floa);
	while(1)
	{
	}
}
