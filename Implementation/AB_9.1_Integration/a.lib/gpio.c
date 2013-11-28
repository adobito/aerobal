/*
 * gpio.c
 * v1.0
 * Created on: Sep 30, 2013
 *      Author: Anthony
 */
#include "gpio.h"


//////////////////////////////////////////
// API Layer 0

/* Enables the use of a GPIO port.
 *
 * - port : the port to enable.
 *
 * Default:
 * - Deactivated.
 */
void gpioSetMasterEnable(uint32_t port){
	switch(port){
		case GPIO_PORTA: SYSCTL_RCGC2_R |= SYSCTL_RCGC2_GPIOA; break ;
		case GPIO_PORTB: SYSCTL_RCGC2_R |= SYSCTL_RCGC2_GPIOB; break ;
		case GPIO_PORTC: SYSCTL_RCGC2_R |= SYSCTL_RCGC2_GPIOC; break ;
		case GPIO_PORTD: SYSCTL_RCGC2_R |= SYSCTL_RCGC2_GPIOD; break ;
		case GPIO_PORTE: SYSCTL_RCGC2_R |= SYSCTL_RCGC2_GPIOE; break ;
		case GPIO_PORTF: SYSCTL_RCGC2_R |= SYSCTL_RCGC2_GPIOF; break ;
		default: break ;
	}
	//Dummy Read.
	uint32_t ui32Loop = SYSCTL_RCGC2_R;
	return ;
}

/* Helper Function created for enabling interrupts
 * globally in microcontroller.
 *
 */
void gpioHelperInterruptMasterEnable(){
	IntMasterEnable();
}

/* Enables interrupts on the corresponding
 * given port.
 *
 * - port - the port to enable interrupts.
 */
void gpioSetInterruptEnable(uint32_t port){
	switch(port){
		case GPIO_PORTA: IntEnable(INT_GPIOA); break ;
		case GPIO_PORTB: IntEnable(INT_GPIOB); break ;
		case GPIO_PORTC: IntEnable(INT_GPIOC); break ;
		case GPIO_PORTD: IntEnable(INT_GPIOD); break ;
		case GPIO_PORTE: IntEnable(INT_GPIOE); break ;
		case GPIO_PORTF: IntEnable(INT_GPIOF); break ;
		default: break ;
	}
}

/* Sets the data of a port on the desired pins.
 *
 */
void gpioSetData(uint32_t port, uint32_t pins, uint32_t data){
	HWREG(port | GPIO_OFFSET_DATA) = (HWREG(port | GPIO_OFFSET_DATA) & ~pins) | (data&pins);
}

/*
 * Returns the data of the pins of a port.
 */
uint32_t gpioGetData(uint32_t port, uint32_t pins){
	return HWREG(port|GPIO_OFFSET_DATA) & pins;
}




/* Sets the direction of the indicated port pins. Set the bit
 * '1' for output and '0' for input.
 */
void gpioSetDirection(uint32_t port, uint32_t pins, uint32_t direction){
	//offset =
	HWREG(port | GPIO_OFFSET_DIRECTION) = (HWREG(port | GPIO_OFFSET_DIRECTION) & ~pins) | (direction & pins);
}

/* Forced Set the digital functionality of the port pins.
 * To retain previous value use gpioSetDigitalEnableOnPins
 * - '1' to enable digital
 * - '0' to disable.
 *
 * Default:
 * - 0 for all pins.
 */
void gpioSetDigitalEnable(uint32_t port, uint32_t pins, uint32_t enable){
	HWREG(port | GPIO_OFFSET_DIGITAL_ENABLE) = (HWREG(port | GPIO_OFFSET_DIGITAL_ENABLE) & ~pins) | (enable&pins);
}

/* Sets the GPIO port to sense for interrupts either levels
 * or edges.
 *
 * '0' - to detect edges.
 * '1' - to detect levels.
 *
 * Default:
 * - '0' for all pins.
 *
 */
void gpioSetInterruptSense(uint32_t port, uint32_t pins, uint32_t type){
	HWREG(port | GPIO_OFFSET_INTERRUPT_SENSE) = (HWREG(port | GPIO_OFFSET_INTERRUPT_SENSE) & ~pins) | (type&pins);
}

/* Allows for both edges causing interrupts. Set to '0' to
 * delegate control to the GPIO Interrupt Event Register.
 *
 * '0' - delegate control to GPIO Interrupt Event Register.
 * '1' - set to detect both edges on the port.
 *
 */
void gpioSetInterruptBothEdges(uint32_t port, uint32_t pins, uint32_t control){
	HWREG(port | GPIO_OFFSET_INTERRUPT_BOTH_EDGES) = (HWREG(port | GPIO_OFFSET_INTERRUPT_BOTH_EDGES) & ~pins) | (control&pins);
}

/* Sets the interrupts to be caused at falling or rising edges IF
 * the GPIO Interrupt Both Edges Register has delegated control
 * to this register.
 *
 * * Needs control delegation from InterruptBothEdges Register.
 *
 * '0' - detect falling edges
 * '1' - detect rising edges
 *
 */
void gpioSetInterruptEvent(uint32_t port, uint32_t pins, uint32_t edgeType){
	HWREG(port | GPIO_OFFSET_INTERRUPT_EVENT) = (HWREG(port | GPIO_OFFSET_INTERRUPT_EVENT) & ~pins) | (edgeType&pins);
}

/* Sets the interrupts from the port to NOT be masked to the
 * controller allowing detection of interrupt.
 *
 * '1' - disable the mask for pin.
 * '0' - enable the mask for pin.
 *
 */
void gpioSetInterruptMaskDisable(uint32_t port, uint32_t pins, uint32_t disable){
	HWREG(port | GPIO_OFFSET_INTERRUPT_MASK) = (HWREG(port | GPIO_OFFSET_INTERRUPT_MASK) & ~pins) | (disable&pins);
}

/* Returns the contents of the register in the interrupt raw status
 * which determines which PIN has caused an interrupt. These bits are
 * cleared by setting the InterruptClear register bits to 1.
 *
 * '1' - this pin has caused an interrupt.
 * '0' - this pin has not caused an interrupt.
 *
 */
uint32_t gpioGetInterruptRawStatus(uint32_t port, uint32_t pins){
	return HWREG(port | GPIO_OFFSET_INTERRUPT_RAW_STATUS) & (pins);
}

/* Contrary to the raw interrupt status, the masked interrupt
 * status shows whether the pin has caused an interrupt if it
 * is unmasked. If masked, then this port does not register
 * the interrupt.
 *
 * '1' - if the pin has caused an interrupt and is unmasked.
 * '0' - if the pin has not caused an interrupt or is unmasked.
 *
 */
uint32_t gpioGetInterruptMaskedStatus(uint32_t port, uint32_t pins){
	return HWREG(port | GPIO_OFFSET_INTERRUPT_MASKED_STATUS) & (pins) ;
}

/* Clears the raw interrupt status bits corresponding
 * to the bit number in this register.
 *
 * '1' - to clear the corresponding RIS bit.
 * '0' - to not clear.
 */
void gpioSetInterruptClear(uint32_t port, uint32_t pins, uint32_t clear){
	HWREG(port | GPIO_OFFSET_INTERRUPT_CLEAR) = (HWREG(port | GPIO_OFFSET_INTERRUPT_CLEAR) & ~pins) | (clear&pins) ;
}


//////////////////////////////////////////
// API Layer 1
