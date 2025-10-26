#pragma once

#include <stdint.h>

#include "m_hal_display.hpp"
#include "m_i2c_hal.hpp"



namespace display
{
	class HalDisplaySSD1315 : public HalDisplayI<HalDisplaySSD1315>
	{
		public:
			using HalDisplayI<HalDisplaySSD1315>::init;
			using HalDisplayI<HalDisplaySSD1315>::turnOn;
			using HalDisplayI<HalDisplaySSD1315>::turnOff;
			using HalDisplayI<HalDisplaySSD1315>::isTurnedOn;
			using HalDisplayI<HalDisplaySSD1315>::getWidth;
			using HalDisplayI<HalDisplaySSD1315>::getHeight;
			using HalDisplayI<HalDisplaySSD1315>::drawPixel;
			using HalDisplayI<HalDisplaySSD1315>::drawRegion;
			using HalDisplayI<HalDisplaySSD1315>::clearRegion;
			using HalDisplayI<HalDisplaySSD1315>::clearScreen;



			HalDisplaySSD1315(periph::I2C_Hal& i2c);

			//need i2c periph abstraction
			bool _init();

			void _turnOn();
			void _turnOff();
			bool _isTurnedOn() const;

			int _getWidth() const;
			int _getHeight() const;

			//Will fire only single pixel and disable the whole column
			void _drawPixel(int x, int y);

			/**
			 * @brief 
			 * 
			 * @param x 
			 * @param y 
			 * @param width 
			 * @param height 
			 * @param data one byte = one column of 8 pixels
			 */
			void _drawRegion(int x, int y, int width, int height, uint8_t* data);
			void _clearRegion(int x, int y, int width, int height);
		
		private:
			periph::I2C_Hal& _i2c;

			bool _isOn = false;



			void _writeCommand(uint8_t cmd);
			void _writeData(uint8_t data);
			void _setDrawRect(uint8_t x, uint8_t y, uint8_t width, uint8_t height);
	};
}