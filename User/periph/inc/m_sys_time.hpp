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
		void init(uint32_t tickHz);

		uint32_t currentMs();

		// static uint32_t currentSec()
		// {
		// 	return currentMs() / 1000;
		// }

		uint32_t currentTick();
	}
}