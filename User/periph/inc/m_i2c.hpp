#pragma once



#include <functional>

#include <stdint.h>

#include "m_hal_i2c.hpp"



namespace periph
{
	class I2C_Peripheral : public I2C_Hal
	{
		public:
			using InitCb = void(*)(int speed);
			using WriteStartCb = void(*)();
			using WriteReadRequestCb = void(*)(int address);
			using WriteWriteRequestCb = void(*)(int address);
			using WriteDataCb = void(*)(uint8_t data);
			using ReadDataCb = uint8_t(*)();
			using WriteStopCb = void(*)();



			I2C_Peripheral(InitCb initCb,
				WriteStartCb startCb,
				WriteReadRequestCb readRequestCb,
				WriteWriteRequestCb writeRequestCb,
				WriteDataCb writeDataCb,
				ReadDataCb readDataCb,
				WriteStopCb stopCb)
					:
					_initCb(initCb),
					_startCb(startCb),
					_readRequestCb(readRequestCb),
					_writeRequestCb(writeRequestCb),
					_writeDataCb(writeDataCb),
					_readDataCb(readDataCb),
					_stopCb(stopCb)
				{
				}



			virtual void init(int speed) override { std::invoke(_initCb, speed); };
			virtual void startSignal() override { std::invoke(_startCb); };
			virtual void stopSignal() override { std::invoke(_stopCb); };
			virtual void readRequest(int address) override { std::invoke(_readRequestCb, (address << 1) | 1); };
			virtual void writeRequest(int address) override { std::invoke(_writeRequestCb, address << 1); };
			virtual void writeData(int data) override { std::invoke(_writeDataCb, data); };
			virtual uint8_t readData() override { return std::invoke(_readDataCb); };
			

		private:
			InitCb _initCb;
			WriteStartCb _startCb;
			WriteReadRequestCb _readRequestCb;
			WriteWriteRequestCb _writeRequestCb;
			WriteDataCb _writeDataCb;
			ReadDataCb _readDataCb;
			WriteStopCb _stopCb;

	};
}