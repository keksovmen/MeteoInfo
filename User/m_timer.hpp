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

			void start()
			{
				_lastTick = periph::sys_time::currentTick();
			}

			/**
			 * @brief 
			 * 
			 * @return true still counting
			 * @return false time is out
			 */

			bool update()
			{
				if(isEnded()){
					return false;
				}
				
				auto delta = periph::sys_time::currentTick() - _lastTick;
				_currentTimeMs += periph::sys_time::toMs(delta);
				_lastTick = periph::sys_time::currentTick();

				return !isEnded();
			}

			bool isEnded()
			{
				return _currentTimeMs >= _periodMs;
			}

			void reset()
			{
				_currentTimeMs = 0;
			}

		private:
			periph::sys_time::time_val _periodMs;
			periph::sys_time::time_val _currentTimeMs = 0;
			periph::sys_time::time_val _lastTick = 0;


	};
}