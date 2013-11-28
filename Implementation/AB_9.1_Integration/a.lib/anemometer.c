/*
 * anemometer.c
 *
 *  Created on: Nov 26, 2013
 *      Author: Administrator
 */
#include "anemometer.h"

char _anemometerEnable = 0;
int _anemometerIntCount;
int _anemometerMilliseconds ;
int _anemometerCount = 0;
float _anemometerVelocityInMS;
float _anemometerVelocityInKMH;

float anemometerGetVelocityInKMH(){
	return _anemometerVelocityInKMH;
}

float anemometerGetVelocityInMIH(){
	return _anemometerVelocityInKMH * ANEMOMETER_KMH_TO_MIH;
}
void anemometerEnable(){
	_anemometerEnable = 1;
	//Activate Timer.
	TimerEnable(TIMER2_BASE, TIMER_A);
}

void anemometerDisable(){
	_anemometerEnable = 0;
	//Deactivate Timer.
}

void anemometerInterruptTimer(void)
{
	TimerIntClear(TIMER2_BASE, TIMER_TIMA_TIMEOUT);
	if(!_anemometerEnable){
		TimerDisable(TIMER2_BASE,TIMER_A);
		return ;
	}
	_anemometerMilliseconds ++ ;
	if(_anemometerMilliseconds >= ANEMOMETER_FREQUENCY*5)
	{
		_anemometerVelocityInKMH = 0;
		_anemometerVelocityInMS = 0;
		//Update variables.

	}
}

void anemometerInterruptGPIO(void)
{

	_anemometerIntCount++;

	if((_anemometerIntCount > ANEMOMETER_CIRCLE_STEPS) && _anemometerEnable){
		_anemometerVelocityInMS = ANEMOMETER_CIRCLE_LENGTH/((float)_anemometerMilliseconds/ANEMOMETER_FREQUENCY) ; //Meters by seconds
		_anemometerVelocityInKMH = _anemometerVelocityInMS * ANEMOMETER_MS_TO_KMH; //KM by hour
		_anemometerMilliseconds = 0;
	}
	gpioSetInterruptClear(GPIO_PORTF,0x10,0x10);
}

void anemometerSetup(){
	gpioSetMasterEnable(GPIO_PORTF);
	gpioSetDirection(GPIO_PORTF,0x10,0x00);
	gpioSetDigitalEnable(GPIO_PORTF,0x10,0x10);
	gpioSetData(GPIO_PORTF,0x10,0x00);

	//Enable Interrupts:
	gpioHelperInterruptMasterEnable();
	gpioSetInterruptEnable(GPIO_PORTF);
	gpioSetInterruptMaskDisable(GPIO_PORTF,0x10,0x10);
	gpioSetInterruptBothEdges(GPIO_PORTF,0x10,0x00);
	gpioSetInterruptEvent(GPIO_PORTF,0x10,0x00);
	//Interrupts Enabled.

	//Enabling Timers.
	SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER2); //Enabling the timer peripheral globally.
	SysCtlClockSet(SYSCTL_SYSDIV_1 | SYSCTL_USE_OSC | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ); //Setting the system clock.
	TimerConfigure(TIMER2_BASE, TIMER_CFG_PERIODIC);  //Configuring for periodicity Or whatever.
	TimerLoadSet(TIMER2_BASE, TIMER_A, SysCtlClockGet() / 100); //Setting the execution rate.
	IntEnable(INT_TIMER2A); //Enabling the interrupts for timers.
	TimerIntEnable(TIMER2_BASE, TIMER_TIMA_TIMEOUT); //Enabling the interrupts in timers.
	//TODO: Check logic in here for integration.

}

