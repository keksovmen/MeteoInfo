#pragma once



#include <stdint.h>



namespace periph
{
	// class SysTime
	// {
	// 	public:
	// 		static 
		
	// };



	namespace sys_time
	{
		using time_val = uint32_t;



		void init(time_val tickHz);

		time_val currentMs();

		time_val currentTick();
		time_val toMs(time_val ticks);
	}
}