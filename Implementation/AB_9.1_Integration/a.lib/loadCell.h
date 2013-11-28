/*
 * loadCell.h
 *
 *  Created on: Nov 18, 2013
 *      Author: JJLB
 */

#ifndef LOADCELL_H_
#define LOADCELL_H_

/****TI Libraries****/
#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "driverlib/interrupt.h"
#include "driverlib/rom.h"
#include "driverlib/timer.h"
#include "driverlib/sysctl.h"
//***** TO DO *****//

/****Self Made Libraries****/
#include "gpio.h"
#include "lcd.h"

/****************************************************/
/*	Constant Definitions							*/
/****************************************************/
#define SG0	0
#define SG1	1
#define SG2	2
#define SG3	3
#define WIND_VANE 				4 //PE0
#define LOADCELL_LIFT_UP		0 //PE3
#define LOADCELL_LIFT_DOWN  	1 //PE2
#define LOADCELL_DRAG_FRONT  	2 //PE1
#define LOADCELL_DRAG_BACK		3 //PE4
#define OUNCES 0
#define POUNDS 1

/****************************************************/
/*	Variable Definitions							*/
/****************************************************/
int sensorIndex;

/****************************************************/
/*	Function Definitions							*/
/****************************************************/

/****Main Functions****/
void loadCellSetup();
void loadCellgetData();
float loadCellgetValues(int sensorIndex,int units);

//****Helper Functions****/

#endif /* LOADCELL_H_ */
