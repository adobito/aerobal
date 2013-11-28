//AB - 0.5
#include "a.lib/ABMain.h" //Component and Helper Functions
#include "a.lib/ABUI.h" //UI Functions
#include "a.lib/ABSM.h" //State Machine Functions.
#include "a.lib/bt.h"
//char state = 0;
//char buffer[20];
//char count;
//char bt_setupState = 0;

char ABStateMachineState  = AB_STATE_MAIN_1_INIT;
char bt_setupState = 0;
char bt_state = 0;
#define DESIRED_WIND_SPEED_RANGE 7.000

void ABStateMachineChangeState(int state){
	ABStateMachineState = state ;
}
void initiateTunnelSetup() {
	//UARTBTSend("InitSetup OK\n",13);
	//AB_SYSTEM_STATE = 0;
	bt_setupState = 0;
	//setupAck();
}



void ABStateMachineRun(){

	switch(ABStateMachineState){
		//////////////////////////////////////////////////////////////////////////////
		// Calibration State SubMachine 											//
		//////////////////////////////////////////////////////////////////////////////

		case AB_STATE_MAIN_1_INIT:
			ABSMMainPrintWelcomeMessage();
			break ;
		case AB_STATE_MAIN_2_CALIBRATE:
			ABSMMain_2_EnterCalibrateSubMachine();
			ABStateMachineRun();
			break ;
		case AB_STATE_MAIN_3_WIND_SETUP:
			ABStateMachineState = AB_STATE_MAIN_4_CONFIRM;
			ABStateMachineRun();
			break ;
		case AB_STATE_MAIN_4_CONFIRM:
			ABSMMain_4_ConfirmExperiment();
			break ;
		case AB_STATE_MAIN_5_VERIFY_WIND_SPEED:
			ABSMMain_5_VerifyWindSpeed();
			ABStateMachineRun();
			break;
		case AB_STATE_MAIN_6_EXPERIMENT:
			ABSMMain_6_GetExperimentData();
			ABStateMachineRun();
			break;
		case AB_STATE_MAIN_7_CALCULATIONS:
			ABSMMain_7_PerformCalculations();
			ABStateMachineRun();
			break;
		case AB_STATE_MAIN_8_OUTPUT_RESULTS:
			ABSMMain_8_OutputResults();
			break;

		//////////////////////////////////////////////////////////////////////////////
		// Calibration State SubMachine 											//
		//////////////////////////////////////////////////////////////////////////////
		case AB_STATE_CALIBRATION_1_INIT:
			ABSMCalibrate_1_Init();

			break;
		case AB_STATE_CALIBRATION_2_STORE_INIT_E:
			ABSMCalibrate_2_StoreInitialValues_E();
			ABStateMachineRun();
			break ;

		case AB_STATE_CALIBRATION_3_CONFIG_CLOSED_TUNNEL:
			ABSMCalibrate_3_PromptForClosedTunnel();
			break ;

		case AB_STATE_CALIBRATION_4_CHECK_WIND_SPEED:
			ABSMCalibrate_4_CalibrateWindSpeed();
			ABStateMachineRun();
			break ;

		case AB_STATE_CALIBRATION_5_STORE_FINAL_E:
			ABSMCalibrate_5_StoreFinalValues_E();
			ABStateMachineRun();
			break ;

		case AB_STATE_CALIBRATION_6_PROMPT_OBJECT:
			ABSMCalibrate_6_PromptForObject();
			break ;

		case AB_STATE_CALIBRATION_7_STORE_INIT_O:
			ABSMCalibrate_7_StoreInitialValues_O();
			ABStateMachineRun();
			break ;

		////
		case -1:
			break ;
		//Other Operations
		case AB_STATE_SM_NO_OPERATION:
			//TODO: Light something?
			break ;
		case AB_STATE_VOID:
			//TODO: Voided State.
			ABStateInvalid(AB_STATE_VOID);
			break ;
		default:
			ABStateInvalid(ABStateMachineState);
			break;
	}
}

/*
void ABStateMachine_CAL() {
	switch (AB_SYSTEM_STATE) {
	case 0:

		UARTBTSend( "Ack to Save Values\n",strlen( "Ack to Save Values\n"));
		AB_SYSTEM_STATE++;
		break;
	case 1:
		AB_SYSTEM_STATE++;
		initUpStaticEmpty = rand();
		initDownStaticEmpty= rand();
		initLeftStaticEmpty = rand();
		initRightStaticEmpty = rand();
		UARTBTSend("Ack to Power Fan\n",strlen("Ack to Power Fan\n"));
		break;
	case 2:
		turnPowerOn();
		//wait for stable read
		UARTBTSend("Ack to Save Values\n",strlen("Ack to Save Values\n"));
		AB_SYSTEM_STATE++;
		break;
	case 3:
		initUpDynamicEmpty = rand();
		initDownDynamicEmpty= rand();
		initLeftDynamicEmpty = rand();
		initRightDynamicEmpty = rand();
		turnPowerOff();
		UARTBTSend("Ack to After placing Object\n",strlen("Ack to After placing Object\n"));
		AB_SYSTEM_STATE++;
		break;
	case 4:
		initUpStaticLoad = rand();
		initDownStaticLoad= rand();
		initLeftStaticLoad = rand();
		initRightStaticLoad = rand();
		UARTBTSend("Ack to Power Fan\n",strlen("Ack to Power Fan\n"));
		AB_SYSTEM_STATE++;
		break;
	case 5:
		turnPowerOn();
		UARTBTSend("Experiment Started\n",strlen("Experiment Started\n"));
		AB_SYSTEM_STATE++;
		break;
	}

}
*/


void ABTestComponents(){


	//int angle  ;
	//int time   ;

	SysCtlClockSet(SYSCTL_SYSDIV_1 | SYSCTL_USE_OSC | SYSCTL_XTAL_16MHZ |SYSCTL_OSC_MAIN);

	ABLCDInit();
	ABBMPInit();
	servoSetupPWM();
	loadCellSetup();
	ABUARTInit();
	BluetoothMainInit();
	int count = 0 ;

	while(1){
		if(++count % 10){
			ABBMPUpdate();
		}
		else{
			ABLoadCellRefresh(OUNCES);
		}


		ABDHT11Refresh();
		lcdClear();
		ABDHT11Output();
		ABLoadCellOutput();
		SysCtlDelay(10000);
		UARTSend("Something",9);

		//servosetPositionPWM(angle,time);
		//angle = (angle+45)%180;
		//time = (time+25)%100;
		SysCtlDelay(500000);
	}

}

void ABTestServo(){

	lcdInit(GPIO_PORTA,GPIO_PORTC,GPIO_PORTD);
	lcdClear();
	servoSetupPWM();
	while(1){
		//servosetPositionPWM(45,25);
		//servosetPositionPWM(90,50);
		servosetPositionPWM(120,75);
		servosetPositionPWM(180,100);

	}
}


void ABTestServoAnemometer(){

	lcdInit(GPIO_PORTA,GPIO_PORTC,GPIO_PORTD);
	lcdClear();
	servoSetupPWM();
	anemometerSetup();
	while(1){
		//servosetPositionPWM(45,25);
		//servosetPositionPWM(90,50);
		servosetPositionPWM(160,0);
		ABServoChange(180,130);

		lcdClear();
		anemometerEnable();
		SysCtlDelay(10000000);
		lcdWriteNumber(anemometerGetVelocityInMIH());
		anemometerDisable();
		SysCtlDelay(10000000);
		ABServoChange(130,160);
		lcdCursorHomeDown();
		anemometerEnable();
		SysCtlDelay(10000000);
		lcdWriteNumber(anemometerGetVelocityInMIH());
		anemometerDisable();






	}
}

void ABWindSpeedAdjust(){

}
//Remember to change interrupt.
void ABTestStateMachine(){
	lcdInit(GPIO_PORTA,GPIO_PORTC,GPIO_PORTD);
	loadCellSetup();
	servoSetupPWM();
	anemometerSetup();
	ABUIButtonSetup();

	while(1);
}

void anemometerTest1(){
	anemometerSetup();
	anemometerEnable();
	lcdInit(GPIO_PORTA,GPIO_PORTC,GPIO_PORTD);
	while(1){

		lcdClear();
		lcdWriteNumber(anemometerGetVelocityInKMH());
		lcdCursorHomeDown();
		lcdWriteNumber(anemometerGetVelocityInMIH());
		SysCtlDelay(SysCtlClockGet()/10);
	}
}

void ABStrainGaugeTest(){
	loadCellSetup();
	lcdInit(GPIO_PORTA,GPIO_PORTC,GPIO_PORTD);
	while(1){
		ABLoadCellRefresh(OUNCES);
		lcdClear();
		lcdWriteString("Lift Up: ");
		lcdWriteNumberWithBounds(_ABLoadCellLiftUp,3,3);
		lcdCursorHomeDown();
		lcdWriteString("Lift Down: ");
		lcdWriteNumberWithBounds(_ABLoadCellLiftDown,3,3);
		lcdWriteCommand(LCD_CMD_CURSOR_POSITION_LINE_3);
		lcdWriteString("Drag Front: ");
		lcdWriteNumberWithBounds(_ABLoadCellDragFront,3,3);
		lcdWriteCommand(LCD_CMD_CURSOR_POSITION_LINE_4);
		lcdWriteString("Drag Back: ");
		lcdWriteNumberWithBounds(_ABLoadCellDragBack,3,3);
		SysCtlDelay(1000);
	}
}

void ABWindVaneTest(){
	lcdInit(GPIO_PORTA,GPIO_PORTC,GPIO_PORTD);
	lcdClear();
	loadCellSetup();
	while(1){
		ABLoadCellRefresh(OUNCES);
		lcdClear();
		lcdWriteString("Direction: ");
		lcdWriteNumberWithBounds(_ABWindVaneAngle,2,2);
		SysCtlDelay(3000000);

	}
}
void ABInit(){
	lcdInit(GPIO_PORTA,GPIO_PORTC,GPIO_PORTD);
}
int main(void){

	BluetoothMainInit();
	//while(1);
	//ABInit();
	//ABWindVaneTest();
	//ABStrainGaugeTest();
	//anemometerTest1();
	//ABTestComponents();
	//ABTestServoAnemometer();
	ABTestStateMachine();
	while(1){
		//UARTBTSend("r\n",2);
	}
}

