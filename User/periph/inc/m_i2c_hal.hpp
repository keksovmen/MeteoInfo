#pragma once

#include <stdint.h>


namespace periph
{
	class I2C_Hal
	{
		public:
			void init(int speed);

			void startSignal();
			void stopSignal();

			void readRequest(int address);
			void writeRequest(int address);

			void writeData(int data);
			uint8_t readData();

			void delayMs(uint32_t ms);
	};
}