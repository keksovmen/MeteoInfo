#pragma once



namespace periph
{
	class I2C_Hal
	{
		public:
			virtual ~I2C_Hal() = default;

			

			virtual void init(int speed) = 0;
			virtual void startSignal() = 0;
			virtual void stopSignal() = 0;
			virtual void readRequest(int address) = 0;
			virtual void writeRequest(int address) = 0;
			virtual void writeData(int data) = 0;
			virtual uint8_t readData() = 0;
	};
}