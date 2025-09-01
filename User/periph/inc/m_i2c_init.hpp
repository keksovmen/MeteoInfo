#pragma once



#include <stdint.h>



namespace periph
{
	void I2C_Init(int bound);
	void I2C_StartSequence(void);
	void I2C_StopSequence(void);
	void I2C_WriteRequest(int address);
	void I2C_ReadRequest(int address);
	void I2C_WriteByte(uint8_t data);
}