#pragma once


// #ifdef __cplusplus
// extern "C"{
// #endif



#include <stdint.h>
#include <stdbool.h>

#include "m_hal_i2c.hpp"



// AHT20 I2C Address
#define AHT20_I2C_ADDRESS          0x38

// AHT20 Commands
#define AHT20_CMD_INIT             0xBE
#define AHT20_CMD_TRIGGER_MEAS     0xAC
#define AHT20_CMD_SOFT_RESET       0xBA

// Status Register Bits
#define AHT20_STATUS_BUSY          (1 << 7)
#define AHT20_STATUS_CALIBRATED    (1 << 3)

// Measurement Constants
#define AHT20_MEASURE_DELAY_MS     80



typedef struct {
	float temperature;
	float humidity;
} aht20_data_t;



// Public functions
bool aht20_init(periph::I2C_Hal *hal);
bool aht20_read_temperature_humidity(periph::I2C_Hal *hal, aht20_data_t *data);
bool aht20_soft_reset(periph::I2C_Hal *hal);
bool aht20_is_calibrated(periph::I2C_Hal *hal);



// #ifdef __cplusplus
// }
// #endif