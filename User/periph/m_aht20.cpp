#include "m_aht20.hpp"

#include "debug.h"



bool aht20_init(periph::I2C_Hal *hal)
{
	if (hal == NULL) {
		return false;
	}
	
	
	// Send initialization command
	// uint8_t init_cmd[3] = {AHT20_CMD_INIT, 0x08, 0x00};
	hal->startSignal();
	hal->writeRequest(AHT20_I2C_ADDRESS);
	hal->writeData(AHT20_CMD_INIT);
	hal->writeData(0x08);
	hal->writeData(0x00);
	hal->stopSignal();
	
	Delay_Ms(10);
	
	// Check if sensor is calibrated
	return aht20_is_calibrated(hal);
}

bool aht20_soft_reset(periph::I2C_Hal *hal)
{
	if (hal == NULL) return false;
	
	hal->startSignal();
	hal->writeRequest(AHT20_I2C_ADDRESS);
	hal->writeData(AHT20_CMD_SOFT_RESET);
	hal->stopSignal();
	
	Delay_Ms(20);

	return true;
}

bool aht20_is_calibrated(periph::I2C_Hal *hal)
{
	if (hal == NULL) return false;
	
	uint8_t status = 0;

	hal->startSignal();
	hal->writeRequest(AHT20_I2C_ADDRESS);
	hal->writeData(0x71);
	hal->stopSignal();

	hal->startSignal();
	hal->readRequest(AHT20_I2C_ADDRESS);
	status = hal->readData();
	hal->stopSignal();
	
	return (status & AHT20_STATUS_CALIBRATED) != 0;
}

bool aht20_read_temperature_humidity(periph::I2C_Hal *hal, aht20_data_t *data)
{
	if (hal == NULL || data == NULL) return false;
	
	// Trigger measurement
	// uint8_t trigger_cmd[3] = {AHT20_CMD_TRIGGER_MEAS, 0x33, 0x00};

	hal->startSignal();
	hal->writeRequest(AHT20_I2C_ADDRESS);
	hal->writeData(AHT20_CMD_TRIGGER_MEAS);
	hal->writeData(0x33);
	hal->writeData(0x00);
	hal->stopSignal();
	
	// Wait for measurement to complete
	Delay_Ms(AHT20_MEASURE_DELAY_MS);
	
	// Read 6 bytes of data (status + humidity + temperature + CRC)
	hal->startSignal();
	hal->readRequest(AHT20_I2C_ADDRESS);

	uint8_t raw_data[6];
	for(int i = 0; i < 6; i++){
		raw_data[i] = hal->readData();
	}
	hal->stopSignal();

	// raw_data[0] = hal->readData();
	
	// Check if measurement is complete
	if (raw_data[0] & AHT20_STATUS_BUSY) {
		return false; // Measurement still in progress
	}
	
	// Extract humidity (20 bits)
	uint32_t humidity_raw = ((uint32_t)raw_data[1] << 12) |
						   ((uint32_t)raw_data[2] << 4) |
						   ((uint32_t)raw_data[3] >> 4);
	
	// Extract temperature (20 bits)
	uint32_t temperature_raw = (((uint32_t)raw_data[3] & 0x0F) << 16) |
							  ((uint32_t)raw_data[4] << 8) |
							  raw_data[5];
	
	// Convert to physical values
	data->humidity = (float)humidity_raw * 100.0f / 1048576.0f; // 2^20 = 1048576
	data->temperature = (float)temperature_raw * 200.0f / 1048576.0f - 50.0f;
	
	return true;
}