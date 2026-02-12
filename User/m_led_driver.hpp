#pragma once


#include <array>
#include <algorithm>
#include <random>

#include "m_pwm.hpp"
#include "m_sys_time.hpp"
#include "m_timer.hpp"



namespace periph
{
	template<periph::Pwm::Pin... Pins>
	class LedDriver
	{
		public:
			enum class Mode
			{
				SLOW,
				NORMAL,
				FAST,
			};



			void tick()
			{
				for(_Entry& e : _entries){
					uint8_t duty = e.function(e.tick++, e.multiplier, &e.store);
					globals::getPwm().writePin(e.pin, duty);

					// switch pattern logic here
					if(e.tick == e.finishTick){
						const uint8_t durationMul = _durationDist(_rand);
						const uint8_t patternDurationMul = std::max(durationMul, _patternDurDist(_rand));
						const uint8_t repeatCount = patternDurationMul / durationMul;

						e.multiplier = durationMul;
						e.tick = 0;

						//better use tick counter for this purpose
						e.finishTick = static_cast<uint32_t>(durationMul) * repeatCount * 255u;
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

			void setMode(Mode mode)
			{
				switch (mode)
				{
					case Mode::SLOW:
						_durationDist = std::uniform_int_distribution<uint8_t>(10, 100);
						_patternDurDist = std::uniform_int_distribution<uint8_t>(20, 200);
						break;

					case Mode::NORMAL:
						_durationDist = std::uniform_int_distribution<uint8_t>(2, 50);
						_patternDurDist = std::uniform_int_distribution<uint8_t>(5, 100);
						break;

					case Mode::FAST:
						_durationDist = std::uniform_int_distribution<uint8_t>(2, 20);
						_patternDurDist = std::uniform_int_distribution<uint8_t>(5, 40);
						break;
				}

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




			std::array<_Entry, N> _entries{Pins...};
			std::minstd_rand _rand{0};
			std::uniform_int_distribution<uint8_t> _durationDist{2, 10};
			std::uniform_int_distribution<uint8_t> _patternDurDist{4, 20};
			Mode _mode = Mode::NORMAL;



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
	};
}