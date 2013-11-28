/*
 * dht11.h
 *
 * Created on: Nov 6, 2013
 * Author: Juan Lebron
 */

#ifndef DHT11_H_
#define DHT11_H_

/****TI Libraries****/
#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "driverlib/interrupt.h"
#include "driverlib/rom.h"
#include "driverlib/timer.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
//***** TO DO *****//

/****Self Made Libraries****/
#include "gpio.h"
#include "lcd.h"

/****************************************************/
/*	Constant Definitions							*/
/****************************************************/
#define REQUEST 	 0
#define REQUEST18MS  0
#define REQUEST40US  1

#define RESPONSE  	  1
#define RESPONSE54US  0
#define RESPONSE80US  1

#define CHECKIFRESPONSEISOK 	 2
#define CHECKIFRESPONSEISOKSTEP  0

#define DATAREADING 	 3
#define DATAREADINGINITSTEP	 0
#define DATAREADINGSTEP  1

#define DATASIZE 40;

/****************************************************/
/*	Variable Definitions							*/
/****************************************************/
//NONE

/****************************************************/
/*	Function Definitions							*/
/****************************************************/

/****Main Functions****/
int dhtIsActive();
void dhtSetActive();
void dhtSetup();
void dht11init();
void dht11getData();
int dht11getHumidity();
int dht11getTemperature();
int dht11dataValidationStatus();
int dht11RequestFinished();
//****Helper Functions****/

void dht11ClockSetup();

#endif /* DHT11_H_ */
