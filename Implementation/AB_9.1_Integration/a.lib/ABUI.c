/*
 * ABUI.c
 *
 *  Created on: Nov 22, 2013
 *      Author: Administrator
 */

#include "ABUI.h"

char _ABUIButtonB0_UP_State = 0;
char _ABUIButtonB3_DOWN_State = 0;
char _ABUIButtonB4_ENTER_State = 0;
char _ABUIButtonB5_CANCEL_State = 0;
char _ABUIButtonB6_MENU_State = 0;
char _ABUIButtonB7_OPTION_State = 0;




void ABUIButtonInterrupt(){

	ABUIButtonDisable();
	int ABUIButton_RIS = gpioGetInterruptRawStatus(GPIO_PORTB, ABUI_ENABLED_BUTTONS);
	ABStandardSysDelay(AB_UI_BUTTON_STANDARD_DELAY) ;


	//TODO: Assign Priorities.
	if(ABUIButton_RIS & _ABUIButtonB0_UP){
		//Button B0 UP
		//Change main state and execute.
		ABStateMachineChangeState(_ABUIButtonB0_UP_State);
	}
	else if(ABUIButton_RIS & _ABUIButtonB3_DOWN){
		//Button B3 DOWN
		//Change main state and execute.
		ABStateMachineChangeState(_ABUIButtonB3_DOWN_State);
	}
	else if(ABUIButton_RIS & _ABUIButtonB4_ENTER){
		//Button B4 ENTER
		//Change main state and execute.
		ABStateMachineChangeState(_ABUIButtonB4_ENTER_State);

	}
	else if(ABUIButton_RIS & _ABUIButtonB5_CANCEL){
		//Button B5 CANCEL
		//Change main state and execute.
		ABStateMachineChangeState(_ABUIButtonB5_CANCEL_State);

	}
	else if(ABUIButton_RIS & _ABUIButtonB6_MENU){
		//Button B6 MENU
		//Change main state and execute.
		ABStateMachineChangeState(_ABUIButtonB6_MENU_State);

	}
	else if(ABUIButton_RIS & _ABUIButtonB7_OPTION){
		//Button B7 OPTION
		//Change main state and execute.
		ABStateMachineChangeState(_ABUIButtonB7_OPTION_State);
	}

	ABStandardSysDelay(AB_UI_BUTTON_STANDARD_DELAY);
	gpioSetInterruptClear(GPIO_PORTB,ABUI_ENABLED_BUTTONS,ABUI_ENABLED_BUTTONS);

	//TODO: RUN????
	ABStateMachineRun();

	////////////////////////////////////
	// TODO: DISABLE PORT INTERRUPT?? //
	////////////////////////////////////
	//GPIOIntDisable(GPIO_PORTB,0xF9);
}

void ABUIButtonDisable(){
	gpioSetInterruptMaskDisable(GPIO_PORTB,ABUI_ENABLED_BUTTONS,0x00);
}

void ABUIButtonEnable(){
	gpioSetInterruptClear(GPIO_PORTB,ABUI_ENABLED_BUTTONS,ABUI_ENABLED_BUTTONS);
	gpioSetInterruptMaskDisable(GPIO_PORTB,ABUI_ENABLED_BUTTONS,ABUI_ENABLED_BUTTONS);
}

void ABUIButtonSetup(){
	//SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
	//GPIODirModeSet(GPIO_PORTB,0xF9,0x00);
	//IntEnable(INT_GPIOB);
	//GPIOIntEnable(GPIO_PORTB,0xF9);



	gpioSetMasterEnable(GPIO_PORTB);
	//ABUIUnlockPB7();
	gpioSetDirection(GPIO_PORTB,ABUI_ENABLED_BUTTONS,0x00);
	gpioSetDigitalEnable(GPIO_PORTB,ABUI_ENABLED_BUTTONS,ABUI_ENABLED_BUTTONS);

	IntMasterEnable();
	IntEnable(INT_GPIOB);
	gpioSetInterruptBothEdges(GPIO_PORTB,ABUI_ENABLED_BUTTONS,0x00);
	gpioSetInterruptEvent(GPIO_PORTB,ABUI_ENABLED_BUTTONS,0x00);
	gpioSetInterruptMaskDisable(GPIO_PORTB,ABUI_ENABLED_BUTTONS,ABUI_ENABLED_BUTTONS);

	//HWREG(GPIO_PORTB_BASE + GPIO_O_LOCK) = 0;
	//IntMasterEnable();
	//IntEnable(INT_GPIOE);
	//HWREG(0x40005408)= 0x00;
	//HWREG(0x4000540C) = 0x00;
	//HWREG(0x40005410) = 0x06;

}
void ABUIButtonsSetNextState(char _ABUIBB0NextState,char _ABUIBB3NextState,
								char _ABUIBB4NextState, char _ABUIBB5NextState,
								char _ABUIBB6NextState, char _ABUIBB7NextState){
	_ABUIButtonB0_UP_State = _ABUIBB0NextState;
	_ABUIButtonB3_DOWN_State = _ABUIBB3NextState;
	_ABUIButtonB4_ENTER_State = _ABUIBB4NextState;
	_ABUIButtonB5_CANCEL_State = _ABUIBB5NextState;
	_ABUIButtonB6_MENU_State = _ABUIBB6NextState;
	_ABUIButtonB7_OPTION_State = _ABUIBB7NextState;
}

////////////////////////
//
void ABUIButtonSetNextState(char _ABUIButton, char _ABSMNextState){
	switch(_ABUIButton){
	case _ABUIButtonB0_UP:
		_ABUIButtonB0_UP_State = _ABSMNextState;
		break ;
	case _ABUIButtonB3_DOWN:
		_ABUIButtonB3_DOWN_State = _ABSMNextState;
		break ;
	case _ABUIButtonB4_ENTER:
		_ABUIButtonB4_ENTER_State = _ABSMNextState;
		break ;
	case _ABUIButtonB5_CANCEL:
		_ABUIButtonB5_CANCEL_State = _ABSMNextState;
		break ;
	case _ABUIButtonB6_MENU:
		_ABUIButtonB6_MENU_State = _ABSMNextState;
		break ;
	case _ABUIButtonB7_OPTION:
		_ABUIButtonB7_OPTION_State = _ABSMNextState;
		break ;
	default:
		ABStateInvalid(AB_STATE_UI_BUTTON_SET_STATE_FAILURE);
	}
}


/////////////////
//UI Prints
void ABUIPrintStateInvalid(char state){
	lcdClear();
	lcdCursorHome();
	lcdWriteString("Invalid State:");
	lcdCursorHomeDown();
	lcdWriteString("State: ");
	lcdWriteNumber(state);
	lcdWriteCommand(LCD_CMD_CURSOR_POSITION_LINE_3);
	lcdWriteString("Contact Developer.");
	//TODO: TURN ON ERROR LED.
}


void ABUIUnlockPB7() {
	//#define GPIO_PORTD_BASE 0x40007000 // GPIO Port D
	//#define GPIO_O_LOCK 0x00000520 // GPIO Lock

	//After GPIOLOCK is modified, we are then able to set bits 0:7
	//of hardware register GPIOCR to make the configuration registers
	// modifiable.

	HWREG(GPIO_PORTB_BASE + GPIO_O_LOCK) = GPIO_LOCK_KEY;

	//Set the commit register for PD7 to allow changing the function
	HWREG(GPIO_PORTB_BASE + GPIO_O_CR) = 0xC0;

	//Enable the alternate function for PD7 (U2TX)
	HWREG(GPIO_PORTB_BASE + GPIO_O_DIR) |= 0xC0;


	// Turn on the digital enable for PD7
	HWREG(GPIO_PORTB_BASE + GPIO_O_DEN) &= 0x3F;


	// Relock the commit register, to prevent further changes
	//HWREG(GPIO_PORTB_BASE + GPIO_O_LOCK) = 0;
}
