#pragma once


#include <array>
#include <algorithm>
#include <random>
#include <utility>

#include "m_pwm.hpp"
#include "m_sys_time.hpp"
#include "m_timer.hpp"



namespace periph
{
	enum class LedSpeedMode : uint8_t
	{
		SLOW = 0,
		NORMAL,
		FAST,
	};

	enum class LedPatternMode : uint8_t
	{
		RANDOM = 0,
		LINEAR,
		BLINK,
		ORIGINAL,
	};



	template<periph::Pwm::Pin... Pins>
	class LedDriver
	{
		public:
			LedDriver(periph::Pwm& pwm)
				: _pwm(pwm)
			{
				setSpeedMode(LedSpeedMode::NORMAL);
			}

			/**
			 * @brief must call 2550 times per second as main loop
			 */

			void tick()
			{
				for(_Entry& e : _entries){
					_callFunction(e);

					// switch pattern logic here
					if(e.tick == e.finishTick){
						const uint8_t durationMul = _durationDist(_rand);
						// const uint8_t patternDurationMul = std::max(durationMul, _patternDurDist(_rand));
						const uint8_t patternDurationMul = std::max(durationMul, durationMul);
						const uint8_t repeatCount = patternDurationMul / durationMul;

						e.function = _randomNextFunction(e.function);
						e.tick = 0;
						e.store = 0;
						e.multiplier = durationMul;
						e.finishTick = static_cast<uint32_t>(durationMul) * repeatCount * 256u;	// * 256 is PWM resolution
					}
				}
			}

			void restart()
			{
				//seed random device
				_rand = std::minstd_rand(sys_time::currentTick());

				//reset states
				for(_Entry& e : _entries){
					e = _Entry{e.pin};
					_pwm.writePin(e.pin, 0);
				}
			}

			void setSpeedMode(LedSpeedMode mode)
			{
				const uint8_t modeMap[3][4] = {
					{15, 60, 20, 255},	//slow
					{5, 30, 5, 100},	//normal
					{2, 10, 5, 80},		//fast
				};

				const uint8_t index = static_cast<uint8_t>(mode);
				_durationDist = std::uniform_int_distribution<uint8_t>(modeMap[index][0], modeMap[index][1]);
				_patternDurDist = std::uniform_int_distribution<uint8_t>(modeMap[index][2], modeMap[index][3]);

				_speedMode = mode;
			}

			void setPatternMode(LedPatternMode mode)
			{
				_patternMode = mode;
			}

		private:
			enum class WaveFunction : uint8_t
			{
				ASEND = 0,
				DESAND,
				TRIANGLE,
				ON,
				OFF,
				BLINK,
				MAX
			};



			struct _Entry
			{
				periph::Pwm::Pin pin;
				WaveFunction function;
				uint32_t tick;
				uint32_t store;
				uint8_t multiplier;
				uint32_t finishTick;


				_Entry(periph::Pwm::Pin pwmPin)
					: pin(pwmPin), function(WaveFunction::TRIANGLE), tick(0), store(0), multiplier(10), finishTick(2550)
				{

				}
			};


		
		private:
			periph::Pwm& _pwm;
			std::array<_Entry, sizeof...(Pins)> _entries{Pins...};
			std::minstd_rand _rand{};
			std::uniform_int_distribution<uint8_t> _durationDist{};
			std::uniform_int_distribution<uint8_t> _patternDurDist{};
			LedSpeedMode _speedMode{};
			LedPatternMode _patternMode{LedPatternMode::RANDOM};



			void _callFunction(_Entry& e)
			{
				uint8_t duty = 0;
				switch (e.function)
				{
					case WaveFunction::ASEND:
						duty = _asend(e.tick, e.multiplier, &e.store);
						break;

					case WaveFunction::DESAND:
						duty = _desand(e.tick, e.multiplier, &e.store);
						break;

					case WaveFunction::TRIANGLE:
						duty = _triangle(e.tick, e.multiplier, &e.store);
						break;
					case WaveFunction::ON:
						duty = _on(e.tick, e.multiplier, &e.store);
						break;
					case WaveFunction::OFF:
						duty = _off(e.tick, e.multiplier, &e.store);
						break;
					case WaveFunction::BLINK:
						duty = _blink(e.tick, e.multiplier, &e.store);
						break;
				}
				e.tick++;
				_pwm.writePin(e.pin, duty);
			}



			WaveFunction _randomNextFunction(WaveFunction current)
			{
				if(_patternMode == LedPatternMode::BLINK){
					return WaveFunction::BLINK;

				}else if(_patternMode == LedPatternMode::RANDOM){
					return static_cast<WaveFunction>(_durationDist(_rand,
						std::uniform_int_distribution<uint8_t>::param_type{0, static_cast<uint8_t>(WaveFunction::MAX) - 1}));
				}

				const WaveFunction _flatMap[] = {
					//ORIGINAL MODE
					//case: ASEND 
					WaveFunction::DESAND, WaveFunction::ON,
					//case: DESAND, TRIANGLE 
					WaveFunction::ASEND, WaveFunction::OFF, WaveFunction::BLINK, WaveFunction::TRIANGLE,
					//case: ON 
					WaveFunction::DESAND, WaveFunction::BLINK,
					//case: OFF
					WaveFunction::ASEND, WaveFunction::BLINK, WaveFunction::TRIANGLE,
					//case: BLINK
					WaveFunction::DESAND, WaveFunction::BLINK, WaveFunction::ON, WaveFunction::OFF,
					//LINEAR MODE
					//case: ASEND 
					WaveFunction::DESAND, WaveFunction::ON,
					//case: DESAND, TRIANGLE 
					WaveFunction::ASEND, WaveFunction::OFF, WaveFunction::TRIANGLE,
					//case: ON 
					WaveFunction::DESAND,
					//case: OFF
					WaveFunction::ASEND, WaveFunction::TRIANGLE
				};

				const std::pair<uint8_t, uint8_t> _rangeMap[] = {
					//ORIGINAL MODE
					{0, 1},		//ASEND
					{2, 5},		//DESAND
					{2, 5},		//TRIANGLE
					{6, 7},		//ON
					{8, 10},		//OFF
					{11, 14},	//BLINK
					//LINEAR MODE
					{15, 16},		//ASEND
					{17, 19},		//DESAND
					{17, 19},		//TRIANGLE
					{20, 20},		//ON
					{21, 22},		//OFF
					{20, 20},	//BLINK
				};

				const uint8_t index = static_cast<uint8_t>(current) + ((_patternMode == LedPatternMode::ORIGINAL) ? 0 : 6);
				// roll random index in flat map
				return _flatMap[_durationDist(_rand, std::uniform_int_distribution<uint8_t>::param_type{_rangeMap[index].first, _rangeMap[index].second})];
			}

			static uint8_t _triangle(uint32_t tick, uint8_t multiplier, uint32_t* data)
			{
				const bool swap = (tick % (multiplier * 128u)) == 0;
				const uint16_t result = (tick / multiplier) % 256u;
				if(swap){
					*data = (*data == 0) ? 1u : 0u;
				}

				return (*data == 1) ? result : 255u - result;
			}

			static uint8_t _asend(uint32_t tick, uint8_t multiplier, uint32_t* data)
			{
				const uint16_t result = (tick / multiplier) % 256u;

				return result;
			}

			static uint8_t _desand(uint32_t tick, uint8_t multiplier, uint32_t* data)
			{
				return 255u - _asend(tick, multiplier, data);
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