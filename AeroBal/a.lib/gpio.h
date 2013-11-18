/*
 * helloLCD.h
 *
 *  Created on: Sep 29, 2013
 *      Author: Administrator
 */

#ifndef HELLOLCD_H_
#define HELLOLCD_H_

#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_types.h"
#include "inc/tm4c123gh6pm.h"
#include "driverlib/sysctl.h"
#include "driverlib/interrupt.h"


#define	GPIO_PORTA 0x40004000
#define GPIO_PORTB 0x40005000
#define GPIO_PORTC 0x40006000
#define GPIO_PORTD 0x40007000
#define GPIO_PORTE 0x40024000
#define GPIO_PORTF 0x40025000
#define GPIO_OFFSET_DATA 0x000003FC
#define GPIO_OFFSET_DIRECTION 0x00000400
#define GPIO_OFFSET_INTERRUPT_SENSE 0x00000404
#define GPIO_OFFSET_INTERRUPT_BOTH_EDGES 0x00000408
#define GPIO_OFFSET_INTERRUPT_EVENT 0x40C
#define GPIO_OFFSET_INTERRUPT_MASK  0x00000410
#define GPIO_OFFSET_INTERRUPT_RAW_STATUS  0x00000414
#define GPIO_OFFSET_INTERRUPT_MASKED_STATUS  0x00000418
#define GPIO_OFFSET_INTERRUPT_CLEAR  0x0000041C
#define GPIO_OFFSET_ALTERNATE_FUNCTION_SELECT  0x00000420
#define GPIO_OFFSET_2MA_DRIVE_SELECT  0x00000500
#define GPIO_OFFSET_4MA_DRIVE_SELECT  0x00000504
#define GPIO_OFFSET_8MA_DRIVE_SELECT  0x00000508
#define GPIO_OFFSET_OPEN_DRAIN_SELECT  0x0000050C
#define GPIO_OFFSET_PULL_UP_SELECT  0x00000510
#define GPIO_OFFSET_PULL_DOWN_SELECT  0x00000514
#define GPIO_OFFSET_SLEW_RATE_CONTROL  0x00000518
#define GPIO_OFFSET_DIGITAL_ENABLE  0x0000051C
#define GPIO_OFFSET_LOCK  0x00000520
#define GPIO_OFFSET_COMMIT  0x00000524
#define GPIO_OFFSET_ANALOG_SELECT  0x00000528
#define GPIO_OFFSET_PORT_CONTROL  0x0000052C
#define GPIO_OFFSET_ADC_CONTROL  0x00000530
#define GPIO_OFFSET_DMA_CONTROL  0x00000534
#define GPIO_OFFSET_PERIPHERAL_ID_4  0x00000FD0
#define GPIO_OFFSET_PERIPHERAL_ID_5  0x00000FD4
#define GPIO_OFFSET_PERIPHERAL_ID_6  0x00000FD8
#define GPIO_OFFSET_PERIPHERAL_ID_7  0x00000FDC
#define GPIO_OFFSET_PERIPHERAL_ID_0  0x00000FE0
#define GPIO_OFFSET_PERIPHERAL_ID_1  0x00000FE4
#define GPIO_OFFSET_PERIPHERAL_ID_2  0x00000FE8
#define GPIO_OFFSET_PERIPHERAL_ID_3  0x00000FEC
#define GPIO_OFFSET_PRIMECELL_0  0x00000FF0
#define GPIO_OFFSET_PRIMECELL_1  0x00000FF4
#define GPIO_OFFSET_PRIMECELL_2  0x00000FF8
#define GPIO_OFFSET_PRIMECELL_3  0x00000FFC

//////////////////////////////////////////
// API Layer 0
void gpioSetMasterEnable(uint32_t port);
void gpioHelperInterruptMasterEnable();
void gpioSetInterruptEnable(uint32_t port);
void gpioSetData(uint32_t port, uint32_t pins, uint32_t data);
uint32_t gpioGetData(uint32_t port, uint32_t pins);
void gpioSetDirection(uint32_t port, uint32_t pins, uint32_t direction);
void gpioSetDigitalEnable(uint32_t port, uint32_t pins, uint32_t enable);
void gpioSetInterruptSense(uint32_t port, uint32_t pins, uint32_t type);
void gpioSetInterruptBothEdges(uint32_t port, uint32_t pins, uint32_t control);
void gpioSetInterruptEvent(uint32_t port, uint32_t pins, uint32_t edgeType);
void gpioSetInterruptMaskDisable(uint32_t port, uint32_t pins, uint32_t disable);
uint32_t gpioGetInterruptRawStatus(uint32_t port, uint32_t pins);
uint32_t gpioGetInterruptMaskedStatus(uint32_t port, uint32_t pins);
void gpioSetInterruptClear(uint32_t port, uint32_t pins, uint32_t clear);

//Alternate Function

//////////////////////////////////////////
// API Layer 1

#endif /* HELLOLCD_H_ */
