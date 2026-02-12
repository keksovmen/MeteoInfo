#pragma once



#include "ch32v00x_gpio.h"
#include "ch32v00x_tim.h"



namespace periph
{
	//TODO: made it accept timer as argument, so you could use all possible timers if needed
	//TODO: add swap pin modes
	class Pwm
	{
		public:
			//physical pin depends on current remmaping for now use default mapping
			enum class Pin
			{
				PIN_0 = 0,	//PD2
				PIN_1,	//PA1
				PIN_2,	//PC3
				PIN_3,	//PC4
			};


			/**
			 * @brief Construct a new Pwm object
			 * 
			 * @param frequencyHz [2; 94110]
			 */
			Pwm(int frequencyHz);

			void enable();
			void disable();

			void enablePin(Pin pin);
			void disablePin(Pin pin);

			/**
			 * @brief set duty ratio for given pin
			 * 
			 * @param pin 
			 * @param val [0; 255]
			 */
			void writePin(Pin pin, uint8_t val);
		
		private:
			const int _frequencyHz;
	};
}