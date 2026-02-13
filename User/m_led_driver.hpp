#pragma once


#include <array>
#include <algorithm>
#include <random>

#include "m_pwm.hpp"
#include "m_sys_time.hpp"
#include "m_timer.hpp"



namespace periph
{
	enum class LedDriverMode : uint8_t
	{
		SLOW = 0,
		NORMAL,
		FAST,
	};



	template<periph::Pwm::Pin... Pins>
	class LedDriver
	{
		public:
			LedDriver(periph::Pwm& pwm)
				: _pwm(pwm)
			{

			}

			/**
			 * @brief must call 2550 times per second as main loop
			 */

			void tick()
			{
				for(_Entry& e : _entries){
					uint8_t duty = e.function(e.tick++, e.multiplier, &e.store);
					_pwm.writePin(e.pin, duty);

					// switch pattern logic here
					if(e.tick == e.finishTick){
						const uint8_t durationMul = _durationDist(_rand);
						const uint8_t patternDurationMul = std::max(durationMul, _patternDurDist(_rand));
						const uint8_t repeatCount = patternDurationMul / durationMul;

						e.tick = 0;
						e.store = 0;
						e.multiplier = durationMul;
						e.finishTick = static_cast<uint32_t>(durationMul) * repeatCount * 255u;	// * 255 is PWM resolution
					}
				}
			}

			void restart()
			{
				//seed random device
				_rand = std::minstd_rand(sys_time::currentTick());
				for(_Entry& e : _entries){
					e = _Entry{e.pin};
				}
			}

			void setMode(LedDriverMode mode)
			{
				const uint8_t modeMap[3][4] = {
					{20, 100, 20, 255},	//slow
					{2, 50, 5, 100},	//normal
					{2, 15, 5, 80},		//fast
				};

				const uint8_t index = static_cast<uint8_t>(mode);
				_durationDist = std::uniform_int_distribution<uint8_t>(modeMap[index][0], modeMap[index][1]);
				_patternDurDist = std::uniform_int_distribution<uint8_t>(modeMap[index][2], modeMap[index][3]);

				_mode = mode;
			}

		private:
			using _Function = uint8_t(*)(uint32_t tick, uint8_t multiplier, uint32_t* data);



			struct _Entry
			{
				periph::Pwm::Pin pin;
				_Function function;
				uint32_t tick;
				uint32_t store;
				uint8_t multiplier;
				uint32_t finishTick;


				_Entry(periph::Pwm::Pin pwmPin)
					: pin(pwmPin), function(&_triangle), tick(0), store(0), multiplier(10), finishTick(2550)
				{

				}
			};
		
		private:
			static constexpr std::size_t N = sizeof...(Pins);



			periph::Pwm& _pwm;
			std::array<_Entry, N> _entries{Pins...};
			std::minstd_rand _rand;
			std::uniform_int_distribution<uint8_t> _durationDist;
			std::uniform_int_distribution<uint8_t> _patternDurDist;
			LedDriverMode _mode;



			static uint8_t _triangle(uint32_t tick, uint8_t multiplier, uint32_t* data)
			{
				const bool swap = (tick % (multiplier * 128u)) == 0;
				const uint16_t result = (tick / multiplier) % 256u;
				if(swap){
					*data = (*data == 0) ? 1u : 0u;
				}

				return (*data == 1) ? result : 255u - result;
			}

			static uint8_t _line(uint32_t tick, uint8_t multiplier, uint32_t* data)
			{
				const uint16_t result = (tick / multiplier) % 256u;

				return result;
			}

			static uint8_t _blink(uint32_t tick, uint8_t multiplier, uint32_t* data)
			{
				const uint16_t result = (tick / multiplier) % 256u;

				return (result > 127u) ? 255u : 0u;
			}

			static uint8_t _on(uint32_t tick, uint8_t multiplier, uint32_t* data)
			{
				return 255u;
			}

			static uint8_t _off(uint32_t tick, uint8_t multiplier, uint32_t* data)
			{
				return 0u;
			}
	};
}