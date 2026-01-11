#include "m_battery.hpp"



using namespace periph;



#define _MAX_ADC 8



struct _Entry
{
	int pin;
	GPIO_TypeDef* reg;
};



static const _Entry _MAPPER[_MAX_ADC] = {
	{GPIO_Pin_2, GPIOA},	
	{GPIO_Pin_1, GPIOA},	
	{GPIO_Pin_4, GPIOC},	
	{GPIO_Pin_2, GPIOD},	
	{GPIO_Pin_3, GPIOD},	
	{GPIO_Pin_5, GPIOD},	
	{GPIO_Pin_6, GPIOD},	
	{GPIO_Pin_4, GPIOD},	
};



Battery::Battery(uint8_t adcChannel)
	: _adcChannel(adcChannel)
{

}

void Battery::init()
{
	if(_MAPPER[_adcChannel].reg == GPIOA){
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

	}else if(_MAPPER[_adcChannel].reg == GPIOD){
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);

	}else{
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	}

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
	RCC_ADCCLKConfig(RCC_PCLK2_Div8);

	ADC_InitTypeDef adc_cfg = {
		.ADC_Mode = ADC_Mode_Independent,
		.ADC_ScanConvMode = DISABLE,
		.ADC_ContinuousConvMode = DISABLE,
		.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None,
		.ADC_DataAlign = ADC_DataAlign_Right,
		.ADC_NbrOfChannel = 1,
	};
	ADC_Init(ADC1, &adc_cfg);

	ADC_Calibration_Vol(ADC1, ADC_CALVOL_50PERCENT);
	ADC_Cmd(ADC1, ENABLE);
	
	ADC_ResetCalibration(ADC1);
	while(ADC_GetResetCalibrationStatus(ADC1));

	ADC_StartCalibration(ADC1);
	while(ADC_GetCalibrationStatus(ADC1));

	// NVIC_InitTypeDef irq_cfg = {
	// 	.NVIC_IRQChannel = ADC_IRQn,
	// 	.NVIC_IRQChannelPreemptionPriority = 0,
	// 	.NVIC_IRQChannelSubPriority = 1,
	// 	.NVIC_IRQChannelCmd = ENABLE,
	// };
	// NVIC_Init(&irq_cfg);
	// ADC_ITConfig(ADC1, ADC_IT_EOC, ENABLE);

	//init GPIO
	GPIO_InitTypeDef gpio_cfg = {
		.GPIO_Pin = _MAPPER[_adcChannel].pin,
		.GPIO_Speed = GPIO_Speed_10MHz,
		.GPIO_Mode = GPIO_Mode_AIN,
	};

	GPIO_Init(_MAPPER[_adcChannel].reg, &gpio_cfg);
}

uint16_t Battery::readRaw()
{
	ADC_RegularChannelConfig(ADC1, _adcChannel, 1, ADC_SampleTime_241Cycles);
	ADC_ClearFlag(ADC1, ADC_FLAG_EOC);
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);
	while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC));

	return ADC_GetConversionValue(ADC1);
}

uint8_t Battery::readPercents()
{
	uint32_t tmp = readRaw() - _minRaw;
	tmp *= 100;
	tmp /= _maxRaw - _minRaw;
	
	return tmp;
}