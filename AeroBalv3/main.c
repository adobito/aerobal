//*****************************************************************************
//
// pressure_bmp180.c - Example to use of the SensorLib with the BMP180
//
// Copyright (c) 2013 Texas Instruments Incorporated.  All rights reserved.
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
// This is part of revision 1.0 of the EK-TM4C123GXL Firmware Package.
//
//*****************************************************************************

#include <stdint.h>
#include <stdbool.h>
#include <math.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_ints.h"
#include "driverlib/debug.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/pin_map.h"
#include "driverlib/rom.h"
#include "driverlib/sysctl.h"
#include "driverlib/systick.h"
#include "driverlib/uart.h"
#include "utils/uartstdio.h"
#include "sensorlib/hw_bmp180.h"
#include "sensorlib/i2cm_drv.h"
#include "sensorlib/bmp180.h"
#include "drivers/rgb.h"

#include "a.lib/gpio.h"
#include "a.lib/lcd.h"

//*****************************************************************************
//
// Define BMP180 I2C Address.
//
//*****************************************************************************
#define BMP180_I2C_ADDRESS  0x77

//*****************************************************************************
//
//! \addtogroup example_list
//! <h1>pressure Measurement with the BMP180 (pressure_bmp180)</h1>
//!
//! This example demonstrates the basic use of the Sensor Library, TM4C123G
//! LaunchPad and the SensHub BoosterPack to obtain air pressure and
//! temperature measurements with the BMP180 sensor.
//!
//! Connect a serial terminal program to the LaunchPad's ICDI virtual serial
//! port at 115,200 baud.  Use eight bits per byte, no parity and one stop bit.
//! The raw sensor measurements are printed to the terminal.  The RGB LED
//! blinks at 1Hz once the initialization is complete and the example is
//! running.
//
//*****************************************************************************

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
// Called by the NVIC as a SysTick interrupt, which is used to generate the
// sample interval
//
//*****************************************************************************
void
SysTickIntHandler()
{
    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1, GPIO_PIN_1);
    BMP180DataRead(&g_sBMP180Inst, BMP180AppCallback, &g_sBMP180Inst);
    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1, 0x00);
}

//*****************************************************************************
//
// Configure the UART and its pins.  This must be called before UARTprintf().
//
//*****************************************************************************
void
ConfigureUART(void)
{
    //
    // Enable the GPIO Peripheral used by the UART.
    //
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);

    //
    // Enable UART0
    //
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);

    //
    // Configure GPIO Pins for UART mode.
    //
    ROM_GPIOPinConfigure(GPIO_PA0_U0RX);
    ROM_GPIOPinConfigure(GPIO_PA1_U0TX);
    ROM_GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);

    //
    // Use the internal 16MHz oscillator as the UART clock source.
    //
    UARTClockSourceSet(UART0_BASE, UART_CLOCK_PIOSC);

    //
    // Initialize the UART for console I/O.
    //
    UARTStdioConfig(0, 115200, 16000000);
}

void initLCD(){

	lcdInit(GPIO_PORTE,GPIO_PORTC,GPIO_PORTD);
	lcdClear();
	lcdCursorHome();
}

//*****************************************************************************
//
// Main 'C' Language entry point.
//
//*****************************************************************************


int
main(void)
{
    float fTemperature, fPressure, fAltitude;
    int32_t i32IntegerPart;
    int32_t i32FractionPart;

    //
    // Setup the system clock to run at 40 Mhz from PLL with crystal reference
    //
    SysCtlClockSet(SYSCTL_SYSDIV_5 | SYSCTL_USE_PLL | SYSCTL_XTAL_16MHZ |
                       SYSCTL_OSC_MAIN);

    //
    // Initialize the UART.
    //
    //ConfigureUART();

    //
    // Print the welcome message to the terminal.
    //
    //UARTprintf("\033[2JBMP180 Example\n");

    //
    // Set the color to a white approximation.
    //
    g_pui32Colors[RED] = 0x8000;
    g_pui32Colors[BLUE] = 0x8000;
    g_pui32Colors[GREEN] = 0x8000;

    //
    // Initialize RGB driver. Use a default intensity and blink rate.
    //
    RGBInit(0);
    RGBColorSet(g_pui32Colors);
    RGBIntensitySet(0.5f);
    RGBEnable();

    initLCD();
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
    //Initial the GPIO for LED
    //
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_1);
    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1, 0x00);

    //
    // Enable interrupts to the processor.
    //
    IntMasterEnable();


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

    //
    // Wait for initialization callback to indicate reset request is complete.
    //
    while(g_vui8DataFlag == 0)
    {
        //
        // Wait for I2C Transactions to complete.
        //
    }

    //initLCD();
    //
    // Reset the data ready flag
    //
    g_vui8DataFlag = 0;

    //
    // Enable the system ticks at 10 hz.
    //
    SysTickPeriodSet(ROM_SysCtlClockGet() / (10 * 3));
    SysTickIntEnable();
    SysTickEnable();

    //
    // After all the init and config we start blink the LED
    //
    RGBBlinkRateSet(1.0f);

    //
    // Begin the data collection and printing.  Loop Forever.
    //
    while(1)
    {
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

        //
        // Print altitude with three digits of decimal precision.
        //
        //UARTprintf("Altitude %3d.%03d", i32IntegerPart, i32FractionPart);

        //
        // Print new line.
        //
        //UARTprintf("\n");

        lcdCursorHome();
        int temp  = fTemperature ;
        float decimals = fTemperature - (float)((int)fTemperature);
        int d = decimals * 100.0;
        lcdWriteLetter(48+(temp/10));
        lcdWriteLetter(48+(temp%10));
        lcdWriteLetter(46);
        lcdWriteLetter(48+(d/10));
        lcdWriteLetter(48+(d%10));
        lcdWriteLetter(32);
        lcdWriteLetter(0b11011111);
        lcdWriteLetter(0b01000011);

        lcdCursorHomeDown();
        float celsius = (fTemperature*9/5)+32.0;
        /*temp  = celsius ;
        decimals = celsius - (float)((int)celsius);
        d = decimals * 100.0;
        lcdWriteLetter(48+(temp/10));
        lcdWriteLetter(48+(temp%10));
        lcdWriteLetter(46);
        lcdWriteLetter(48+(d/10));
        lcdWriteLetter(48+(d%10));
        lcdWriteLetter(32);
        lcdWriteLetter(0b11011111);
        lcdWriteLetter(0b01000110);
        s*/

        //lcdWriteCommand(0x80+0x14);
        lcdCursorHomeDown();
        celsius = fPressure/1000.0;
        temp  = celsius ;
        decimals = celsius - (float)((int)celsius);
        d = decimals * 100.0;
        lcdWriteLetter(48+(temp/100));
        lcdWriteLetter(48+(temp/10)-((temp/100)*10));
        lcdWriteLetter(48+(temp%10));
        lcdWriteLetter(46);
        lcdWriteLetter(48+(d/10));
        lcdWriteLetter(48+(d%10));
        lcdWriteLetter(32);
        lcdWriteLetter(0b01101011);
        lcdWriteLetter(0b01010000);
        lcdWriteLetter(0b01100001);




        //
        // Delay to keep printing speed reasonable. About 100 milliseconds.
        //
        SysCtlDelay(ROM_SysCtlClockGet() / (10 * 3));

    }//while end
}

