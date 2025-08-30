#include "ch32v00x.h"

#include <stdint.h>
#include <string.h>
#include <stdlib.h>

#include "debug.h"
#include "k_font.h"

#include "m_i2c.hpp"
#include "m_hal_ssd1315.hpp"
#include "m_buffered_display.hpp"
#include "m_font_writer.hpp"



// ���է�֧�? �էڧ��ݧ֧� �ߧ� ��ڧߧ�? I2C
#define SSD1315_ADDRESS 0x3C
// #define SSD1315_ADDRESS 0x38

// ���ѧ٧ާ֧��? �էڧ��ݧ֧� (128x64)
#define SSD1315_WIDTH 128
#define SSD1315_HEIGHT 64


// ��������ڧ�� ���ߧܧ�ڧ�?
void IIC_Init (int bound);
void I2C_Start (void);
void I2C_Stop (void);
void I2C_WriteRequest (int address);
void I2C_WriteByte (uint8_t data);
void SSD1315_WriteCommand (uint8_t command);
void SSD1315_WriteData (uint8_t data);
void SSD1315_Init (void);
void SSD1315_Clear (void);
void SSD1315_DrawSquare (uint8_t x, uint8_t y, uint8_t width, uint8_t height);
void Delay_ms (uint32_t ms);
void draw_str(uint8_t x, uint8_t y, const char* str);



static periph::I2C_Peripheral i2c{&IIC_Init, &I2C_Start, &I2C_WriteRequest, &I2C_WriteRequest, &I2C_WriteByte, &I2C_Stop};
static display::HalDisplaySSD1315 ssd1315(i2c);
static display::PartitionBufferedWriter<128> writer(ssd1315);
static display::FontWriter font(writer);



void USARTx_CFG (void) {
    GPIO_InitTypeDef GPIO_InitStructure = {0};
    USART_InitTypeDef USART_InitStructure = {0};

    RCC_APB2PeriphClockCmd (RCC_APB2Periph_GPIOD | RCC_APB2Periph_USART1, ENABLE);

    /* USART1 TX-->D.5   RX-->D.6 */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_30MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init (GPIOD, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init (GPIOD, &GPIO_InitStructure);

    USART_InitStructure.USART_BaudRate = 115200;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;

    USART_Init (USART1, &USART_InitStructure);
    USART_Cmd (USART1, ENABLE);
}

int main (void) {
    // ���ߧڧ�ڧѧݧڧ٧ѧ�ڧ� ��ڧ��֧ާ�?
    // SystemInit();

    // NVIC_PriorityGroupConfig (NVIC_PriorityGroup_1);
    SystemCoreClockUpdate();
    Delay_Init();
#if (SDI_PRINT == SDI_PR_OPEN)
    SDI_Printf_Enable();
#else
    USART_Printf_Init (115200);
#endif
    printf ("SystemClk:%u\r\n", SystemCoreClock);
    printf ("ChipID:%08x\r\n", DBGMCU_GetCHIPID());

    USARTx_CFG();

	// periph::I2C_Peripheral i2c(&IIC_Init, &I2C_Start, NULL, &I2C_WriteRequest, &I2C_WriteByte, &I2C_Stop);

	i2c.init(100000);
    // ���ߧڧ�ڧѧݧڧ٧ѧ�ڧ� I2C
    // IIC_Init (100000, 0);

    // ���ߧڧ�ڧѧݧڧ٧ѧ�ڧ� �էڧ��ݧ֧�
	
	ssd1315.init();
    // SSD1315_Init();

    // ����ڧ��ܧ�? �էڧ��ݧ֧�
    // SSD1315_Clear();
	ssd1315.clearScreen();

    // ���ڧ��֧� �ܧӧѧէ�ѧ�? �� ��֧�֧էڧߧ� ��ܧ�ѧߧ�
    // ���ѧ٧ާ֧� �ܧӧѧէ�ѧ�� 40x40 ��ڧܧ�֧ݧ֧�
    // uint8_t square_size = 40;
    // uint8_t x_pos = (SSD1315_WIDTH - square_size) / 2;
    // uint8_t y_pos = (SSD1315_HEIGHT - square_size) / 2;

	// uint8_t tmp[128];
	// memset(tmp, 0xFF, sizeof(tmp));

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

	// ssd1315.drawRegion(110, 0, 10, 5, tmp);
    // SSD1315_DrawSquare (10, 5, 20, 7);


	writer.flush();

    while (1) {
        // ���֧�ܧ�ߧ֧�ߧ���? ��ڧܧ�?
        // printf ("Cycle\r\n");
    }
}

// ���ߧڧ�ڧѧݧڧ٧ѧ�ڧ� I2C
void IIC_Init (int bound) {
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

void I2C_Start(void)
{
	while (I2C_GetFlagStatus (I2C1, I2C_FLAG_BUSY))
        ;
    I2C_GenerateSTART (I2C1, ENABLE);

    while (!I2C_CheckEvent (I2C1, I2C_EVENT_MASTER_MODE_SELECT))
        ;
}

void I2C_Stop(void)
{
	I2C_GenerateSTOP (I2C1, ENABLE);
}

void I2C_WriteRequest (int address)
{
	I2C_Send7bitAddress (I2C1, address, I2C_Direction_Transmitter);
    while (!I2C_CheckEvent (I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
        ;
}

void I2C_WriteByte (uint8_t data)
{
	I2C_SendData (I2C1, data);  // ���ѧާ� �ܧ�ާѧߧէ�?
    while (!I2C_CheckEvent (I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
        ;
}


// ������ѧӧܧ�? �ܧ�ާѧߧէ�? �ߧ� �էڧ��ݧ֧�
void SSD1315_WriteCommand (uint8_t command) {
	i2c.startSignal();
	i2c.writeRequest(SSD1315_ADDRESS);
	i2c.writeData(0x00);
	i2c.writeData(command);
	i2c.stopSignal();

    // ������ѧӧܧ�? �ܧ�ߧ���ݧ�ߧ�ԧ� �ҧѧۧ��? (Co=0, D/C=0)
    // while (I2C_GetFlagStatus (I2C1, I2C_FLAG_BUSY))
    //     ;
    // I2C_GenerateSTART (I2C1, ENABLE);
    // while (!I2C_CheckEvent (I2C1, I2C_EVENT_MASTER_MODE_SELECT))
    //     ;

    // I2C_Send7bitAddress (I2C1, SSD1315_ADDRESS << 1, I2C_Direction_Transmitter);
    // while (!I2C_CheckEvent (I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
    //     ;

    // I2C_SendData (I2C1, 0x00);  // Control byte - �ܧ�ާѧߧէ�?
    // while (!I2C_CheckEvent (I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
    //     ;

    // I2C_SendData (I2C1, command);  // ���ѧާ� �ܧ�ާѧߧէ�?
    // while (!I2C_CheckEvent (I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
    //     ;

    // I2C_GenerateSTOP (I2C1, ENABLE);
}

// ������ѧӧܧ�? �էѧߧߧ��� �ߧ� �էڧ��ݧ֧�
void SSD1315_WriteData (uint8_t data) {
	i2c.startSignal();
	i2c.writeRequest(SSD1315_ADDRESS);
	i2c.writeData(0x40);
	i2c.writeData(data);
	i2c.stopSignal();

    // ������ѧӧܧ�? �ܧ�ߧ���ݧ�ߧ�ԧ� �ҧѧۧ��? (Co=0, D/C=1)
    // while (I2C_GetFlagStatus (I2C1, I2C_FLAG_BUSY))
    //     ;
    // I2C_GenerateSTART (I2C1, ENABLE);
    // while (!I2C_CheckEvent (I2C1, I2C_EVENT_MASTER_MODE_SELECT))
    //     ;

    // I2C_Send7bitAddress (I2C1, SSD1315_ADDRESS << 1, I2C_Direction_Transmitter);
    // while (!I2C_CheckEvent (I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
    //     ;

    // I2C_SendData (I2C1, 0x40);  // Control byte - �էѧߧߧ���
    // while (!I2C_CheckEvent (I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
    //     ;

    // I2C_SendData (I2C1, data);  // ���ѧߧߧ���
    // while (!I2C_CheckEvent (I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
    //     ;

    // I2C_GenerateSTOP (I2C1, ENABLE);
}

// ���ߧڧ�ڧѧݧڧ٧ѧ�ڧ� �էڧ��ݧ֧� SSD1315
void SSD1315_Init (void) {
    Delay_Ms (100);  // ���ѧ�٧�? ����ݧ�? ���էѧ��? ��ڧ�ѧߧڧ�

    // �����ݧ֧է�ӧѧ�֧ݧ�ߧ���� �ܧ�ާѧߧ�? �ڧߧڧ�ڧѧݧڧ٧ѧ�ڧ�
    SSD1315_WriteCommand (0xAE);  // Display OFF

    SSD1315_WriteCommand (0x20);  // Set Memory Addressing Mode
    SSD1315_WriteCommand (0x00);  // Horizontal addressing mode

    // SSD1315_WriteCommand (0xB0);  // Set Page Start Address
    SSD1315_WriteCommand (0xC8);  // Set COM Output Scan Direction

    // SSD1315_WriteCommand (0x00);  // Set lower column address
    // SSD1315_WriteCommand (0x10);  // Set higher column address

    SSD1315_WriteCommand (0x40);  // Set start line address
    SSD1315_WriteCommand (0x81);  // Set contrast control
    SSD1315_WriteCommand (0x7F);  // Contrast value (50%)

    SSD1315_WriteCommand (0xA1);  // Set segment re-map
    SSD1315_WriteCommand (0xA6);  // Set normal display
    SSD1315_WriteCommand (0xA8);  // Set multiplex ratio
    SSD1315_WriteCommand (0x3F);  // 64MUX

    SSD1315_WriteCommand (0xA4);  // Output follows RAM content
    SSD1315_WriteCommand (0xD3);  // Set display offset
    SSD1315_WriteCommand (0x00);  // No offset

    SSD1315_WriteCommand (0xD5);  // Set display clock divide ratio
    SSD1315_WriteCommand (0x80);  // Default ratio

    SSD1315_WriteCommand (0xD9);  // Set pre-charge period
    SSD1315_WriteCommand (0xF1);  // Phase 1 = 15, Phase 2 = 1

    SSD1315_WriteCommand (0xDA);  // Set COM pins hardware configuration
    SSD1315_WriteCommand (0x12);  // Alternative COM pin configuration

    SSD1315_WriteCommand (0xDB);  // Set VCOMH
    SSD1315_WriteCommand (0x40);  // VCOMH = 0.83 * VCC

    SSD1315_WriteCommand (0x8D);  // Set DC-DC enable
    SSD1315_WriteCommand (0x14);  // Enable charge pump

    SSD1315_WriteCommand (0xAF);  // Display ON

    Delay_Ms (10);
}

static void SSD1315_Set_Range(uint8_t x, uint8_t y, uint8_t width, uint8_t height)
{
    //TODO: add assertions

    //set low and high column window (X coordinate range)
    SSD1315_WriteCommand (0x21);    //cmd to start
    SSD1315_WriteCommand (x);    //low
    SSD1315_WriteCommand (x + width - 1);    //high

    //set low and high column window (Y coordinate range)
    SSD1315_WriteCommand (0x22);    //cmd to start
    SSD1315_WriteCommand (y / 8);    //low
    SSD1315_WriteCommand ((y + height + 7) / 8);    //high

}

// static void transpose_8x8_bitmap(const uint8_t *input, uint8_t *output)
// {
// 	// Clear the output array
// 	for (uint8_t col = 0; col < 8; col++) {
// 		output[col] = 0;
// 	}

// 	// For each row and each bit in the row...
// 	for (uint8_t row = 0; row < 8; row++) {
// 		uint8_t current_byte = input[row];
// 		// For each bit (column) in this row, check if it's set.
// 		for (uint8_t col_bit = 0; col_bit < 8; col_bit++) {
// 		// If the bit is set in the original row...
// 			if (current_byte & (1 << (7 - col_bit))) { 
// 				// ...set the corresponding bit in the output column.
// 				output[col_bit] |= (1 << row);
// 			}
// 		}
// 	}
// }

static void draw_char(uint8_t x, uint8_t y, char c)
{
    // uint8_t buffer[8] = {0};
    // transpose_8x8_bitmap(k_font_get_glyph(c), buffer);

    // SSD1315_Set_Range(x, y, K_FONT_GLYPH_WIDTH, K_FONT_GLYPH_HEIGHT);
    // for (uint8_t row = 0; row < (K_FONT_GLYPH_HEIGHT + 7) / 8; row++) {
        // transpose_8x8_bitmap(k_font_get_glyph(c) + row * 8, buffer);
        // for (uint8_t i = 0; i < K_FONT_GLYPH_WIDTH; i++) {
            // SSD1315_WriteData(buffer[i]);
        // }
		// writer.drawBitmap(x, y + row * 8, K_FONT_GLYPH_WIDTH, 8, k_font_get_glyph(c) + row * 8);
		writer.drawBitmap(x, y, K_FONT_GLYPH_WIDTH, K_FONT_GLYPH_HEIGHT, k_font_get_glyph(c));
    // }

	// writer.flush();

	// ssd1315.drawRegion(x, y, K_FONT_GLYPH_WIDTH, K_FONT_GLYPH_HEIGHT, buffer);
}

void draw_str(uint8_t x, uint8_t y, const char* str)
{
    for (size_t i = 0; i < strlen(str); i++) {
        draw_char(x + i * K_FONT_GLYPH_WIDTH, y, str[i]);
    }
}
// ����ڧ��ܧ�? �էڧ��ݧ֧�
void SSD1315_Clear (void) {
    // SSD1315_Set_Range(0, 0, SSD1315_WIDTH, SSD1315_HEIGHT);


    // for (uint8_t page = 0; page < 8; page++) {
    //     SSD1315_WriteCommand (0xB0 + page);  // Set page address
    //     SSD1315_WriteCommand (0x00);         // Set lower column address
    //     SSD1315_WriteCommand (0x10);         // Set higher column address

    // for (uint16_t i = 0; i < SSD1315_WIDTH * SSD1315_HEIGHT / 8; i++) {
    //     SSD1315_WriteData (0x00);  // Clear all pixels
    // }
    // }
	ssd1315.clearScreen();
}

// ���ڧ��ӧѧߧڧ� �ܧӧѧէ�ѧ��
void SSD1315_DrawSquare (uint8_t x, uint8_t y, uint8_t width, uint8_t height) {
    // ���ҧ֧էڧާ��?, ���� �ܧ���էڧߧѧ�� �� ��ѧ٧ާ֧�� �� ���֧է֧ݧѧ� ��ܧ�ѧߧ�
    // if (x >= SSD1315_WIDTH || y >= SSD1315_HEIGHT)
    //     return;
    // if (x + width > SSD1315_WIDTH)
    //     width = SSD1315_WIDTH - x;
    // if (y + height > SSD1315_HEIGHT)
    //     height = SSD1315_HEIGHT - y;

    SSD1315_Set_Range(x, y, width, height);
    for (uint8_t row = 0; row < (height + 7) / 8; row++) {
        for (uint8_t i = 0; i < width; i++) {
            SSD1315_WriteData(0xFF);
        }
    }

    // ���ڧ��֧� �ܧӧѧէ�ѧ�?
    // for (uint8_t row = y; row < y + height; row++) {
    //     uint8_t page = row / 8;
    //     uint8_t bit_mask = 1 << (row % 8);

    //     // �����ѧߧ�ӧܧ�? ���٧ڧ�ڧ�?
    //     SSD1315_WriteCommand (0xB0 + page);               // Set page
    //     SSD1315_WriteCommand (0x00 + (x & 0x0F));         // Set lower column
    //     SSD1315_WriteCommand (0x10 + ((x >> 4) & 0x0F));  // Set higher column

    //     // ���ڧ��֧� �ԧ��ڧ٧�ߧ�ѧݧ�ߧ�� �ݧڧߧڧ�
    //     for (uint8_t col = x; col < x + width; col++) {
    //         SSD1315_WriteData (bit_mask);
    //     }
    // }
}

// �������ѧ� ���ߧܧ�ڧ�? �٧ѧէ֧�اܧ�?
// void Delay_ms (uint32_t ms) {
//     for (uint32_t i = 0; i < ms; i++) {
//         for (uint32_t j = 0; j < 2400; j++) {
//             __asm__ ("nop");
//         }
//     }
// }