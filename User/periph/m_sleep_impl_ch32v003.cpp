#include "m_sleep.hpp"

#include "m_pair_finder.hpp"

#include "ch32v00x.h"



#define _LSI_CLOCK_HZ 128000



void periph::sleep::init()
{
	EXTI_InitTypeDef EXTI_InitStructure = {0};

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);

	EXTI_InitStructure.EXTI_Line = EXTI_Line9;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Event;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);
}

void periph::sleep::sleepImpl(uint32_t prescaller, uint32_t count)
{
	//stop timer and reset it if was running
	PWR_AutoWakeUpCmd(DISABLE);

	RCC_LSICmd(ENABLE);
	while(RCC_GetFlagStatus(RCC_FLAG_LSIRDY) == RESET);

	PWR_AWU_SetPrescaler(prescaller);
	PWR_AWU_SetWindowValue(count);
	PWR_AutoWakeUpCmd(ENABLE);

	//sleep
	PWR_EnterSTANDBYMode(PWR_STANDBYEntry_WFE);

	//reinit clock settings if needed
}