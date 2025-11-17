#include "m_sys_time.hpp"

#include "ch32v00x.h"



#define _COUNTER (0xFFFF / 128)



using namespace periph;
using namespace sys_time;



static time_val _ticks = 0;
static time_val _tickRate = 0;



extern "C"
{
	void TIM2_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
	void TIM2_IRQHandler(void)
	{
		// printf("HERE\r\n");
		if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)
		{
			TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
			_ticks ++;
		}
	}
}



void periph::sys_time::init(time_val tickHz)
{
	_tickRate = tickHz;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

	TIM_TimeBaseInitTypeDef timerBase = {
		.TIM_Prescaler = static_cast<uint16_t>((SystemCoreClock / (_COUNTER * tickHz)) - 1),	//how much to divide the clock - 1
		.TIM_CounterMode = TIM_CounterMode_Up,
		.TIM_Period = _COUNTER,	//value + 1 that will be auto loaded and in working mode it will be decreased to 0 and then stop the timer
		.TIM_ClockDivision = TIM_CKD_DIV1,
		.TIM_RepetitionCounter = 0,
	};
	TIM_TimeBaseInit(TIM2, &timerBase);

	TIM_ClearITPendingBit(TIM2, TIM_IT_Update);

	NVIC_InitTypeDef interrupt = {
		.NVIC_IRQChannel = TIM2_IRQn,
		.NVIC_IRQChannelPreemptionPriority = 0,
		.NVIC_IRQChannelSubPriority = 0,
		.NVIC_IRQChannelCmd = ENABLE,
	};
	NVIC_Init(&interrupt);

	TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);

	TIM_Cmd(TIM2, ENABLE);
}

time_val periph::sys_time::currentMs()
{
	return toMs(_ticks);
}

time_val periph::sys_time::currentTick()
{
	return _ticks;
}

time_val periph::sys_time::toMs(time_val ticks)
{
	return ticks * (1000 / _tickRate);
}