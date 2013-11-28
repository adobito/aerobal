/*
 * ABUI.h
 *
 *  Created on: Nov 22, 2013
 *      Author: Administrator
 *      #TODO organize, organize, organize!
 */

#ifndef ABUI_H_
#define ABUI_H_

#include "ABMain.h"
#include "ABSM.h"
#include "lcd.h"
#include "gpio.h"

#define AB_UI_BUTTON_STANDARD_DELAY 200000

//////
// Button Definitions
#define _ABUIButtonB0_UP 0x01
#define _ABUIButtonB3_DOWN 0x08
#define _ABUIButtonB4_ENTER 0x10
#define _ABUIButtonB5_CANCEL 0x20
#define _ABUIButtonB6_MENU 0x40
#define _ABUIButtonB7_OPTION 0x80

#define ABUI_ENABLED_BUTTONS 0x39

//////////////////////////
//Button States



//////////////////////////
//Functions

///////////////////////
//Buttons Interrupt.
void ABUIButtonInterrupt();
void ABUIButtonsSetNextState(char _ABUIBB0NextState,char _ABUIBB3NextState,
		char _ABUIBB4NextState, char _ABUIBB5NextState,
		char _ABUIBB6NextState, char _ABUIBB7NextState);
void ABUIButtonSetNextState(char _ABUIButton, char _ABSMNextState);
void ABUIPrintStateInvalid(char state);
void ABUIButtonSetup();

//Implementation Definition.
extern void ABStateMachineChangeState(int state);
extern void ABStateMachineRun();
void ABUIButtonDisable();
void ABUIButtonEnable();
#endif /* ABUI_H_ */
