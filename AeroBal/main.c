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
#include "sensorlib/hw_bmp180.h"
#include "sensorlib/i2cm_drv.h"
#include "sensorlib/bmp180.h"
/***********************************************/
// Libraries
#include "a.lib/gpio.h"
#include "a.lib/lcd.h"
/***********************************************/

//*****************************************************************************
//
// Define BMP180 I2C Address.
//
//*****************************************************************************
#define BMP180_I2C_ADDRESS  0x77

//*****************************************************************************
//
// Global array for holding the color values for the RGB.
//
//*****************************************************************************
uint32_t g_pui32Colors[3];

//*****************************************************************************
//
// Global instance structure for the I2C master driver.
//
//*****************************************************************************
tI2CMInstance g_sI2CInst;

//*****************************************************************************
//
// Global instance structure for the BMP180 sensor driver.
//
//*****************************************************************************
tBMP180 g_sBMP180Inst;

//*****************************************************************************
//
// Global new data flag to alert main that BMP180 data is ready.
//
//*****************************************************************************
volatile uint_fast8_t g_vui8DataFlag;


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

//*****************************************************************************
//
// Called by the NVIC as a result of I2C3 Interrupt. I2C3 is the I2C connection
// to the BMP180.
//
//*****************************************************************************
void
BMP180I2CIntHandler(void)
{
    //
    // Pass through to the I2CM interrupt handler provided by sensor library.
    // This is required to be at application level so that I2CMIntHandler can
    // receive the instance structure pointer as an argument.
    //
    I2CMIntHandler(&g_sI2CInst);
}

//*****************************************************************************
//
// BMP180 Sensor callback function.  Called at the end of BMP180 sensor driver
// transactions. This is called from I2C interrupt context. Therefore, we just
// set a flag and let main do the bulk of the computations and display.
//
//*****************************************************************************
void BMP180AppCallback(void* pvCallbackData, uint_fast8_t ui8Status)
{
    if(ui8Status == I2CM_STATUS_SUCCESS)
    {
        g_vui8DataFlag = 1;
    }
}

float fTemperature, fPressure, fAltitude;
int32_t i32IntegerPart;
int32_t i32FractionPart;

///////////////////////////////////////////////////////////////////////////////
void initLCD(){

	lcdInit(GPIO_PORTE,GPIO_PORTC,GPIO_PORTD);
	lcdClear();
	lcdCursorHome();
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


void initBMP(){

	 IntMasterEnable();
    //
    // The I2C3 peripheral must be enabled before use.
    //
    SysCtlPeripheralEnable(SYSCTL_PERIPH_I2C1);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);

    //
    // Configure the pin muxing for I2C3 functions on port D0 and D1.
    // This step is not necessary if your part does not support pin muxing.
    //
    GPIOPinConfigure(GPIO_PA6_I2C1SCL);
    GPIOPinConfigure(GPIO_PA7_I2C1SDA);

    //
    // Select the I2C function for these pins.  This function will also
    // configure the GPIO pins pins for I2C operation, setting them to
    // open-drain operation with weak pull-ups.  Consult the data sheet
    // to see which functions are allocated per pin.
    //
    GPIOPinTypeI2CSCL(GPIO_PORTA_BASE, GPIO_PIN_6);
    GPIOPinTypeI2C(GPIO_PORTA_BASE, GPIO_PIN_7);

    //
    // Initialize I2C3 peripheral.
    //
    I2CMInit(&g_sI2CInst, I2C1_BASE, INT_I2C1, 0xff, 0xff,
                 ROM_SysCtlClockGet());

    //
    // Initialize the BMP180
    //
    BMP180Init(&g_sBMP180Inst, &g_sI2CInst, BMP180_I2C_ADDRESS,
                BMP180AppCallback, &g_sBMP180Inst);


}

void bmpRead(){
    //
    // Read the data from the BMP180 over I2C.  This command starts a
    // temperature measurement.  Then polls until temperature is ready.
    // Then automatically starts a pressure measurement and polls for that
    // to complete. When both measurement are complete and in the local
    // buffer then the application callback is called from the I2C
    // interrupt context.  Polling is done on I2C interrupts allowing
    // processor to continue doing other tasks as needed.
    //
    BMP180DataRead(&g_sBMP180Inst, BMP180AppCallback, &g_sBMP180Inst);
    while(g_vui8DataFlag == 0)
    {
        //
        // Wait for the new data set to be available.
        //
    }

    //
    // Reset the data ready flag.
    //
    g_vui8DataFlag = 0;

    //
    // Get a local copy of the latest temperature data in float format.
    //
    BMP180DataTemperatureGetFloat(&g_sBMP180Inst, &fTemperature);

    //
    // Convert the floats to an integer part and fraction part for easy
    // print.
    //
    i32IntegerPart = (int32_t) fTemperature;
    i32FractionPart =(int32_t) (fTemperature * 1000.0f);
    i32FractionPart = i32FractionPart - (i32IntegerPart * 1000);
    if(i32FractionPart < 0)
    {
        i32FractionPart *= -1;
    }

    //
    // Print temperature with three digits of decimal precision.
    //
    //UARTprintf("Temperature %3d.%03d\t\t", i32IntegerPart, i32FractionPart);

    //
    // Get a local copy of the latest air pressure data in float format.
    //
    BMP180DataPressureGetFloat(&g_sBMP180Inst, &fPressure);

    //
    // Convert the floats to an integer part and fraction part for easy
    // print.
    i32IntegerPart = (int32_t) fPressure;
    i32FractionPart =(int32_t) (fPressure * 1000.0f);
    i32FractionPart = i32FractionPart - (i32IntegerPart * 1000);
    if(i32FractionPart < 0)
    {
        i32FractionPart *= -1;
    }

    //
    // Print Pressure with three digits of decimal precision.
    //
    //UARTprintf("Pressure %3d.%03d\t\t", i32IntegerPart, i32FractionPart);

    //
    // Calculate the altitude.
    //
    fAltitude = 44330.0f * (1.0f - powf(fPressure / 101325.0f,
                                        1.0f / 5.255f));

    //
    // Convert the floats to an integer part and fraction part for easy
    // print.
    //
    i32IntegerPart = (int32_t) fAltitude;
    i32FractionPart =(int32_t) (fAltitude * 1000.0f);
    i32FractionPart = i32FractionPart - (i32IntegerPart * 1000);
    if(i32FractionPart < 0)
    {
        i32FractionPart *= -1;
    }

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
	//UARTBTSend((uint8_t *)"Enter text: \r\n", 14);
	//SysCtlDelay(10000000);
	//UARTBTSend((uint8_t *)"AT\r",3);




	while(1)
	{

	}
}
