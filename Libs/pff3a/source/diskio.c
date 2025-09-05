/*-----------------------------------------------------------------------*/
/* Low level disk I/O module skeleton for Petit FatFs (C)ChaN, 2014      */
/*-----------------------------------------------------------------------*/

#include "diskio.h"

#include "ch32v00x.h"
#include "ch32v00x_gpio.h"
#include "ch32v00x_spi.h"
#include "ch32v00x_rcc.h"
#include "debug.h"



// SD card SPI configuration (adjust pins according to your wiring)
#define SD_SPI             SPI1
#define SD_CS_PORT         GPIOC
#define SD_CS_PIN          GPIO_Pin_0
#define SD_SPI_CLK_PORT    GPIOC
#define SD_SPI_CLK_PIN     GPIO_Pin_5
#define SD_SPI_MISO_PORT   GPIOC
#define SD_SPI_MISO_PIN    GPIO_Pin_7
#define SD_SPI_MOSI_PORT   GPIOC
#define SD_SPI_MOSI_PIN    GPIO_Pin_6

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



void sd_cs_low(void);
void sd_cs_high(void);
uint8_t send_cmd(uint8_t cmd, uint32_t arg);
uint8_t sd_init(void);



static uint8_t CardType = 0;



// SPI functions
static void spi_init(void) {
    SPI_InitTypeDef SPI_InitStructure = {0};
    GPIO_InitTypeDef GPIO_InitStructure = {0};
    
    // Enable clocks
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC | RCC_APB2Periph_SPI1, ENABLE);
    
    // Configure SPI pins
    GPIO_InitStructure.GPIO_Pin = SD_SPI_CLK_PIN | SD_SPI_MOSI_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
    GPIO_Init(SD_SPI_CLK_PORT, &GPIO_InitStructure);
    
    GPIO_InitStructure.GPIO_Pin = SD_SPI_MISO_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(SD_SPI_MISO_PORT, &GPIO_InitStructure);
    
    // Configure CS pin
    GPIO_InitStructure.GPIO_Pin = SD_CS_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(SD_CS_PORT, &GPIO_InitStructure);
    
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
    SPI_Init(SD_SPI, &SPI_InitStructure);
    
    SPI_Cmd(SD_SPI, ENABLE);
    sd_cs_high();
}

static uint8_t spi_transfer(uint8_t data) {
    while (SPI_I2S_GetFlagStatus(SD_SPI, SPI_I2S_FLAG_TXE) == RESET);
    SPI_I2S_SendData(SD_SPI, data);
    while (SPI_I2S_GetFlagStatus(SD_SPI, SPI_I2S_FLAG_RXNE) == RESET);
    return SPI_I2S_ReceiveData(SD_SPI);
}

static uint8_t spi_transfer_0xff() {
    while (SPI_I2S_GetFlagStatus(SD_SPI, SPI_I2S_FLAG_TXE) == RESET);
    SPI_I2S_SendData(SD_SPI, 0xFF);
    while (SPI_I2S_GetFlagStatus(SD_SPI, SPI_I2S_FLAG_RXNE) == RESET);
    return SPI_I2S_ReceiveData(SD_SPI);
}

void sd_cs_low(void) {
    GPIO_WriteBit(SD_CS_PORT, SD_CS_PIN, Bit_RESET);
}

void sd_cs_high(void) {
    GPIO_WriteBit(SD_CS_PORT, SD_CS_PIN, Bit_SET);
}



uint8_t send_cmd(uint8_t cmd, uint32_t arg) {
	BYTE n, res;

	if (cmd & 0x80) {	/* ACMD<n> is the command sequense of CMD55-CMD<n> */
		cmd &= 0x7F;
		res = send_cmd(CMD55, 0);
		if (res > 1) return res;
	}

	/* Select the card */
	sd_cs_high();
	spi_transfer_0xff();
	sd_cs_low();
	spi_transfer_0xff();

	/* Send a command packet */
	spi_transfer(cmd);						/* Start + Command index */
	spi_transfer((BYTE)(arg >> 24));		/* Argument[31..24] */
	spi_transfer((BYTE)(arg >> 16));		/* Argument[23..16] */
	spi_transfer((BYTE)(arg >> 8));			/* Argument[15..8] */
	spi_transfer((BYTE)arg);				/* Argument[7..0] */
	n = 0x01;							/* Dummy CRC + Stop */
	if (cmd == CMD0) n = 0x95;			/* Valid CRC for CMD0(0) */
	if (cmd == CMD8) n = 0x87;			/* Valid CRC for CMD8(0x1AA) */
	spi_transfer(n);

	/* Receive a command response */
	n = 10;								/* Wait for a valid response in timeout of 10 attempts */
	do {
		res = spi_transfer_0xff();
	} while ((res & 0x80) && --n);

	return res;			/* Return with the response value */
}



/*-----------------------------------------------------------------------*/
/* Initialize Disk Drive                                                 */
/*-----------------------------------------------------------------------*/

DSTATUS disk_initialize (void)
{
	BYTE n, cmd, ty, ocr[4];
	UINT tmr;

#if PF_USE_WRITE
	if (CardType != 0) disk_writep(0, 0);	/* Finalize write process if it is in progress */
#endif
	spi_init();		/* Initialize ports to control MMC */
	sd_cs_high();
	for (n = 10; n; n--) spi_transfer_0xff();	/* 80 dummy clocks with CS=H */

	ty = 0;
	if (send_cmd(CMD0, 0) == 1) {			/* GO_IDLE_STATE */
		if (send_cmd(CMD8, 0x1AA) == 1) {	/* SDv2 */
			for (n = 0; n < 4; n++) ocr[n] = spi_transfer_0xff();		/* Get trailing return value of R7 resp */
			if (ocr[2] == 0x01 && ocr[3] == 0xAA) {			/* The card can work at vdd range of 2.7-3.6V */
				for (tmr = 10000; tmr && send_cmd(ACMD41, 1UL << 30); tmr--) Delay_Us(100);	/* Wait for leaving idle state (ACMD41 with HCS bit) */
				if (tmr && send_cmd(CMD58, 0) == 0) {		/* Check CCS bit in the OCR */
					for (n = 0; n < 4; n++) ocr[n] = spi_transfer_0xff();
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
			if (!tmr || send_cmd(CMD16, 512) != 0) {	/* Set R/W block length to 512 */
				ty = 0;
			}
		}
	}
	CardType = ty;
	printf("Card type = %d\r\n", CardType);
	sd_cs_high();
	spi_transfer_0xff();

	return ty ? 0 : STA_NOINIT;
}



/*-----------------------------------------------------------------------*/
/* Read Partial Sector                                                   */
/*-----------------------------------------------------------------------*/

DRESULT disk_readp (
	BYTE* buff,		/* Pointer to the destination object */
	DWORD sector,	/* Sector number (LBA) */
	UINT offset,	/* Offset in the sector */
	UINT count		/* Byte count (bit15:destination) */
)
{
	DRESULT res;
	BYTE rc;
	UINT bc;


	if (!(CardType & CT_BLOCK)) sector *= 512;	/* Convert to byte address if needed */

	res = RES_ERROR;
	if (send_cmd(CMD17, sector) == 0) {	/* READ_SINGLE_BLOCK */

		bc = 40000;	/* Time counter */
		do {				/* Wait for data block */
			rc = spi_transfer_0xff();
		} while (rc == 0xFF && --bc);

		if (rc == 0xFE) {	/* A data block arrived */

			bc = 512 + 2 - offset - count;	/* Number of trailing bytes to skip */

			/* Skip leading bytes in the sector */
			while (offset--) spi_transfer_0xff();

			/* Receive a part of the sector */
			if (buff) {	/* Store data to the memory */
				do {
					*buff++ = spi_transfer_0xff();
				} while (--count);
			} else {	/* Forward data to the outgoing stream */
				do {
					printf("%c", spi_transfer_0xff());
				} while (--count);
			}

			/* Skip trailing bytes in the sector and block CRC */
			do spi_transfer_0xff(); while (--bc);

			res = RES_OK;
		}
	}

	sd_cs_high();
	spi_transfer_0xff();

	return res;
}



/*-----------------------------------------------------------------------*/
/* Write Partial Sector                                                  */
/*-----------------------------------------------------------------------*/

DRESULT disk_writep (
	const BYTE* buff,		/* Pointer to the data to be written, NULL:Initiate/Finalize write operation */
	DWORD sc		/* Sector number (LBA) or Number of bytes to send */
)
{
	DRESULT res;
	UINT bc;
	static UINT wc;	/* Sector write counter */

	res = RES_ERROR;

	if (buff) {		/* Send data bytes */
		bc = sc;
		while (bc && wc) {		/* Send data bytes to the card */
			spi_transfer(*buff++);
			wc--; bc--;
		}
		res = RES_OK;
	} else {
		if (sc) {	/* Initiate sector write process */
			if (!(CardType & CT_BLOCK)) sc *= 512;	/* Convert to byte address if needed */
			if (send_cmd(CMD24, sc) == 0) {			/* WRITE_SINGLE_BLOCK */
				spi_transfer(0xFF); spi_transfer(0xFE);		/* Data block header */
				wc = 512;							/* Set byte counter */
				res = RES_OK;
			}
		} else {	/* Finalize sector write process */
			bc = wc + 2;
			while (bc--) spi_transfer(0);	/* Fill left bytes and CRC with zeros */
			if ((spi_transfer_0xff() & 0x1F) == 0x05) {	/* Receive data resp and wait for end of write process in timeout of 500ms */
				for (bc = 5000; spi_transfer_0xff() != 0xFF && bc; bc--) {	/* Wait for ready */
					Delay_Us(100);
				}
				if (bc) res = RES_OK;
			}
			sd_cs_high();
			spi_transfer_0xff();
		}
	}

	return res;

	// DRESULT res;


	// if (!buff) {
	// 	if (sc) {

	// 		// Initiate write process

	// 	} else {

	// 		// Finalize write process

	// 	}
	// } else {

	// 	// Send data to the disk

	// }

	// return res;
}

