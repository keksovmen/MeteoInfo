#include "m_i2c_impl.hpp"

#include "ch32v00x.h"



using namespace periph;



void periph::i2c_init(int bound)
{
	GPIO_InitTypeDef GPIO_InitStructure = {0};
	I2C_InitTypeDef I2C_InitTSturcture = {0};

	RCC_APB2PeriphClockCmd (RCC_APB2Periph_GPIOC | RCC_APB2Periph_AFIO, ENABLE);
	RCC_APB1PeriphClockCmd (RCC_APB1Periph_I2C1, ENABLE);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_30MHz;
	GPIO_Init (GPIOC, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_30MHz;
	GPIO_Init (GPIOC, &GPIO_InitStructure);

	I2C_InitTSturcture.I2C_ClockSpeed = bound;
	I2C_InitTSturcture.I2C_Mode = I2C_Mode_I2C;
	I2C_InitTSturcture.I2C_DutyCycle = I2C_DutyCycle_2;
	I2C_InitTSturcture.I2C_OwnAddress1 = 0;
	I2C_InitTSturcture.I2C_Ack = I2C_Ack_Enable;
	I2C_InitTSturcture.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
	I2C_Init (I2C1, &I2C_InitTSturcture);

	I2C_Cmd (I2C1, ENABLE);
}

void periph::i2c_start_sequence(void)
{
	while (I2C_GetFlagStatus (I2C1, I2C_FLAG_BUSY))
		;

	I2C_GenerateSTART (I2C1, ENABLE);
	while (!I2C_CheckEvent (I2C1, I2C_EVENT_MASTER_MODE_SELECT))
		;
}

void periph::i2c_stop_sequence(void)
{
	I2C_GenerateSTOP (I2C1, ENABLE);
}

void periph::i2c_write_request(int address)
{
	I2C_Send7bitAddress (I2C1, address, I2C_Direction_Transmitter);
	while (!I2C_CheckEvent (I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
		;
}

void periph::i2c_read_request(int address)
{
	//TODO: fix direction
	I2C_Send7bitAddress (I2C1, address, I2C_Direction_Receiver);
	while (!I2C_CheckEvent (I2C1, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED))
		;
}

void periph::i2c_write_byte(uint8_t data)
{
	I2C_SendData (I2C1, data);
	while (!I2C_CheckEvent (I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
		;
}

uint8_t periph::i2c_read_byte()
{
	while (!I2C_CheckEvent (I2C1, I2C_EVENT_MASTER_BYTE_RECEIVED))
		;
	uint8_t result = I2C_ReceiveData (I2C1);

	return result;
}
