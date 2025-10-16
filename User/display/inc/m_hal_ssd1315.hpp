#pragma once

#include <stdint.h>

#include "m_hal_display.hpp"
#include "m_i2c_hal.hpp"



namespace display
{
	class HalDisplaySSD1315 : public HalDisplayI
	{
		public:
			HalDisplaySSD1315(periph::I2C_Hal& i2c);

			//need i2c periph abstraction
			virtual bool init() override;

			virtual void turnOn() override;
			virtual void turnOff() override;
			virtual bool isTurnedOn() const override;

			virtual int getWidth() const override;
			virtual int getHeight() const override;

			//Will fire only single pixel and disable the whole column
			virtual void drawPixel(int x, int y) override;

			/**
			 * @brief 
			 * 
			 * @param x 
			 * @param y 
			 * @param width 
			 * @param height 
			 * @param data one byte = one column of 8 pixels
			 */
			virtual void drawRegion(int x, int y, int width, int height, uint8_t* data) override;
			virtual void clearRegion(int x, int y, int width, int height) override;
		
		private:
			periph::I2C_Hal& _i2c;

			bool _isOn = false;



			void _writeCommand(uint8_t cmd);
			void _writeData(uint8_t data);
			void _setDrawRect(uint8_t x, uint8_t y, uint8_t width, uint8_t height);
	};
}