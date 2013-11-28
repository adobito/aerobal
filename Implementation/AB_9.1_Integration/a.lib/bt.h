/*
 * bt.h
 *
 *  Created on: Nov 21, 2013
 *      Author: Administrator
 */

#ifndef BT_H_
#define BT_H_

struct RollingBuffer;

void BluetoothMainInit();
void UARTBTSend(const uint8_t *pui8Buffer, uint32_t ui32Count);
void AddToBuffer(struct RollingBuffer *r, float value);
void InitBuffer(struct RollingBuffer *r);
void sendState(int state);
void sendMeasurements(float drag, float lift, float tilt);
void turnPowerOff();
void turnPowerOn();
#endif /* BT_H_ */
