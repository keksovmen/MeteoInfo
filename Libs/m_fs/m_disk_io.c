#include "m_disk_io.h"



#include "ch32v00x.h"
#include "ch32v00x_gpio.h"
#include "ch32v00x_spi.h"
#include "ch32v00x_rcc.h"
#include "debug.h"



// SD card SPI configuration (adjust pins according to your wiring)
#define _SD_SPI             SPI1
#define _SD_CS_PORT         GPIOC
#define _SD_CS_PIN          GPIO_Pin_0
#define _SD_SPI_CLK_PORT    GPIOC
#define _SD_SPI_CLK_PIN     GPIO_Pin_5
#define _SD_SPI_MISO_PORT   GPIOC
#define _SD_SPI_MISO_PIN    GPIO_Pin_7
#define _SD_SPI_MOSI_PORT   GPIOC
#define _SD_SPI_MOSI_PIN    GPIO_Pin_6

/* Definitions for MMC/SDC command */
#define CMD0	(0x40+0)	/* GO_IDLE_STATE */
#define CMD1	(0x40+1)	/* SEND_OP_COND (MMC) */
#define	ACMD41	(0xC0+41)	/* SEND_OP_COND (SDC) */
#define CMD8	(0x40+8)	/* SEND_IF_COND */
#define CMD16	(0x40+16)	/* SET_BLOCKLEN */
#define CMD17	(0x40+17)	/* READ_SINGLE_BLOCK */
#define CMD24	(0x40+24)	/* WRITE_BLOCK */
#define CMD55	(0x40+55)	/* APP_CMD */
#define CMD58	(0x40+58)	/* READ_OCR */

/* Card type flags (CardType) */
#define CT_MMC				0x01	/* MMC version 3 */
#define CT_SD1				0x02	/* SD version 1 */
#define CT_SD2				0x04	/* SD version 2+ */
#define CT_BLOCK			0x08	/* Block addressing */

#define BLOCK_SIZE M_FS_CONF_BLOCK_SIZE



static uint8_t _card_type = 0;
static uint8_t _buffer[BLOCK_SIZE];
static uint32_t _buffer_sector = 123456789;



//CS toggles
static void _set_cs_low(void) {
	GPIO_WriteBit(_SD_CS_PORT, _SD_CS_PIN, Bit_RESET);
}

static void _set_cs_high(void) {
	GPIO_WriteBit(_SD_CS_PORT, _SD_CS_PIN, Bit_SET);
}

static uint8_t _is_miso_high()
{
	return GPIO_ReadInputDataBit(_SD_SPI_MISO_PORT, _SD_SPI_MISO_PIN);
}

// SPI functions
static void _spi_init(void) {
	SPI_InitTypeDef SPI_InitStructure = {0};
	GPIO_InitTypeDef GPIO_InitStructure = {0};
	
	// Enable clocks
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC | RCC_APB2Periph_SPI1, ENABLE);
	
	// Configure SPI pins
	GPIO_InitStructure.GPIO_Pin = _SD_SPI_CLK_PIN | _SD_SPI_MOSI_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_Init(_SD_SPI_CLK_PORT, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = _SD_SPI_MISO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(_SD_SPI_MISO_PORT, &GPIO_InitStructure);
	
	// Configure CS pin
	GPIO_InitStructure.GPIO_Pin = _SD_CS_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(_SD_CS_PORT, &GPIO_InitStructure);
	
	// SPI configuration
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_256; // Slow init speed
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI_InitStructure.SPI_CRCPolynomial = 7;
	SPI_Init(_SD_SPI, &SPI_InitStructure);
	
	SPI_Cmd(_SD_SPI, ENABLE);
	_set_cs_high();
}

static uint8_t spi_transfer(uint8_t data) {
	while (SPI_I2S_GetFlagStatus(_SD_SPI, SPI_I2S_FLAG_TXE) == RESET);
	SPI_I2S_SendData(_SD_SPI, data);
	while (SPI_I2S_GetFlagStatus(_SD_SPI, SPI_I2S_FLAG_RXNE) == RESET);
	return SPI_I2S_ReceiveData(_SD_SPI);
}

static uint8_t _spi_read_byte() {
	while (SPI_I2S_GetFlagStatus(_SD_SPI, SPI_I2S_FLAG_TXE) == RESET);
	SPI_I2S_SendData(_SD_SPI, 0xFF);
	while (SPI_I2S_GetFlagStatus(_SD_SPI, SPI_I2S_FLAG_RXNE) == RESET);
	return SPI_I2S_ReceiveData(_SD_SPI);
}



static uint8_t send_cmd(uint8_t cmd, uint32_t arg) {
	uint8_t n, res;

	if (cmd & 0x80) {	/* ACMD<n> is the command sequense of CMD55-CMD<n> */
		cmd &= 0x7F;
		res = send_cmd(CMD55, 0);
		if (res > 1) return res;
	}

	/* Select the card */
	_set_cs_high();
	// Delay_Us(100);
	_spi_read_byte();
	_set_cs_low();
	// Delay_Us(100);
	_spi_read_byte();

	/* Send a command packet */
	spi_transfer(cmd);						/* Start + Command index */
	spi_transfer((uint8_t)(arg >> 24));		/* Argument[31..24] */
	spi_transfer((uint8_t)(arg >> 16));		/* Argument[23..16] */
	spi_transfer((uint8_t)(arg >> 8));			/* Argument[15..8] */
	spi_transfer((uint8_t)arg);				/* Argument[7..0] */
	n = 0x01;							/* Dummy CRC + Stop */
	if (cmd == CMD0) n = 0x95;			/* Valid CRC for CMD0(0) */
	if (cmd == CMD8) n = 0x87;			/* Valid CRC for CMD8(0x1AA) */
	spi_transfer(n);

	/* Receive a command response */
	n = 10;								/* Wait for a valid response in timeout of 10 attempts */
	do {
		res = _spi_read_byte();
	} while ((res & 0x80) && --n);

	return res;			/* Return with the response value */
}

static UINT _get_proper_sector(UINT sector)
{
	return (!(_card_type & CT_BLOCK)) ? sector *= BLOCK_SIZE : sector;	/* Convert to byte address if needed */
}

static void _finish_transaction()
{
	_set_cs_high();
	_spi_read_byte();
}


/*-----------------------------------------------------------------------*/
/* Initialize Disk Drive                                                 */
/*-----------------------------------------------------------------------*/

m_disk_io_res_e m_disk_io_init()
{
	uint8_t n, cmd, ty, ocr[4];
	UINT tmr;

	_spi_init();		/* Initialize ports to control MMC */

	_set_cs_high();

	for (n = 10; n; n--){
		_spi_read_byte();	/* 80 dummy clocks with CS=H */
	}

	ty = 0;
	if (send_cmd(CMD0, 0) == 1) {			/* GO_IDLE_STATE */
		if (send_cmd(CMD8, 0x1AA) == 1) {	/* SDv2 */
			for (n = 0; n < 4; n++) ocr[n] = _spi_read_byte();		/* Get trailing return value of R7 resp */
			if (ocr[2] == 0x01 && ocr[3] == 0xAA) {			/* The card can work at vdd range of 2.7-3.6V */
				for (tmr = 10000; tmr && send_cmd(ACMD41, 1UL << 30); tmr--) Delay_Us(100);	/* Wait for leaving idle state (ACMD41 with HCS bit) */
				if (tmr && send_cmd(CMD58, 0) == 0) {		/* Check CCS bit in the OCR */
					for (n = 0; n < 4; n++) ocr[n] = _spi_read_byte();
					ty = (ocr[0] & 0x40) ? CT_SD2 | CT_BLOCK : CT_SD2;	/* SDv2 (HC or SC) */
				}
			}
		} else {							/* SDv1 or MMCv3 */
			if (send_cmd(ACMD41, 0) <= 1) 	{
				ty = CT_SD1; cmd = ACMD41;	/* SDv1 */
			} else {
				ty = CT_MMC; cmd = CMD1;	/* MMCv3 */
			}
			for (tmr = 10000; tmr && send_cmd(cmd, 0); tmr--) Delay_Us(100);	/* Wait for leaving idle state */
			if (!tmr || send_cmd(CMD16, BLOCK_SIZE) != 0) {	/* Set R/W block length to BLOCK_SIZE */
				ty = 0;
			}
		}
	}
	_card_type = ty;
	printf("Card type = %d\r\n", _card_type);
	_finish_transaction();

	return ty ? M_DISK_IO_RES_OK : M_DISK_IO_RES_ERROR;
}



/*-----------------------------------------------------------------------*/
/* Read Partial Sector                                                   */
/*-----------------------------------------------------------------------*/
m_disk_io_res_e m_disk_io_read(uint8_t* buff, UINT sector, UINT offset, UINT count)
{
	//read from buffer of current sector
	if(_buffer_sector == sector){
		for (UINT i = offset; i < offset + count; i++)
		{
			*buff++ = _buffer[i];
		}
		return M_DISK_IO_RES_OK;
	}

	if (send_cmd(CMD17, _get_proper_sector(sector)) == 0) {	/* READ_SINGLE_BLOCK */

		UINT tries = 40000;	/* Time counter */
		uint8_t header = 0;
		do {				/* Wait for data block */
			header = _spi_read_byte();
		} while (header == 0xFF && --tries);

		if (header == 0xFE) {	/* A data block arrived */
			for(int i = 0; i < BLOCK_SIZE; i++){
				_buffer[i] = _spi_read_byte();
			}

			uint16_t crc = _spi_read_byte() << 8;
			crc |= _spi_read_byte();

			_buffer_sector = sector;

			_finish_transaction();

			return m_disk_io_read(buff, sector, offset, count);
		}
	}

	_finish_transaction();

	return M_DISK_IO_RES_ERROR;
}



/*-----------------------------------------------------------------------*/
/* Write Partial Sector                                                  */
/*-----------------------------------------------------------------------*/

m_disk_io_res_e m_disk_io_write(const uint8_t* buff, UINT sector, UINT offset, UINT count)
{
	if(_buffer_sector != sector){
		//read sector
		if(m_disk_io_read(NULL, sector, 0, 0) != M_DISK_IO_RES_OK){
			return M_DISK_IO_RES_ERROR;
		}
	}

	//now we have necessary buffer
	for (UINT i = offset, j = 0; i < offset + count; i++, j++)
	{
		_buffer[i] = buff[j];
	}

	//ready to flash
	if (send_cmd(CMD24, _get_proper_sector(sector)) == 0) {			/* WRITE_SINGLE_BLOCK */

		Delay_Us(100);
		spi_transfer(0xFE);		/* Data block header */
		for (UINT i = 0; i < M_FS_CONF_BLOCK_SIZE; i++)
		{
			spi_transfer(_buffer[i]);
		}
		
		//CRC
		spi_transfer(0);
		spi_transfer(0);

		//read the response
		uint8_t response = _spi_read_byte();
		// printf("Response = %d\r\n", response);
		if(!(response & 0b101)){
			//failed to write

			_finish_transaction();

			return M_DISK_IO_RES_ERROR;
		}

		//now wait until DI is HIGH
		Delay_Us(100);
		while(_spi_read_byte() == 0x00){
			Delay_Us(100);
		}

		_finish_transaction();

		return M_DISK_IO_RES_OK;
	}

	_finish_transaction();
	
	return M_DISK_IO_RES_ERROR;
}