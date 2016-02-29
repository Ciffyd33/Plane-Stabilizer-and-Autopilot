// HMC5883L.h

#ifndef _HMC5883L_h
#define _HMC5883L_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include "sensors.h"

// HMC5883L registers
#define HMC5883L_ADDRESS				(0x1E)

#define HMC5883L_CONFIG_A				(0x00)
#define HMC5883L_CONFIG_B				(0x01)
#define HMC5883L_MODE					(0x02)
#define HMC5883L_OUT_X_H				(0x03)
#define HMC5883L_OUT_X_L				(0x04)
#define HMC5883L_OUT_Z_H				(0x05)
#define HMC5883L_OUT_Z_L				(0x06)
#define HMC5883L_OUT_Y_H				(0x07)
#define HMC5883L_OUT_Y_L				(0x08)
#define HMC5883L_STATUS					(0x09)
#define HMC5883L_IDA					(0x0A)		// should return 0x48
#define HMC5883L_IDB					(0x0B)		// should return 0x34
#define HMC5883L_IDC					(0x0C)		// should return 0x33


enum Mrate { // set magnetometer ODR
	MRT_0075 = 0, // 0.75 Hz ODR
	MRT_015,      // 1.5 Hz
	MRT_030,      // 3.0 Hz
	MRT_075,      // 7.5 Hz
	MRT_15,       // 15 Hz
	MRT_30,       // 30 Hz
	MRT_75,       // 75 Hz ODR    
};



void initHMC5883L();
byte selfTestHMC5883L();
void readMagData(int16_t * destination);

#endif



