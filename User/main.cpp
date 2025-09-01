#include "ch32v00x.h"

#include <stdint.h>
#include <string.h>
#include <stdlib.h>

#include "debug.h"

#include "m_i2c.hpp"
#include "m_i2c_init.hpp"
#include "m_hal_ssd1315.hpp"
#include "m_buffered_display.hpp"
#include "m_font_writer.hpp"




static periph::I2C_Peripheral i2c{
	&periph::I2C_Init,
	&periph::I2C_StartSequence,
	&periph::I2C_ReadRequest,
	&periph::I2C_WriteRequest,
	&periph::I2C_WriteByte,
	&periph::I2C_StopSequence
};

static display::HalDisplaySSD1315 ssd1315(i2c);
static display::PartitionBufferedWriter<128> writer(ssd1315);
static display::FontWriter font(writer);



// void USARTx_CFG (void) {
//     GPIO_InitTypeDef GPIO_InitStructure = {0};
//     USART_InitTypeDef USART_InitStructure = {0};

//     RCC_APB2PeriphClockCmd (RCC_APB2Periph_GPIOD | RCC_APB2Periph_USART1, ENABLE);

//     /* USART1 TX-->D.5   RX-->D.6 */
//     GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
//     GPIO_InitStructure.GPIO_Speed = GPIO_Speed_30MHz;
//     GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
//     GPIO_Init (GPIOD, &GPIO_InitStructure);
//     GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
//     GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
//     GPIO_Init (GPIOD, &GPIO_InitStructure);

//     USART_InitStructure.USART_BaudRate = 115200;
//     USART_InitStructure.USART_WordLength = USART_WordLength_8b;
//     USART_InitStructure.USART_StopBits = USART_StopBits_1;
//     USART_InitStructure.USART_Parity = USART_Parity_No;
//     USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
//     USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;

//     USART_Init (USART1, &USART_InitStructure);
//     USART_Cmd (USART1, ENABLE);
// }

int main (void) {
    SystemCoreClockUpdate();
    Delay_Init();
#if (SDI_PRINT == SDI_PR_OPEN)
    SDI_Printf_Enable();
#else
    USART_Printf_Init (115200);
#endif
    printf ("SystemClk:%u\r\n", SystemCoreClock);
    printf ("ChipID:%08x\r\n", DBGMCU_GetCHIPID());

    // USARTx_CFG();

	i2c.init(100000);
	
	ssd1315.init();
	ssd1315.clearScreen();

	writer.addDrawAction([](auto& w){
		w.drawRectangle(0, 16, 16, 8);
		w.setPixel(127, 0, true);
		w.setPixel(126, 1, true);
		w.setPixel(125, 2, true);
		w.setPixel(124, 3, true);
		w.setPixel(123, 4, true);
		w.setPixel(122, 5, true);
		w.setPixel(121, 6, true);
		w.setPixel(120, 7, true);

		w.setPixel(127, 7, true);
		w.setPixel(126, 6, true);
		w.setPixel(125, 5, true);
		w.setPixel(124, 4, true);
		w.setPixel(123, 3, true);
		w.setPixel(122, 2, true);
		w.setPixel(121, 1, true);
		w.setPixel(120, 0, true);

		w.drawLine(120, 8, 127, 15);
		w.drawLine(120, 15, 127, 8);

		font.changeSize<display::FontWriter::FontSize::MEDIUM>();
		font.drawStr(0, 0, "%+-_*!@#");

		font.changeSize<display::FontWriter::FontSize::SMALL>();
		font.drawStr(0, 32, "+ 0123456789 -");

		font.changeSize<display::FontWriter::FontSize::MEDIUM>();
		font.drawStr(64, 48, "ABCDEZ");
	});


	writer.flush();

    while (1) {
        // printf ("Cycle\r\n");
    }
}