// 
// 
// 

#include "ADXL345.h"

/*#define ASCALE		(AFS_2G)

#if ASCALE==AFS_2G
#define ARES		(2.0 / (512.*64.))

#elif ASCALE==AFS_4G
#define ARES		(4.0 / (1024.*32.))

#endif*/

uint8_t Ascale = AFS_2G; // ADXL345 in +/- 2g mode
uint8_t Arate = ARTBW_200_100; // 200 Hz ODR, 100 Hz bandwidth

float aRes; // scale resolutions per LSB for the sensors

/* ###################################################
*  ##### Useful functions for accelerometer ##########
*  ###################################################
*/

void getAres() {
	switch (Ascale)
	{
		// Possible accelerometer scales (and their register bit settings) are:
		// 2 Gs (00), 4 Gs (01), 8 Gs (10), and 16 Gs  (11). 
	case AFS_2G:
		aRes = 2.0 / (512.*64.);   // 10-bit 2s-complement
		break;
	case AFS_4G:
		aRes = 4.0 / (1024.*32.);  // 11-bit 2s-complement
		break;
	case AFS_8G:
		aRes = 8.0 / (2048.*16.);  // 12-bit 2s-complement
		break;
	case AFS_16G:
		aRes = 16.0 / (4096.*8.);  // 13-bit 2s-complement
		break;
	}
}

void readAccelData(int16_t * destination)
{
	uint8_t rawData[6];  // x/y/z accel register data stored here
	readBytes(ADXL345_ADDRESS, ADXL345_DATAX0, 6, &rawData[0]);  // Read the six raw data registers into data array
	destination[0] = ((int16_t)rawData[1] << 8) | rawData[0];  // Turn the MSB and LSB into a signed 16-bit value
	destination[1] = ((int16_t)rawData[3] << 8) | rawData[2];
	destination[2] = ((int16_t)rawData[5] << 8) | rawData[4];
}

void initADXL345()
{
	// wake up device
	writeByte(ADXL345_ADDRESS, ADXL345_POWER_CTL, 0x00); // Put device in standby mode and clear sleep bit 2
	delay(100);  // Let device settle down
	writeByte(ADXL345_ADDRESS, ADXL345_POWER_CTL, 0x08); // Put device in normal mode

														 // Set accelerometer configuration; interrupt active high, left justify MSB
	writeByte(ADXL345_ADDRESS, ADXL345_DATA_FORMAT, 0x04 | Ascale); // Set full scale range for the accelerometer 

																	// Choose ODR and bandwidth
	writeByte(ADXL345_ADDRESS, ADXL345_BW_RATE, Arate); // Select normal power operation, and ODR and bandwidth

	writeByte(ADXL345_ADDRESS, ADXL345_INT_ENABLE, 0x80);  // Enable data ready interrupt
	writeByte(ADXL345_ADDRESS, ADXL345_INT_MAP, 0x00);     // Enable data ready interrupt on INT_1

	writeByte(ADXL345_ADDRESS, ADXL345_FIFO_CTL, 0x00);    // Bypass FIFO
}

void calADXL345()
{
	uint8_t data[6] = { 0, 0, 0, 0, 0, 0 };
	int abias[3] = { 0, 0, 0 };
	int16_t accel_bias[3] = { 0, 0, 0 };
	int samples, ii;

	// wake up device
	writeByte(ADXL345_ADDRESS, ADXL345_POWER_CTL, 0x00); // Put device in standby mode and clear sleep bit 2
	delay(10);  // Let device settle down
	writeByte(ADXL345_ADDRESS, ADXL345_POWER_CTL, 0x08); // Put device in normal mode
	delay(10);

	// Set accelerometer configuration; interrupt active high, left justify MSB
	writeByte(ADXL345_ADDRESS, ADXL345_DATA_FORMAT, 0x04 | 0x00); // Set full scale range to 2g for the bias calculation 
	uint16_t  accelsensitivity = 256;  // = 256 LSB/g at 2g full scale

									   // Choose ODR and bandwidth
	writeByte(ADXL345_ADDRESS, ADXL345_BW_RATE, 0x09); // Select normal power operation, and 100 Hz ODR and 50 Hz bandwidth
	delay(10);

	writeByte(ADXL345_ADDRESS, ADXL345_FIFO_CTL, 0x40 | 0x2F);    // Enable FIFO stream mode | collect 32 FIFO samples
	delay(1000);  // delay 1000 milliseconds to collect FIFO samples

	samples = readByte(ADXL345_ADDRESS, ADXL345_FIFO_STATUS);
	for (ii = 0; ii < samples; ii++) {
		readBytes(ADXL345_ADDRESS, ADXL345_DATAX0, 6, &data[0]);
		accel_bias[0] += (((int16_t)data[1] << 8) | data[0]) >> 6;
		accel_bias[1] += (((int16_t)data[3] << 8) | data[2]) >> 6;
		accel_bias[2] += (((int16_t)data[5] << 8) | data[4]) >> 6;
	}

	accel_bias[0] /= samples; // average the data
	accel_bias[1] /= samples;
	accel_bias[2] /= samples;

	// Remove gravity from z-axis accelerometer bias value
	if (accel_bias[2] > 0) {
		accel_bias[2] -= accelsensitivity;
	}
	else {
		accel_bias[2] += accelsensitivity;
	}

	abias[0] = (int)((-accel_bias[0] / 4) & 0xFF); // offset register are 8 bit 2s-complement, so have sensitivity 1/4 of 2g full scale
	abias[1] = (int)((-accel_bias[1] / 4) & 0xFF);
	abias[2] = (int)((-accel_bias[2] / 4) & 0xFF);

	writeByte(ADXL345_ADDRESS, ADXL345_OFSX, abias[0]);
	writeByte(ADXL345_ADDRESS, ADXL345_OFSY, abias[1]);
	writeByte(ADXL345_ADDRESS, ADXL345_OFSZ, abias[2]);
}


