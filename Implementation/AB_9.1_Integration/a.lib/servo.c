/*
 * servo.c
 *
 *  Created on: Nov 20, 2013
 *      Author: JJLB
 */
/****************************************************/
/*					Instructions					*/
/****************************************************/
/*set a timer for timer() function
 *
 *
 *
 */
#include <stdint.h>
#include <stdbool.h>

#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/timer.h"
#include "inc/hw_memmap.h"
#include "driverlib/rom.h"
#include "inc/tm4c123gh6pm.h"
#include "servo.h"

void servoSetup();
void servosetPosition(int degrees);
int servogetPosition();
void servosetTimer();
void servoTimer();
void servoDisableTimer();

/****************************************************/
/*	Variable Definitions							*/
/****************************************************/

#define SERVO_ULPERIOD 9500000

int down_time = 40;
int currentPosition = 0;
int _servoState = 1;

/****************************************************/
/*				Function Definitions				*/
/****************************************************/

/****Main Functions****/

void servoSetup(){
	_servoState = 1;
	gpioSetMasterEnable(GPIO_PORTB);
	gpioSetDirection(GPIO_PORTB,0x04,0x04);
	gpioSetDigitalEnable(GPIO_PORTB,0x04,0x04);
	gpioSetData(GPIO_PORTB,0x04,00);

}

void servosetTimer(){
	IntMasterEnable();
	SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);

	SysCtlClockSet(SYSCTL_SYSDIV_1 | SYSCTL_USE_OSC | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);

	TimerConfigure(TIMER0_BASE, TIMER_CFG_ONE_SHOT);

	TimerLoadSet(TIMER0_BASE, TIMER_A, SysCtlClockGet());

	IntEnable(INT_TIMER0A);

	TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);

	TimerEnable(TIMER0_BASE, TIMER_A);

}
void servosetPosition(int degrees){
	currentPosition =(int)(4.2222*(degrees) + 440);
}

void servoSetupPWM(){
	//SysCtlClockSet(SYSCTL_SYSDIV_10|SYSCTL_USE_OSC|SYSCTL_XTAL_16MHZ|SYSCTL_OSC_MAIN);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
	GPIOPinConfigure(GPIO_PB2_T3CCP0);
	GPIOPinTypeTimer(GPIO_PORTB_BASE, GPIO_PIN_2);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER3);
	TimerConfigure(TIMER3_BASE, TIMER_CFG_SPLIT_PAIR|TIMER_CFG_A_PWM);
	TimerLoadSet(TIMER3_BASE, TIMER_A, SERVO_ULPERIOD-1);
}
void servosetPositionPWM(int _servoDegrees, int _servoSpeed){
	long dutyCycle;
	SysCtlClockSet(SYSCTL_SYSDIV_10|SYSCTL_USE_OSC|SYSCTL_XTAL_16MHZ|SYSCTL_OSC_MAIN);

	dutyCycle = 9299600+_servoDegrees*(2250/180);

	TimerEnable(TIMER3_BASE, TIMER_A);
	TimerMatchSet(TIMER3_BASE, TIMER_A, dutyCycle);

	SysCtlDelay(1000000-(7500*(_servoSpeed%100)));
	TimerDisable(TIMER3_BASE, TIMER_A);
	SysCtlClockSet(SYSCTL_SYSDIV_1|SYSCTL_USE_OSC|SYSCTL_XTAL_16MHZ|SYSCTL_OSC_MAIN);
}
int servogetPosition(){
	return currentPosition;
}

void servoDisableTimer(){
	IntDisable(INT_GPIOB);
	IntMasterDisable();
	IntDisable(INT_TIMER0A);
	TimerDisable(TIMER0_BASE, TIMER_A);
	TimerIntDisable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);

}
/**** Helper Functions *****/

void servoTimer(){
	TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
	//some code here....
	gpioSetData(GPIO_PORTB | GPIO_OFFSET_DATA, 0x04,HWREG(GPIO_PORTB|GPIO_OFFSET_DATA) ^ 0x04) ;

	if(_servoState){
		_servoState = 0;
		TimerLoadSet(TIMER0_BASE, TIMER_A, SysCtlClockGet()/currentPosition);
		TimerEnable(TIMER0_BASE, TIMER_A);
	}
	else{
		_servoState = 1 ;
		TimerLoadSet(TIMER0_BASE, TIMER_A, SysCtlClockGet()/down_time);
		TimerEnable(TIMER0_BASE, TIMER_A);
	}
}
