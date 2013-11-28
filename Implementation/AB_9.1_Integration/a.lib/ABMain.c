/*
 * ABMain.c
 *
 *  Created on: Nov 22, 2013
 *      Author: Administrator
 */
#include "ABMain.h"




tI2CMInstance g_sI2CInst; // I2C Instance.
tBMP180 g_sBMP180Inst; //BMP180 Instance.

void BMP180AppCallback(void* pvCallbackData, uint_fast8_t ui8Status){
	if(ui8Status == I2CM_STATUS_SUCCESS) g_vui8DataFlag = 1;
}
void BMP180I2CIntHandler(void){
	I2CMIntHandler(&g_sI2CInst);
}

void ABBMPInit(){
	SysCtlPeripheralEnable(SYSCTL_PERIPH_I2C1);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
	GPIOPinConfigure(GPIO_PA6_I2C1SCL);
	GPIOPinConfigure(GPIO_PA7_I2C1SDA);
	GPIOPinTypeI2CSCL(GPIO_PORTA_BASE, GPIO_PIN_6);
	GPIOPinTypeI2C(GPIO_PORTA_BASE, GPIO_PIN_7);
	IntMasterEnable();
	I2CMInit(&g_sI2CInst, I2C1_BASE, INT_I2C1, 0xff, 0xff,
			SysCtlClockGet());
	BMP180Init(&g_sBMP180Inst, &g_sI2CInst, BMP180_I2C_ADDRESS,
			BMP180AppCallback, &g_sBMP180Inst);
	while(g_vui8DataFlag == 0);
	g_vui8DataFlag = 0;
}
void ABBMPUpdate(){
	BMP180DataRead(&g_sBMP180Inst, BMP180AppCallback, &g_sBMP180Inst);
	//while(g_vui8DataFlag == 0);
	SysCtlDelay(1000);
	g_vui8DataFlag = 0;
	BMP180DataPressureGetFloat(&g_sBMP180Inst, &ABBMPPressure);
	BMP180DataTemperatureGetFloat(&g_sBMP180Inst, &ABBMPTemperature);
}
float ABBMPGetTemperature(){
	return ABBMPTemperature;
}

float ABBMPGetPressure(){
	return ABBMPPressure;
}

void ABLCDInit(){
	lcdInit(GPIO_PORTA,GPIO_PORTC,GPIO_PORTD);
	lcdClear();
	lcdCursorHome();
}


void ABDHT11Refresh(){
	dhtSetup();
	dhtSetActive();
	dht11getData();
	while(dhtIsActive());
}

void ABDHT11Output(){
	lcdCursorHome();
	lcdWriteString("Hmd: ");
	lcdWriteNumber(dht11getHumidity());
	lcdWriteString(" T: ");
	lcdWriteNumberWithBounds(ABBMPTemperature,2,2);
	lcdWriteLetter(223);
	lcdWriteString("C");
	lcdCursorHomeDown();
	lcdWriteString("Tmp: ");
	lcdWriteNumber(dht11getTemperature());
	lcdWriteString(" P: ");
	lcdWriteNumberWithBounds(ABBMPPressure/1000.0,3,2);
	lcdWriteString("kPa");
}

void ABLoadCellRefresh(int _ABLoadCellUnits){

	_ABWindVaneAngle = loadCellgetValues(WIND_VANE,_ABLoadCellUnits);
	_ABLoadCellLiftUp = loadCellgetValues(LOADCELL_LIFT_UP,_ABLoadCellUnits);
	_ABLoadCellLiftDown = loadCellgetValues(LOADCELL_LIFT_DOWN,_ABLoadCellUnits);
	_ABLoadCellDragFront = loadCellgetValues(LOADCELL_DRAG_FRONT,_ABLoadCellUnits);
	_ABLoadCellDragBack = loadCellgetValues(LOADCELL_DRAG_BACK,_ABLoadCellUnits);
}

void ABLoadCellOutput(){
	lcdWriteCommand(LCD_CMD_CURSOR_POSITION_LINE_3);
	lcdWriteString("LU:");
	lcdWriteNumberWithBounds(/*_ABLoadCellLiftUp*/_ABWindVaneAngle,2,2);
	lcdWriteString(" LD:");
	lcdWriteNumberWithBounds(_ABLoadCellLiftDown,2,2);
	lcdWriteCommand(LCD_CMD_CURSOR_POSITION_LINE_4);
	lcdWriteString("DF:");
	lcdWriteNumberWithBounds(_ABLoadCellDragFront,2,2);
	lcdWriteString(" DB:");
	lcdWriteNumberWithBounds(_ABLoadCellDragBack,2,2);
	lcdCursorHome();
}
void UARTIntHandler(void)
{
	uint32_t ui32Status;
	ui32Status = UARTIntStatus(UART0_BASE, true);
	UARTIntClear(UART0_BASE, ui32Status);
	while(UARTCharsAvail(UART0_BASE)){
		UARTCharPutNonBlocking(UART0_BASE,UARTCharGetNonBlocking(UART0_BASE));
	}
}

void ABUARTInit(){
	SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
	//Port Activate
	GPIOPinConfigure(GPIO_PA0_U0RX);
	GPIOPinConfigure(GPIO_PA1_U0TX);
	GPIOPinTypeUART(GPIO_PORTA_BASE,0x03);
	UARTConfigSetExpClk(UART0_BASE,SysCtlClockGet(),115200,0x60);
	IntEnable(INT_UART0);
	UARTIntEnable(UART0_BASE,0x50);

}



///////////////////////////////////////
///////////////////////////////////////
void ABStateInvalid(char state){
	ABUIPrintStateInvalid(state);

	//
	//Lock Execution for Evaluation.
	//Same as FaultISR.
	while(1);
}
void ABSysDelay(int milliseconds){
	SysCtlDelay((SysCtlClockGet()/1000)*milliseconds);
}

void ABStandardSysDelay(int delay){
	SysCtlDelay(delay);
}

///////////////////////////////////////

