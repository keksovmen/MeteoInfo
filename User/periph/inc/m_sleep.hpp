#pragma once



#include <algorithm>
#include <stdint.h>

#include "m_pair_finder.hpp"



namespace periph
{
	namespace sleep
	{
		void init();
		void sleepImpl(uint32_t prescaller, uint32_t count);

		template<uint32_t MS>
		constexpr void sleepForMs()
		{
			constexpr std::array<int, 15> dividers = {1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096, 10240, 61440};
			constexpr auto v = util::findBestTimingPair<dividers.size()>(MS, 128000u, 0x3F, dividers);
			//check if deviation is in range of 5%
			static_assert((v.deviation <= (MS * 0.05f)));
			if constexpr (v.i == 0){
				sleepImpl(0, v.count);
			}else{
				sleepImpl(v.i + 1, v.count);
			}
		}
	}
}