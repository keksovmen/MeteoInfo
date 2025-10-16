#include "m_aht20.hpp"



#define _AHT20_I2C_ADDRESS          0x38

// AHT20 Commands
#define _AHT20_CMD_INIT             0xBE
#define _AHT20_CMD_READ_STATUS      0x71
#define _AHT20_CMD_TRIGGER_MEAS     0xAC
#define _AHT20_CMD_SOFT_RESET       0xBA

// Status Register Bits
#define _AHT20_STATUS_BUSY          (1 << 7)
#define _AHT20_STATUS_CALIBRATED    (1 << 3)

// Measurement Constants
#define _AHT20_MEASURE_DELAY_MS     80



using namespace periph;



Aht20::Aht20(I2C_Hal& hal)
	: _hal(hal)
{

}

bool Aht20::init()
{
	// Send initialization command
	_hal.startSignal();
	_hal.writeRequest(_AHT20_I2C_ADDRESS);
	_hal.writeData(_AHT20_CMD_INIT);
	//args
	_hal.writeData(0x08);
	_hal.writeData(0x00);
	_hal.stopSignal();
	
	_hal.delayMs(10);
	
	// Check if sensor is calibrated
	return isCalibrated();
}

void Aht20::reset()
{
	_hal.startSignal();
	_hal.writeRequest(_AHT20_I2C_ADDRESS);
	_hal.writeData(_AHT20_CMD_SOFT_RESET);
	_hal.stopSignal();
	
	_hal.delayMs(20);
}

bool Aht20::isCalibrated()
{
	_hal.startSignal();
	_hal.writeRequest(_AHT20_I2C_ADDRESS);
	_hal.writeData(_AHT20_CMD_READ_STATUS);
	_hal.stopSignal();

	_hal.startSignal();
	_hal.readRequest(_AHT20_I2C_ADDRESS);
	uint8_t status = _hal.readData();
	_hal.stopSignal();
	
	return (status & _AHT20_STATUS_CALIBRATED) != 0;
}

std::pair<float, float> Aht20::readTempAndHum()
{
	_hal.startSignal();
	_hal.writeRequest(_AHT20_I2C_ADDRESS);
	_hal.writeData(_AHT20_CMD_TRIGGER_MEAS);
	_hal.writeData(0x33);
	_hal.writeData(0x00);
	_hal.stopSignal();
	
	// Wait for measurement to complete
	_hal.delayMs(_AHT20_MEASURE_DELAY_MS);
	
	// Read 6 bytes of data (status + humidity + temperature + CRC)
	_hal.startSignal();
	_hal.readRequest(_AHT20_I2C_ADDRESS);

	uint8_t raw_data[6];
	for(int i = 0; i < 6; i++){
		raw_data[i] = _hal.readData();
	}
	_hal.stopSignal();

	
	// Check if measurement is complete
	if (raw_data[0] & _AHT20_STATUS_BUSY) {
		return {}; // Measurement still in progress
	}
	
	// Extract humidity (20 bits)
	uint32_t humidity_raw = ((uint32_t)raw_data[1] << 12) |
						   ((uint32_t)raw_data[2] << 4) |
						   ((uint32_t)raw_data[3] >> 4);
	
	// Extract temperature (20 bits)
	uint32_t temperature_raw = (((uint32_t)raw_data[3] & 0x0F) << 16) |
							  ((uint32_t)raw_data[4] << 8) |
							  raw_data[5];
	
	return {(float)temperature_raw * 200.0f / 1048576.0f - 50.0f,
			(float)humidity_raw * 100.0f / 1048576.0f};
}