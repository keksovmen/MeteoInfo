#include "m_button.hpp"

#include "ch32v00x.h"



//for gpio init
#define _PORT GPIOD
#define _PIN GPIO_Pin_0
//for interrupt
#define _PORT_SOURCE GPIO_PortSourceGPIOD
#define _PIN_SOURCE GPIO_PinSource0
#define _LINE EXTI_Line0

#define _DEBOUNCE_TIME_MS 100



using namespace periph;
using namespace buttons;



//user configs
static sys_time::time_val _longPressDuration = 1000;
static std::function<void()> _stab;

//internal
static Event _event;
static bool _eventFlag = false;
static bool _isPressed = false;
static sys_time::time_val _pressedAt = 0;



static void _press()
{
	if(_isPressed){
		return;
	}

	_isPressed = true;
	_pressedAt = sys_time::currentTick();
}

static void _release()
{
	if(!_isPressed){
		return;
	}

	_isPressed = false;

	const sys_time::time_val durationMs = sys_time::toMs(sys_time::currentTick() - _pressedAt);

	//TODO: fix for deep sleep irq
	if(durationMs < _DEBOUNCE_TIME_MS){
		// bounce time
		return;
	}

	_event.duration = durationMs;
	_event.action = (durationMs > _longPressDuration) ? Action::LONG_RELEASED : Action::RELEASED;

	_eventFlag = true;
}



extern "C"{
	void EXTI7_0_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
	void EXTI7_0_IRQHandler(void)
	{
		if(EXTI_GetITStatus(_LINE) != RESET)
		{
			if(_stab){
				std::invoke(_stab);
			}


			//need somehow just create wakeup event not button event
			// _event.duration = 0;
			// _event.action = Action::RELEASED;
			// _eventFlag = true;

			// if(GPIO_ReadInputDataBit(_PORT, _PIN)){
			// 	_release();
			// }else{
			// 	_press();
			// }
			
			// do not clear???
			EXTI_ClearITPendingBit(_LINE);     /* Clear Flag */
		}
	}
}



void periph::buttons::init(sys_time::time_val longPressMs, std::function<void()>&& stab)
{
	_longPressDuration = longPressMs;
	std::construct_at(&_stab, std::forward<decltype(stab)>(stab));


	GPIO_InitTypeDef cfg = {
		.GPIO_Pin = _PIN,
		.GPIO_Speed = GPIO_Speed_10MHz,
		.GPIO_Mode = GPIO_Mode_IPU,
	};

	GPIO_Init(_PORT,  &cfg);
	GPIO_EXTILineConfig(_PORT_SOURCE, _PIN_SOURCE);

    NVIC_InitTypeDef nvic_cfg = {0};
	nvic_cfg.NVIC_IRQChannel = EXTI7_0_IRQn;
    nvic_cfg.NVIC_IRQChannelPreemptionPriority = 0;
    nvic_cfg.NVIC_IRQChannelSubPriority = 1;
    nvic_cfg.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&nvic_cfg);
}


bool periph::buttons::hasEvent()
{
	return _eventFlag;
}

Event periph::buttons::getLastEvent()
{
	_eventFlag = false;
	return _event;
}

void periph::buttons::enableIrq()
{
	EXTI_InitTypeDef ext_cfg = {0};
	ext_cfg.EXTI_Line = _LINE;
	ext_cfg.EXTI_Mode = EXTI_Mode_Interrupt;
	//didn't work out as expected
	// ext_cfg.EXTI_Mode = EXTI_Mode_Event;
	ext_cfg.EXTI_Trigger = EXTI_Trigger_Falling;
	ext_cfg.EXTI_LineCmd = ENABLE;
	EXTI_Init(&ext_cfg);
}

void periph::buttons::disableIqr()
{
	EXTI_DeInit();
}

bool periph::buttons::pull()
{
	if(GPIO_ReadInputDataBit(_PORT, _PIN)){
		_release();
	}else{
		_press();
	}

	return hasEvent();
}
