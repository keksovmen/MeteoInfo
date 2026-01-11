#pragma once



#include "ch32v00x_adc.h"



namespace periph
{
	class Battery
	{
		public:
			//gpio and port will be deduced from the channel, but need to use proper tables for each MCU
			/**
			 * @brief Construct a new Battery object
			 * 
			 * @param adcChannel one of ADC_Channel_N defines in ch32v00x_adc.h
			 */
			Battery(uint8_t adcChannel);

			/**
			 * @brief do actual initialization here
			 */
			void init();

			/**
			 * @brief 
			 * 
			 * @return uint16_t what 10 bit ADC could get
			 */
			uint16_t readRaw();

			/**
			 * @brief converts value to percents
			 * 
			 * @return uint8_t [0; 100]
			 */
			uint8_t readPercents();

		private:
			const uint8_t _adcChannel;

			uint16_t _minRaw = 0;
			uint16_t _maxRaw = 1023;
	};
}