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
			std::pair<float, float> readTempAndHum();

		private:
			I2C_Hal& _hal;
	};
}