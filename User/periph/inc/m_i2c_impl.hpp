#pragma once



#include <stdint.h>
#include "ch32v00x.h"



namespace periph
{
	void i2c_init(int bound);
	void i2c_start_sequence(void);
	void i2c_stop_sequence(void);
	void i2c_write_request(int address);
	void i2c_read_request(int address);
	void i2c_write_byte(uint8_t data);
	uint8_t i2c_read_byte();
}