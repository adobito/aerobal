/*
 * ABMain.h
 *
 *  Created on: Nov 22, 2013
 *      Author: Administrator
 */

#ifndef ABMAIN_H_
#define ABMAIN_H_

///////////////////////////////////////////////////
// Includes 									 //
///////////////////////////////////////////////////
#include <stdint.h>
#include <stdbool.h>
#include <math.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_gpio.h"
//#include "inc/hw_ints.h"
#include "driverlib/debug.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/pin_map.h"
#include "driverlib/rom.h"
#include "driverlib/sysctl.h"
#include "driverlib/systick.h"
#include "driverlib/uart.h"
//#include "utils/uartstdio.h"
#include "sensorlib/hw_bmp180.h"
#include "sensorlib/i2cm_drv.h"
#include "sensorlib/bmp180.h"

//////////////////////////////
//Self-Made Libraries
#include "gpio.h"
#include "lcd.h"
#include "dht11.h"
#include "loadCell.h"
#include "bt.h"
#include "servo.h"
#include "anemometer.h"
#include "ABUI.h"


///////////////////////////////////////////////////

///////////////////////////////////////////////////
// Pressure Sensor								 //
///////////////////////////////////////////////////
//| - Defs
#define BMP180_I2C_ADDRESS  0x77

//| - Vars
volatile uint_fast8_t g_vui8DataFlag;
float ABBMPTemperature;
float ABBMPPressure;

//| - Functions
void ABBMPInit();
void ABBMPUpdate();
float ABBMPGetTemperature();
float ABBMPGetPressure();

///////////////////////////////////////////////////
// Load Cells									 //
///////////////////////////////////////////////////
//| - Defs
float _ABLoadCellLiftUp;
float _ABLoadCellLiftDown;
float _ABLoadCellDragFront;
float _ABLoadCellDragBack ;
float _ABWindVaneAngle;

//| - Functions
void ABLoadCellRefresh(int _ABLoadCellUnits);
void ABLoadCellOutput();
///////////////////////////////////////////////////

///////////////////////////////////////////////////
// LCD											 //
///////////////////////////////////////////////////
//| - Defs
//None

//| - Vars
//None

//| - Functions
void ABLCDInit();

///////////////////////////////////////////////////
// DHT11										 //
///////////////////////////////////////////////////
//| - Defs
//None

//| - Vars
//None

//| - Functions
void ABDHT11Refresh();
void ABDHT11Output();

///////////////////////////////////////////////////
// Bluetooth											 //
///////////////////////////////////////////////////
//| - Defs
//None

//| - Vars
//None

//| - Functions;
void UARTIntHandler(void);
void ABUARTInit();
///////////////////////////////////////////////////
// Servo											 //
///////////////////////////////////////////////////
//| - Defs
//None

//| - Vars
//None

//| - Functions
void ABServoRefresh(int angle);
///////////////////////////////////////////////////




///////////////////////////////////////////////////
// Helper Functions								 //
///////////////////////////////////////////////////
//| - Defs
//None

//| - Vars
//None

//| - Functions
void ABStateInvalid(char state);
void ABSysDelay(int milliseconds);
void ABStandardSysDelay(int delay);
///////////////////////////////////////////////////
#endif /* ABMAIN_H_ */
