/*
 * ABSM.c
 *
 *  Created on: Nov 22, 2013
 *      Author: Administrator
 */

#include "ABSM.h"
#include "anemometer.h"
#include "servo.h"
#include "bt.h"

float _ABLoadBase_NoWindDragFront;
float _ABLoadBase_NoWindDragBack;
float _ABLoadBase_NoWindLiftUp;
float _ABLoadBase_NoWindLiftDown;
float _ABLoadBase_WindDragFront;
float _ABLoadBase_WindDragBack;
float _ABLoadBase_WindLiftUp;
float _ABLoadBase_WindLiftDown;
float _ABLoadCombined_NoWindDragFront ;
float _ABLoadCombined_NoWindDragBack ;
float _ABLoadCombined_NoWindLiftUp ;
float _ABLoadCombined_NoWindLiftDown ;
float _ABLoadCombined_WindDragFront ;
float _ABLoadCombined_WindDragBack ;
float _ABLoadCombined_WindLiftUp ;
float _ABLoadCombined_WindLiftDown ;

float _wind_car_drag ;
float _wind_car_lift ;

#define DESIRED_WIND_SPEED_RANGE 7.000

void ABServoChange(int start, int finish){
	if(start < finish){
		while(start < finish){

			start+= 5;
			servosetPositionPWM(start,0);
		}
	}
	else{
		while(start > finish){
			start -= 5 ;
			servosetPositionPWM(start,0);
		}
	}
}

void ABServoSet(){
	float wind_speed = 0;
	//
	int window_angle = 130;
	while(1){
		ABServoChange(window_angle,window_angle+5);
		SysCtlDelay(10000000);
		anemometerEnable();
		SysCtlDelay(10000000);
		wind_speed = anemometerGetVelocityInMIH();
		anemometerDisable();
		if((wind_speed-DESIRED_WIND_SPEED_RANGE) > 0 || window_angle > 175){
			break ;
		}
		window_angle+= 5;
	}
}
void ABSMPrintScreen(char line1_en, char* line1,
					 char line2_en, char* line2,
					 char line3_en, char* line3,
					 char line4_en, char* line4){
	lcdClear();
	if(line1_en){
		lcdCursorHome();
		lcdWriteString(line1);
	}
	if(line2_en){
		lcdCursorHomeDown();
		lcdWriteString(line2);
	}
	if(line3_en){
		lcdWriteCommand(LCD_CMD_CURSOR_POSITION_LINE_3);
		lcdWriteString(line3);
	}
	if(line4_en){
		lcdWriteCommand(LCD_CMD_CURSOR_POSITION_LINE_4);
		lcdWriteString(line4);
	}
}
//////////////////////////////////////////////////////////
// The Main State Sub Machine.
//////////
void ABSMMainPrintWelcomeMessage(){
	ABUIButtonsSetNextState(
			AB_STATE_MAIN_2_CALIBRATE,AB_STATE_MAIN_1_INIT,
			AB_STATE_MAIN_1_INIT,AB_STATE_MAIN_1_INIT,
			AB_STATE_MAIN_1_INIT,AB_STATE_MAIN_1_INIT);
	ABSMPrintScreen(1," Welcome To Aerobal  ",
					0,"",
					1,"Press Enter to Start",
					1,"   Calibration ");
	sendState(AB_STATE_MAIN_1_INIT);
	ABStandardSysDelay(5000000);
	ABUIButtonEnable();
	//Delays are outside the function.

}

void ABSMMain_2_EnterCalibrateSubMachine(){
	//Call Calibration State Machine.
	ABStateMachineState = AB_STATE_CALIBRATION_1_INIT;
	sendState(AB_STATE_MAIN_2_CALIBRATE);
}


void ABSMMain_4_ConfirmExperiment(){
	ABUIButtonsSetNextState(
				AB_STATE_MAIN_5_VERIFY_WIND_SPEED,AB_STATE_MAIN_1_INIT,
				AB_STATE_MAIN_1_INIT,AB_STATE_MAIN_1_INIT,
				AB_STATE_MAIN_1_INIT,AB_STATE_MAIN_1_INIT);
	ABSMPrintScreen(1," Main_4 ",
						0,"",
						1,"Press Enter to Start",
						1,"   Experiment ");
	sendState(AB_STATE_MAIN_4_CONFIRM);
	ABStandardSysDelay(5000000);
	ABUIButtonEnable();
}

void ABSMMain_5_VerifyWindSpeed(){
	ABStateMachineState = AB_STATE_MAIN_6_EXPERIMENT;
	//
	ABSMPrintScreen(1," Main_5:  ",
					1," Setting Wind Speed",
					1," Don't Open Tunnel",
					0,"");
	turnPowerOn();
	sendState(AB_STATE_MAIN_5_VERIFY_WIND_SPEED);
	//Verifying Wind Speed.
	ABServoSet();
	ABStandardSysDelay(5000000);
}
void ABSMMain_6_GetExperimentData(){
	ABStateMachineState = AB_STATE_MAIN_7_CALCULATIONS;

	//
	ABSMPrintScreen(1," Main_6:  ",
						1," Recording Experiment",
						1," Data.",
						0,"");
	//
	sendState(AB_STATE_MAIN_6_EXPERIMENT);
	ABLoadCellRefresh(OUNCES);
	_ABLoadCombined_WindDragFront = _ABLoadCellDragFront;
	_ABLoadCombined_WindDragBack = _ABLoadCellDragBack;
	_ABLoadCombined_WindLiftUp = _ABLoadCellLiftUp;
	_ABLoadCombined_WindLiftDown = _ABLoadCellLiftDown;

	ABStandardSysDelay(5000000);
}

void ABSMMain_7_PerformCalculations(){
	ABStateMachineState = AB_STATE_MAIN_8_OUTPUT_RESULTS;

	//
	ABSMPrintScreen(1," Main_7:  ",
					1," Performing ",
					1," Calculations...",
					0,"");
	//
	sendState(AB_STATE_MAIN_7_CALCULATIONS);
	turnPowerOff();
	//Discrete values.
	float wind_base_drag_front = _ABLoadBase_WindDragFront - _ABLoadBase_NoWindDragFront;
	float wind_base_drag_back = _ABLoadBase_WindDragBack - _ABLoadBase_NoWindDragFront ;
	float wind_base_lift_up = _ABLoadBase_WindLiftUp - _ABLoadBase_NoWindLiftUp;
	float wind_base_lift_down = _ABLoadBase_WindLiftDown - _ABLoadBase_NoWindLiftDown ;

	float wind_comb_drag_front = _ABLoadCombined_WindDragFront - _ABLoadCombined_NoWindDragFront;
	float wind_comb_drag_back = _ABLoadCombined_WindDragBack - _ABLoadCombined_NoWindDragFront ;
	float wind_comb_lift_up = _ABLoadCombined_WindLiftUp - _ABLoadCombined_NoWindLiftUp;
	float wind_comb_lift_down = _ABLoadCombined_WindLiftDown - _ABLoadCombined_NoWindLiftDown ;

	//Wind on base and both.
	float wind_base_drag = (wind_base_drag_front > 0) ? wind_base_drag_front : wind_base_drag_back;
	float wind_base_lift = (wind_base_lift_up > 0) ? wind_base_lift_up : wind_base_lift_down;
	float wind_comb_drag = (wind_comb_drag_front > 0) ? wind_comb_drag_front : wind_comb_drag_back;
	float wind_comb_lift = (wind_comb_lift_up > 0) ? wind_comb_lift_up : wind_comb_lift_down;

	//Calculate wind car.
	_wind_car_drag = wind_comb_drag - wind_base_drag ;
	_wind_car_lift = wind_comb_lift - wind_base_lift ;

	//lcdClear();
	//lcdWriteString("CD: ");
	//lcdWriteNumberWithBounds(wind_car_drag,3,3);
	lcdCursorHomeDown("CL: ");
	//lcdWriteNumberWithBounds(wind_car_lift,3,3);
	ABStandardSysDelay(5000000);
}

void ABSMMain_8_OutputResults(){
	ABUIButtonsSetNextState(
					AB_STATE_MAIN_1_INIT,AB_STATE_MAIN_1_INIT,
					AB_STATE_MAIN_1_INIT,AB_STATE_MAIN_1_INIT,
					AB_STATE_MAIN_1_INIT,AB_STATE_MAIN_1_INIT);
	//
	//
	sendState(AB_STATE_MAIN_8_OUTPUT_RESULTS);
	lcdClear();
	lcdWriteString("Main 8: Results");
	lcdCursorHomeDown();
	lcdWriteString("Drag: ");
	lcdWriteNumberWithBounds(_wind_car_drag,3,3);
	lcdWriteCommand(LCD_CMD_CURSOR_POSITION_LINE_3);
	lcdWriteString("Lift: ");
	lcdWriteNumberWithBounds(_wind_car_lift,3,3);
	sendMeasurements(_wind_car_drag,_wind_car_lift,0);
	ABStandardSysDelay(5000000);

	ABUIButtonEnable();
}


///////////////////////////////////////////////////////////
// The Calibrate State Sub Machine.
//////////
void ABSMCalibrate_1_Init(){
	ABUIButtonsSetNextState(
			AB_STATE_CALIBRATION_2_STORE_INIT_E,AB_STATE_MAIN_1_INIT,
			AB_STATE_MAIN_1_INIT,AB_STATE_MAIN_1_INIT,
			AB_STATE_MAIN_1_INIT,AB_STATE_MAIN_1_INIT);
	//
	ABSMPrintScreen(1," Calibrate_1:  ",
					1," Make Sure Tunnel ",
					1," is Empty. Press",
					1," Enter to Calibrate");
	//
	sendState(AB_STATE_CALIBRATION_1_INIT);
	ABStandardSysDelay(5000000);
	ABUIButtonEnable();
}

void ABSMCalibrate_2_StoreInitialValues_E(){
	ABStateMachineState = AB_STATE_CALIBRATION_3_CONFIG_CLOSED_TUNNEL;
	//
	ABSMPrintScreen(1," Calibrate_2:  ",
					1," Calibrating.... ",
					1," Please Wait.",
					0,"");
	//Read multiple values.


	ABLoadCellRefresh(OUNCES);
	_ABLoadBase_NoWindDragFront = _ABLoadCellDragFront;
	_ABLoadBase_NoWindDragBack = _ABLoadCellDragBack;
	_ABLoadBase_NoWindLiftUp = _ABLoadCellLiftUp;
	_ABLoadBase_NoWindLiftDown = _ABLoadCellLiftDown;

	//BTSendData: those variables for initial config.

	ABStandardSysDelay(5000000);
	//ABUIButtonEnable();
}

void ABSMCalibrate_3_PromptForClosedTunnel(){
	ABUIButtonsSetNextState(
			AB_STATE_CALIBRATION_4_CHECK_WIND_SPEED,AB_STATE_MAIN_1_INIT,
			AB_STATE_MAIN_1_INIT,AB_STATE_MAIN_1_INIT,
			AB_STATE_MAIN_1_INIT,AB_STATE_MAIN_1_INIT);
	//
	ABSMPrintScreen(1," Calibrate_3:  ",
					1," Please Close all  ",
					1," Tunnel Doors. Press",
					0,"");
	//
	sendState(AB_STATE_CALIBRATION_2_STORE_INIT_E);
	ABStandardSysDelay(5000000);
	ABUIButtonEnable();
}

void ABSMCalibrate_4_CalibrateWindSpeed(){
	ABStateMachineState = AB_STATE_CALIBRATION_5_STORE_FINAL_E;
	//
	ABSMPrintScreen(1," Calibrate_4:  ",
					1," Calibrating Wind",
					1," Don't Open Tunnel",
					0,"");
	//
	sendState(AB_STATE_CALIBRATION_4_CHECK_WIND_SPEED);
	turnPowerOn();
	//Turn on Wind Tunnel
	ABServoSet();
	ABStandardSysDelay(5000000);

}

void ABSMCalibrate_5_StoreFinalValues_E(){
	ABStateMachineState = AB_STATE_CALIBRATION_6_PROMPT_OBJECT;

	//
	ABSMPrintScreen(1," Calibrate_5:  ",
					1," Storing Final ",
					1," Values.",
					0,"");
	//
	sendState(AB_STATE_CALIBRATION_5_STORE_FINAL_E);
	ABLoadCellRefresh(OUNCES);
	_ABLoadBase_WindDragFront = _ABLoadCellDragFront;
	_ABLoadBase_WindDragBack = _ABLoadCellDragBack;
	_ABLoadBase_WindLiftUp = _ABLoadCellLiftUp;
	_ABLoadBase_WindLiftDown = _ABLoadCellLiftDown;
	ABStandardSysDelay(5000000);
	turnPowerOff();
	//ABUIButtonEnable();
}

void ABSMCalibrate_6_PromptForObject(){
	ABUIButtonsSetNextState(
				AB_STATE_CALIBRATION_7_STORE_INIT_O,AB_STATE_MAIN_1_INIT,
				AB_STATE_MAIN_1_INIT,AB_STATE_MAIN_1_INIT,
				AB_STATE_MAIN_1_INIT,AB_STATE_MAIN_1_INIT);
	//
	ABSMPrintScreen(1," Calibrate_6:  ",
					1," Place object",
					1," in tunnel base.",
					0,"");
	//
	sendState(AB_STATE_CALIBRATION_6_PROMPT_OBJECT);
	ABStandardSysDelay(5000000);
	ABUIButtonEnable();
}

void ABSMCalibrate_7_StoreInitialValues_O(){
	ABStateMachineState = AB_STATE_MAIN_3_WIND_SETUP;
	//
	ABSMPrintScreen(1," Calibrate_7:  ",
					1," Storing initial",
					1," object values..",
					0,"");
	//
	sendState(AB_STATE_CALIBRATION_7_STORE_INIT_O);
	ABLoadCellRefresh(OUNCES);
	_ABLoadCombined_NoWindDragFront = _ABLoadCellDragFront;
	_ABLoadCombined_NoWindDragBack = _ABLoadCellDragBack;
	_ABLoadCombined_NoWindLiftUp = _ABLoadCellLiftUp;
	_ABLoadCombined_NoWindLiftDown = _ABLoadCellLiftDown;
	ABStandardSysDelay(5000000);
}



//ABStateMachine_CAL : Calibration State Machine
