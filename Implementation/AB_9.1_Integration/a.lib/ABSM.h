/*
 * ABSM.h
 *
 *  Created on: Nov 22, 2013
 *      Author: Administrator
 */

#ifndef ABSM_H_
#define ABSM_H_

#include "ABUI.h"
#include "ABMain.h"


//State Machine States
#define AB_STATE_MAIN_1_INIT 0x00
#define AB_STATE_MAIN_2_CALIBRATE 0x01 // To Sub Machine
#define AB_STATE_MAIN_3_WIND_SETUP 0x02
#define AB_STATE_MAIN_4_CONFIRM 0x03
#define AB_STATE_MAIN_5_VERIFY_WIND_SPEED 0x04
#define AB_STATE_MAIN_6_EXPERIMENT 0x05
#define AB_STATE_MAIN_7_CALCULATIONS 0x06
#define AB_STATE_MAIN_8_OUTPUT_RESULTS 0x07

#define AB_STATE_CALIBRATION_1_INIT 0x08
#define AB_STATE_CALIBRATION_2_STORE_INIT_E 0x09
#define AB_STATE_CALIBRATION_3_CONFIG_CLOSED_TUNNEL 0x0A
#define AB_STATE_CALIBRATION_4_CHECK_WIND_SPEED 0x0B//To Sub Machine
#define AB_STATE_CALIBRATION_5_STORE_FINAL_E 0x0C
#define AB_STATE_CALIBRATION_6_PROMPT_OBJECT 0x0D
#define AB_STATE_CALIBRATION_7_STORE_INIT_O 0x0F


//UI and Control States

#define AB_STATE_SM_NO_OPERATION 0xFD //Button does not do any function.
#define AB_STATE_UI_BUTTON_SET_STATE_FAILURE 0xFE //ABSM.c ABSMButtonSetNextState
#define AB_STATE_VOID 0xFF

extern char ABStateMachineState ;
///////
// Function Definitions
void ABSMMainPrintWelcomeMessage();
void ABSMMain_2_EnterCalibrateSubMachine();
void ABSMMain_4_ConfirmExperiment();
void ABSMMain_5_VerifyWindSpeed();
void ABSMMain_6_GetExperimentData();
void ABSMMain_7_PerformCalculations();
void ABSMMain_8_OutputResults();

void ABSMCalibrate_1_Init();
void ABSMCalibrate_2_StoreInitialValues_E();
void ABSMCalibrate_3_PromptForClosedTunnel();
void ABSMCalibrate_4_CalibrateWindSpeed();
void ABSMCalibrate_5_StoreFinalValues_E();
void ABSMCalibrate_6_PromptForObject();
void ABSMCalibrate_7_StoreInitialValues_O();

#endif /* ABSM_H_ */
