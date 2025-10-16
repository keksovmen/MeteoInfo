#pragma once



#include <utility>

#include "m_hal_i2c.hpp"



namespace periph
{
	class Aht20 final
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