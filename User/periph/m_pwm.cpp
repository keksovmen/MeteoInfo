#include "m_pwm.hpp"



using namespace periph;



struct _Entry
{
	int channel;
	int pin;
	GPIO_TypeDef* port;
};



static const _Entry _MAP_DEFAULT[4] = {
	{TIM_Channel_1, GPIO_Pin_2, GPIOD},
	{TIM_Channel_2, GPIO_Pin_1, GPIOA},
	{TIM_Channel_3, GPIO_Pin_3, GPIOC},
	{TIM_Channel_4, GPIO_Pin_4, GPIOC},
};



Pwm::Pwm(int frequencyHz)
	: _frequencyHz(frequencyHz)
{

}

void Pwm::enable()
{
	//must be called before any commands
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);

	TIM_TimeBaseInitTypeDef timCfg = {0};
	timCfg.TIM_Period = 255;
	timCfg.TIM_Prescaler = SystemCoreClock / (timCfg.TIM_Period * _frequencyHz);	//convert frequency to prescaller 
	timCfg.TIM_ClockDivision = TIM_CKD_DIV1;
	timCfg.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM1, &timCfg);

	TIM_CtrlPWMOutputs(TIM1, ENABLE);
	TIM_ARRPreloadConfig(TIM1, ENABLE);
	TIM_Cmd(TIM1, ENABLE);
}

void Pwm::disable()
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, DISABLE);
	TIM_CtrlPWMOutputs(TIM1, DISABLE);
	TIM_ARRPreloadConfig(TIM1, DISABLE);
	TIM_Cmd(TIM1, DISABLE);
}

void Pwm::enablePin(Pin pin)
{
	//TODO: select proper periph clock
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOD, ENABLE );

	GPIO_InitTypeDef gpioCfg = {0};
	gpioCfg.GPIO_Pin = _MAP_DEFAULT[static_cast<int>(pin)].pin;
	gpioCfg.GPIO_Mode = GPIO_Mode_AF_PP;
	gpioCfg.GPIO_Speed = GPIO_Speed_30MHz;
	GPIO_Init(_MAP_DEFAULT[static_cast<int>(pin)].port, &gpioCfg);

	TIM_OCInitTypeDef ocrCfg = {0};
	ocrCfg.TIM_OCMode = TIM_OCMode_PWM1;
	ocrCfg.TIM_OutputState = TIM_OutputState_Enable;
	ocrCfg.TIM_Pulse = 0;
	ocrCfg.TIM_OCPolarity = TIM_OCPolarity_High;

	switch (pin)
	{
		case Pwm::Pin::PIN_0:
			TIM_OC1Init(TIM1, &ocrCfg);
			TIM_OC1PreloadConfig(TIM1, TIM_OCPreload_Disable);
			break;

		case Pwm::Pin::PIN_1:
			TIM_OC2Init(TIM1, &ocrCfg);
			TIM_OC2PreloadConfig(TIM1, TIM_OCPreload_Disable);
			break;

		case Pwm::Pin::PIN_2:
			TIM_OC3Init(TIM1, &ocrCfg);
			TIM_OC3PreloadConfig(TIM1, TIM_OCPreload_Disable);
			break;

		case Pwm::Pin::PIN_3:
			TIM_OC4Init(TIM1, &ocrCfg);
			TIM_OC4PreloadConfig(TIM1, TIM_OCPreload_Disable);
			break;
	}
}

void Pwm::disablePin(Pin pin)
{
	TIM_CCxCmd(TIM1, _MAP_DEFAULT[static_cast<int>(pin)].channel, TIM_CCx_Disable);
	GPIO_WriteBit(_MAP_DEFAULT[static_cast<int>(pin)].port, _MAP_DEFAULT[static_cast<int>(pin)].pin, Bit_RESET);
}

void Pwm::writePin(Pin pin, uint8_t val)
{
	switch (pin)
	{
		case Pwm::Pin::PIN_0:
			TIM_SetCompare1(TIM1, val);
			break;

		case Pwm::Pin::PIN_1:
			TIM_SetCompare2(TIM1, val);
			break;

		case Pwm::Pin::PIN_2:
			TIM_SetCompare3(TIM1, val);
			break;

		case Pwm::Pin::PIN_3:
			TIM_SetCompare4(TIM1, val);
			break;
	}
}
