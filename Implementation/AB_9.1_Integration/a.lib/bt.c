/*
 * bt.c
 *
 *  Created on: Nov 21, 2013
 *      Author: Administrator
 */


//*****************************************************************************
//mj
// uaasrt_echo.c - Example for reading data from and writing data to the UART in
//               an interrupt driven fashion.
//
// Copyright (c) 2012-2013 Texas Instruments Incorporated.z  All rights reserved.
// Softsaware License Agreement
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
#include "inc/hw_types.h"
#include "inc/hw_gpio.h"
#include "driverlib/debug.h"
#include "driverlib/fpu.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/pin_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/uart.h"
#include "driverlib/timer.h"
#include <time.h>
struct Node {
	struct Node *next;
	float element;
};
struct Queue {
	int size;
	struct Node *head;
	struct Node *tail;
};


//////////
//Rolling Buffer
//////////
struct RollingBuffer {
	float total;
	int current;
	float buffer[100];
};
/////////////////////////////////////////
//Queue / Linked List struct signatures
//////////////////////////////////////////
void Enqueue(struct Queue *q, float value);
float Dequeue(struct Queue *q);
int IsEmpty(struct Queue *q);
float Get(struct Queue *q, int index);
float Peek(struct Queue *q);
////////////////////////////////////////////
//Rolling BUffer Signatures
//////////////////////////////////////////
void InitBuffer(struct RollingBuffer *r);
void AddToBuffer(struct RollingBuffer *r, float value);

///////////////////////////////////////
//Bluetooth Protocol Signatures
/////////////////////////////////////////
void sendDrag(float drag);
void sendTilt(float tilt);
void sendLift(float lift);
void sendDragMeasurement(float drag);
void sendTiltMeasurement(float tilt);
void sendLiftMeasurement(float lift);
void initiateTunnelSetup();
void sendState(int state);
//void setupAck();
void sendMeasurements(float drag, float lift, float tilt);


//////////////////////////////////////////
//Gloval Variables
///////////////////////////////////////////

struct RollingBuffer dragBuffer;
struct RollingBuffer liftBuffer;
struct RollingBuffer tiltBuffer;
struct RollingBuffer pressureBuffer;
struct RollingBuffer humidityBuffer;
struct RollingBuffer speedBuffer;

char buffer[20];
char count;
char state = 0;
char buffer[20];
char count;
char setupState = 0;

float initUpStaticEmpty = -1;
float initDownStaticEmpty = -1;
float initLeftStaticEmpty = -1;
float initRightStaticEmpty = -1;

float initUpDynamicEmpty = -1;
float initDownDynamicEmpty = -1;
float initLeftDynamicEmpty = -1;
float initRightDynamicEmpty = -1;

float initUpStaticLoad = -1;
float initDownStaticLoad = -1;
float initLeftStaticLoad = -1;
float initRightStaticLoad = -1;

/////////////////////////////////////////////
//Quueue Struct Methods
/////////////////////////////////////////////
void Enqueue(struct Queue *q, float value) {
	struct Node *newNode = (struct Node*) malloc(sizeof(struct Node));
	newNode->next = NULL;
	newNode->element = value;
	if(q->size == 0) {
		q->head = newNode;
	}
	else {
		q->tail->next = newNode;
	}
	q->tail = newNode;
	q->size = q->size + 1;
}
float Dequeue(struct Queue *q) {
	if(q->size < 0) {
		return -1;
	}
	float tbr = q->head->element;
	struct Node* oldNode = q->head;
	q->head = oldNode->next;
	q->size = q->size - 1;
	if(q->size == 0) {
		q->tail = NULL;
	}
	free(oldNode);
	return tbr;
}
int IsEmpty(struct Queue *q) {
	return q->size == 0;
}
float Peek(struct Queue *q) {
	return q->head->element;
}

float Get(struct Queue *q, int index) {
	if(index >= 0 && index < q->size) {
		struct Node* curr =  q->head;
		int i = 0;
		for(i = 0; i < index; i++) {
			curr = curr->next;
		}
		return curr->element;
	}
	return -1;
}


///////////////////////////////////
//Rolling Buffer Functions
//////////////////////////////////
void InitBuffer(struct RollingBuffer *r) {
     int i;
     for(i = 0; i < 20; i++) {
           r->buffer[i] = 0;
     }
     r->current = 0;
     r->total = 0;
}
void AddToBuffer(struct RollingBuffer *r, float value) {
     r->total -= r->buffer[r->current];
     r->buffer[r->current++] = value;
     r->current %=20;
     r->total += value;
}
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



float max(float one, float two) {
	if(one > two)
			return one;
	return two;
}
float getDrag() {

	return rand();
}
float getLift() {
	//float max = max()
	return rand();
}
float getTilt() {
	return rand();
}
float getPressure() {
	return rand();
}
float getHumidity() {
	return rand();
}
float getSpeed() {
	return rand();
}

//////////////////////
//Timer Int
//////////////////////
void IntTimerProcessValues() {
	TimerIntClear(TIMER4_BASE, TIMER_TIMA_TIMEOUT);
	AddToBuffer(&dragBuffer, getDrag());
	AddToBuffer(&liftBuffer, getLift());
	AddToBuffer(&tiltBuffer, getTilt());
	AddToBuffer(&pressureBuffer, getPressure());
	AddToBuffer(&humidityBuffer, getHumidity());
	AddToBuffer(&speedBuffer, getSpeed());
	//AddToBuffer(&dragBuffer, getDrag());
}


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

void turnPowerOn() {
	UARTBTSend("On\r",3);
	GPIOPinWrite(GPIO_PORTF_BASE,GPIO_PIN_0,0);
}


void turnPowerOff() {
	UARTBTSend("Off\r",4);

	GPIOPinWrite(GPIO_PORTF_BASE,GPIO_PIN_0,GPIO_PIN_0);
}

void sendMeasurements(float drag, float lift, float tilt) {
	sendDragMeasurement(drag);//(rand()%10000)/100.0);
	sendLiftMeasurement(lift);//(rand()%10000)/100.0);;
	sendTiltMeasurement(tilt);//(rand()%10000)/100.0);;

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
void evaluateBuffer() {
	if(strequals("bt:power=on",buffer)) {
		turnPowerOn();
	}
	else if(strequals("bt:power=off",buffer)) {
		turnPowerOff();
	}
	else if(strequals("bt:measure",buffer)) {
		sendMeasurements(3.3,5,12);
	}

	else if(strequals("bt:setup=init",buffer)) {
		initiateTunnelSetup();
	}

	else if(strequals("bt:setup=ack",buffer)) {
		//setupAck();
	}/*
	else if(strequals("bt:power=off\r\n",buffer)) {
		turnPowerOn();
	}
	else if(strequals("bt:power=off\r\n",buffer)) {
		turnPowerOn();
	}*/
}

void
UARTBTIntHandler(void)
{
	uint32_t ui32Status;

	//
	// Get the interrupt status.
	//
	ui32Status = UARTIntStatus(UART2_BASE, true);

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
		char ch = UARTCharGetNonBlocking(UART2_BASE);


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

		UARTCharPutNonBlocking(UART0_BASE,
				ch);


		//
		// Blink the LED to show a character transfer is occuring.
		//
		//GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_3, GPIO_PIN_3);

		//
		// Delay for 1 millisecond.  Each SysCtlDelay is about 3 clocks.
		//
		//SysCtlDelay(SysCtlClockGet() / (1000 * 3));

		//
		// Turn off the LED
		//
		//GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_3, 0);

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

void ftoa(float value, char* out ) {
	int integer = (int) value;
	itoa(integer,out);
	char* pt = ".";
	concat(out,pt,out);
	char curr[6];
	itoa((int)((value - (int)value)*10000),curr);
	concat(out,curr,out);

}

void send(char* name, char* value) {
	int size = strlen(name) + strlen(value) + 7;
	//char* out = malloc(size*sizeof(char));
	//UARTCharPut(UART0_BASE,ch);
	//concat("bt:",name,out);
	//concat(out,"=",out);
	//concat(out,value,out);
	//concat(out,"\r\n",out);
	//char* str = concat(concat(concat(concat("bt:",name),"="),value),"\r\n");
	//UARTBTSend((uint8_t *) out,size);
	//free(out);
	UARTCharPut(UART2_BASE,'b');
	UARTCharPut(UART2_BASE,'t');
	UARTCharPut(UART2_BASE,':');
	int i;
	for(i = 0; name[i] != '\0'; i++) {
		UARTCharPut(UART2_BASE,name[i]);
	}
	UARTCharPut(UART2_BASE,'=');
	for(i = 0; value[i] != '\0'; i++) {
		UARTCharPut(UART2_BASE,value[i]);
	}
	UARTCharPut(UART2_BASE,'\r');
	UARTCharPut(UART2_BASE,'\n');

	SysCtlDelay(10000);
}
void sendDrag(float drag) {
	char name[20];
	ftoa(drag,name);
	//char num[numberOfDigits];
	send("drag",name);
}
void sendLift(float lift) {
	char name[20];
	ftoa(lift,name);
	//char num[numberOfDigits];
	send("lift",name);
}
void sendTilt(float tilt) {
	char name[20];
	ftoa(tilt,name);
	//char num[numberOfDigits];
	send("tilt",name);
}
void sendDragMeasurement(float drag) {
	char name[20];
	ftoa(drag,name);
	//char num[numberOfDigits];
	send("msrdrag",name);
}
void sendLiftMeasurement(float lift) {
	char name[20];
	ftoa(lift,name);
	//char num[numberOfDigits];
	send("msrlift",name);
}
void sendTiltMeasurement(float tilt) {
	char name[20];
	ftoa(tilt,name);
	//char num[numberOfDigits];
	send("msrtilt",name);
}
void sendPressure(float pressure) {
	char name[20];
	ftoa(pressure,name);
	//char num[numberOfDigits];
	send("pressure",name);
}
void sendHumidity(float humidity) {
	char name[20];
	ftoa(humidity,name);
	//char num[numberOfDigits];
	send("humidity",name);
}
void sendSpeed(float speed) {
	char name[20];
	ftoa(speed,name);
	//char num[numberOfDigits];
	send("speed",name);
}

void sendState(int state) {
	char name[20];
	ftoa(state,name);
	send("state",name);
}
void unlockPD7() {
	//#define GPIO_PORTD_BASE 0x40007000 // GPIO Port D
	//#define GPIO_O_LOCK 0x00000520 // GPIO Lock

	//After GPIOLOCK is modified, we are then able to set bits 0:7
	//of hardware register GPIOCR to make the configuration registers
	// modifiable.

	HWREG(GPIO_PORTD_BASE + GPIO_O_LOCK) = GPIO_LOCK_KEY;

	//Set the commit register for PD7 to allow changing the function
	HWREG(GPIO_PORTD_BASE + GPIO_O_CR) = 0x80;

	//Enable the alternate function for PD7 (U2TX)
	HWREG(GPIO_PORTD_BASE + GPIO_O_AFSEL) |= 0x80;


	// Turn on the digital enable for PD7
	HWREG(GPIO_PORTD_BASE + GPIO_O_DEN) |= 0x80;


	// Relock the commit register, to prevent further changes
	HWREG(GPIO_PORTD_BASE + GPIO_O_LOCK) = 0;
}

void unlockPF0() {
	//#define GPIO_PORTD_BASE 0x40007000 // GPIO Port D
	//#define GPIO_O_LOCK 0x00000520 // GPIO Lock

	//After GPIOLOCK is modified, we are then able to set bits 0:7
	//of hardware register GPIOCR to make the configuration registers
	// modifiable.

	HWREG(GPIO_PORTF_BASE + GPIO_O_LOCK) = GPIO_LOCK_KEY;

	//Set the commit register for PD7 to allow changing the function
	HWREG(GPIO_PORTF_BASE + GPIO_O_CR) = 0x01;

	//Enable the alternate function for PD7 (U2TX)
	HWREG(GPIO_PORTF_BASE + GPIO_O_AFSEL) |= 0x01;


	// Turn on the digital enable for PD7
	HWREG(GPIO_PORTF_BASE + GPIO_O_DEN) |= 0x01;


	// Relock the commit register, to prevent further changes
	HWREG(GPIO_PORTF_BASE + GPIO_O_LOCK) = 0;
}

/*
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

	init();
	IntEnable(INT_TIMER4A);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER4);
		TimerConfigure(TIMER4_BASE, TIMER_CFG_PERIODIC);
		TimerLoadSet(TIMER4_BASE, TIMER_A, SysCtlClockGet()/100);
		IntEnable(INT_TIMER4A);
		TimerIntEnable(TIMER4_BASE, TIMER_TIMA_TIMEOUT);
		TimerEnable(TIMER4_BASE, TIMER_A);
	srand(time(NULL));
	char arr[20];
	while(1)
	{

	}
}
*/
void BluetoothMainInit() {

	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);
	unlockPD7();
	unlockPF0();
	//
	// Enable fthe GPIO pins for the LED (PF2).
	//
	//GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_2);
	//GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_3);
	GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_0);

	//
	// Enable the peripherals used by this example.
	//
	//SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
	//SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_UART2);
	//SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);

	//
	// Enable processor interrupts.
	//
	IntMasterEnable();
	//Timer4AInit();

	// Set GPIO A0 and A1 as UART pins.
	//
	//GPIOPinConfigure(GPIO_PA0_U0RX);
	//GPIOPinConfigure(GPIO_PA1_U0TX);
	GPIOPinConfigure(GPIO_PD6_U2RX);
	GPIOPinConfigure(GPIO_PD7_U2TX);
	//GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);
	GPIOPinTypeUART(GPIO_PORTD_BASE, GPIO_PIN_6 | GPIO_PIN_7);

	//
	// Configure the UART for 115,200, 8-N-1 operation.
	//
	//UARTConfigSetExpClk(UART0_BASE, SysCtlClockGet(), 115200,
	//		(UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE |
	//				UART_CONFIG_PAR_NONE));

	UARTConfigSetExpClk(UART2_BASE, SysCtlClockGet(), 9600,
			(UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE |
					UART_CONFIG_PAR_NONE));

	//
	// Enable the UART interrupt.
	//
	//IntEnable(INT_UART0);
	//UARTIntEnable(UART0_BASE, UART_INT_RX | UART_INT_RT);

	IntEnable(INT_UART2);
	UARTIntEnable(UART2_BASE, UART_INT_RX | UART_INT_RT);

	//
	// Prompt for text to be entered.
	//
	//SysCtlDelay(10000000);
	//UARTBTSend((uint8_t *)"AT\r",3);

	//
	// Loop forever echoing data through the UART.
	//

}
