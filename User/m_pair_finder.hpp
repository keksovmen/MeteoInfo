#pragma once



#include <array>
#include <cmath>
#include <limits>
#include <stdint.h>



namespace util
{
	struct TimingPair {
		int i;
		int divider;
		uint32_t count;
		float actual_time;
		float deviation;
	};



	template<int N>
	constexpr TimingPair findBestTimingPair(uint32_t target_time_ms, uint32_t base_frequency, uint32_t maxCount, const std::array<int, N>& dividers) {
		// constexpr float base_frequency = baseFreq;
		float min_deviation = std::numeric_limits<float>::max();
		TimingPair best_pair{0, 0, 0, 0.0, 0.0};
		
		for (size_t i = 0; i < dividers.size(); i++) {
			int divider = dividers[i];
			float frequency = base_frequency / divider;
			float time_per_count = 1000.0f / frequency; // ms per count
			
			// Calculate ideal count for this divider
			float ideal_count = static_cast<float>(target_time_ms) / time_per_count;
			
			// Find the closest integer count in range [0, maxCount]
			uint32_t count = static_cast<uint32_t>(roundf(ideal_count));
			if (count < 0) count = 0;
			if (count > maxCount) count = maxCount;
			
			// Calculate actual time and deviation
			float actual_time = count * time_per_count;
			float deviation = std::abs(actual_time - static_cast<float>(target_time_ms));
			
			// Update best pair if this is better
			if (deviation < min_deviation) {
				min_deviation = deviation;
				best_pair = {static_cast<int>(i), divider, count, actual_time, deviation};
			}
		}
		
		return best_pair;
	}

}