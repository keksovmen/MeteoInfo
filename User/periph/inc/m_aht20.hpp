#pragma once



#include <utility>

#include "m_i2c_hal.hpp"



namespace periph
{
	class Aht20
	{
		public:
			Aht20(I2C_Hal& hal);

			bool init();
			void reset();
			bool isCalibrated();
			/**
			 * @brief int32_t is actual value * 100, so you have 2 digits for precision
			 * first = temperature
			 * second = humidity
			 * 
			 * @return std::pair<int32_t, int32_t> 
			 */
			std::pair<int32_t, int32_t> readTempAndHum();

		private:
			I2C_Hal& _hal;
	};
}