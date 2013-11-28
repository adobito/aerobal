/*
 * servo.h
 *
 *  Created on: Nov 20, 2013
 *      Author: JJLB
 */

#ifndef SERVO_H_
#define SERVO_H_

/****TI Libraries****/
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/timer.h"
#include "inc/hw_memmap.h"
#include "driverlib/rom.h"
#include "inc/tm4c123gh6pm.h"
#include "driverlib/systick.h"
#include "servo.h"


//***** TO DO *****//

/****Self Made Libraries****/
#include "gpio.h"

/****************************************************/
/*	Constant Definitions							*/
/****************************************************/

#define AB_SERVO_OPEN 180
#define AB_SERVO_CLOSED 90

/****************************************************/
/*	Variable Definitions							*/
/****************************************************/
//NONE

/****************************************************/
/*	Function Definitions							*/
/****************************************************/

/****Main Functions****/
void servoSetup();
void servoSetupPWM();
void servosetPosition(int degrees);
void servosetPositionPWM(int _servoDegrees, int _servoSpeed);
int servogetPosition();
void servosetTimer();
void servoDisableTimer();

//****Helper Functions****/
#endif /* SERVO_H_ */
