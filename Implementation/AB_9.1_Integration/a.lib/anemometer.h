/*
 * anemometer.h
 *
 *  Created on: Nov 26, 2013
 *      Author: Administrator
 */

#ifndef ANEMOMETER_H_
#define ANEMOMETER_H_

#include "gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/timer.h"
#include "inc/hw_memmap.h"

#define ANEMOMETER_BUFFER_SIZE 25
#define ANEMOMETER_FREQUENCY 100.0
#define ANEMOMETER_DIAMETER 0.101
#define ANEMOMETER_PI 3.14159265358979323846
#define ANEMOMETER_CIRCLE_LENGTH (ANEMOMETER_DIAMETER * ANEMOMETER_PI)
#define ANEMOMETER_MS_TO_KMH (60.0*60.0/1000.0)
#define ANEMOMETER_KMH_TO_MIH 1.0/1.609344
#define ANEMOMETER_CIRCLE_STEPS 5



float anemometerGetVelocityInKMH();
float anemometerGetVelocityInMIH();
void anemometerEnable();
void anemometerDisable();
void anemometerInterruptTimer(void);
void anemometerInterruptGPIO(void);
void anemometerSetup(void);


#endif /* ANEMOMETER_H_ */
