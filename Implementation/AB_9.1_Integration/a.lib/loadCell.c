/*
 * LoadCell.c
 *
 *  Created on: Nov 18, 2013
 *      Author: JJLB
 */
/****************************************************/
/*					Instructions					*/
/****************************************************/
/*
 *
 *
 *
 */
#include "loadCell.h"
#include "driverlib/adc.h"
#include <stdbool.h>
#include <stdint.h>
#include "inc/hw_memmap.h"

#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/uart.h"
#include "utils/uartstdio.h"
#include "driverlib/interrupt.h"
#include "driverlib/rom.h"
#include "driverlib/timer.h"


void loadCellSetup();
void loadCellgetData();
float loadCellgetValues(int sensorIndex,int units);
/****************************************************/
/*	Variable Definitions							*/
/****************************************************/

int MAX_VAL = 1000;
int values[1000];
int counter = 0;
uint32_t pui32ADC0Value[7];

/****************************************************/
/*				Function Definitions				*/
/****************************************************/

void initializeADC(void){

	//SysCtlClockSet(SYSCTL_SYSDIV_10 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);

	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);

	GPIOPinTypeADC(GPIO_PORTE_BASE, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4);

	ADCSequenceConfigure(ADC0_BASE, 0, ADC_TRIGGER_PROCESSOR, 0);
							//[PE0, PE4,PE1, PE2 PE3]
	HWREG(ADC0_BASE | 0x040) = 0x00039210; //Port Selection Port 9 / Port 2 / Port 1 / Port 0
	HWREG(ADC0_BASE | 0x044) = 0b00000000000001100000000000000000; //Sample until port 3
	HWREG(ADC0_BASE | 0x050) = 0x00000000; //Save Samples in FIFOn

	//HWREG(ADC0_BASE | 0x030)=0x6;	//Set Sample Averaging Control to 64x Hardware Oversampling
	//HWREG(ADC0_BASE | 0x024)=0x8;	//Set Phase Shift by 180 degree

	ADCSequenceEnable(ADC0_BASE, 0);

	ADCIntClear(ADC0_BASE, 0);
}


float averageValues(int sensorIndex){
	float average = 0;
	while(counter < MAX_VAL){
		values[counter++] = pui32ADC0Value[sensorIndex];
		loadCellgetData();
	}
		counter = 0;
		int i;
		for( i = 0; i < MAX_VAL; i++){
			average += values[i];
		}
		average /= MAX_VAL;

		return average;
}/****Main Functions****/
void loadCellSetup(){
	initializeADC();
}

void loadCellgetData(){

	SysCtlClockSet(SYSCTL_SYSDIV_10 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);
	//
	// Trigger the ADC conversion.
	//
	ADCProcessorTrigger(ADC0_BASE, 0);

	//
	// Wait for conversion to be completed.
	//
	while(!ADCIntStatus(ADC0_BASE, 0, false))
	{
	}
	//
	// Clear the ADC interrupt flag.
	//
	ADCIntClear(ADC0_BASE, 0);

	//
	// Read ADC Value.
	//
	ADCSequenceDataGet(ADC0_BASE, 0, pui32ADC0Value);
	SysCtlClockSet(SYSCTL_SYSDIV_1 | SYSCTL_USE_OSC | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);
	SysCtlDelay(10);
}

float loadCellgetWeight(float average,int units,int sensorIndex){
	switch(sensorIndex){
			case  LOADCELL_LIFT_UP:
				if(units == OUNCES){
					return ((average - 148.81)/4.7143); //SG0
				}
				else{
					return ((average - 148.81)/4.7143)/16; //SG0
				}
			//break;
			case  LOADCELL_LIFT_DOWN:
				if(units == OUNCES){
					return ((average-125.22)/12.302); //SG1
				}
				else{
					return ((average-125.22)/12.302)/16; //SG1
				}
			//break;
			case  LOADCELL_DRAG_FRONT:
				if(units == OUNCES){
					return ((average-146)/5.9375); //SG2
				}
				else{
					return ((average-146)/5.9375)/16; //SG2
				}
			//break;
			case  LOADCELL_DRAG_BACK:
				if(units == OUNCES){
					return ((average-159.3)/7.7626); //SG3
				}
				else{
					return ((average-159.3)/7.7626)/16; //SG3
				}
			case WIND_VANE:
				return average ;
			//break;
	}
	return -1;
}

float loadCellgetValues(int sensorIndex, int units){
	switch(sensorIndex){
		case  LOADCELL_LIFT_UP:
			loadCellgetData();
			return loadCellgetWeight(averageValues(sensorIndex),units,sensorIndex);
		//break;
		case  LOADCELL_LIFT_DOWN:
			loadCellgetData();
			return loadCellgetWeight(averageValues(sensorIndex),units,sensorIndex);
		//break;
		case  LOADCELL_DRAG_FRONT:
			loadCellgetData();
			return loadCellgetWeight(averageValues(sensorIndex),units,sensorIndex);
		//break;
		case  LOADCELL_DRAG_BACK:
			loadCellgetData();
			return loadCellgetWeight(averageValues(sensorIndex),units,sensorIndex);
		case WIND_VANE:
			loadCellgetData();
			return pui32ADC0Value[4];
			//return averageValues(sensorIndex);
			//break;
	}
	return -1;
}

/**** Helper Functions *****/









