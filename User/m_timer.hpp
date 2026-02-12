#pragma once



#include "m_sys_time.hpp"



namespace timers
{
	class TimerCounter
	{
		public:
			TimerCounter(periph::sys_time::time_val periodMs)
				: _periodMs(periodMs)
			{

			}

			operator bool() const
			{
				return update();
			}

			/**
			 * @brief 
			 * 
			 * @return true timer is expired
			 * @return false still runing
			 */

			bool update() const
			{
				const auto delta = periph::sys_time::currentTick() - _startTick;

				return periph::sys_time::toMs(delta) >= _periodMs;
			}

			void reset()
			{
				_startTick = periph::sys_time::currentTick();
			}

			void changePeriod(periph::sys_time::time_val periodMs)
			{
				_periodMs = periodMs;
			}

		private:
			periph::sys_time::time_val _periodMs;
			periph::sys_time::time_val _startTick = 0;
	};
}