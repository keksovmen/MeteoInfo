#include "ch32v00x.h"

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// #include "pff.h"
#include "m_disk_io.h"
#include "m_fs.h"

#include "m_aht20.hpp"

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
	&periph::I2C_ReadByte,
	&periph::I2C_StopSequence
};

static display::HalDisplaySSD1315 ssd1315(i2c);
static display::PartitionBufferedWriter<128> writer(ssd1315);
static display::FontWriter font(writer);

// static FATFS _fs;



int check_aht20(void)
{
	Delay_Ms(100);

	periph::Aht20 aht(i2c);

	// Initialize sensor
	if (!aht.init()) {
		printf("AHT20 initialization failed!\r\n");
		return -1;
	}
	
	printf("AHT20 initialized successfully\r\n");
	
	// Read sensor data
	std::pair<float, float> data = aht.readTempAndHum();

	// if (aht20_read_temperature_humidity(&i2c, &sensor_data)) {
	printf("Temperature: %d.%d °C\r\n", (int) data.first, (int) ((data.first - (int) data.first) * 100));
	printf("Humidity: %d.%d %%\r\n", (int) data.second, (int) ((data.second - (int) data.second) * 100));
	// } else {
	// 	printf("Failed to read sensor data\r\n");
	// }
	
	return 0;
}



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

	check_aht20();

	
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

	char buff[8] = {0, 0, 0, 0};

	m_fs_t fs;
	printf("Init result = %d\r\n", m_fs_init(&fs));

	m_fs_file_t root;
	printf("Open result = %d, size = %d\r\n", m_fs_open(NULL, M_FS_OPEN_MODE_WRITE, &root), root.sizeB);
	printf("Write result = %d, size = %d\r\n", m_fs_write(&root, (const uint8_t*) "AHAHAHA", 8), root.sizeB);
	printf("Open result = %d, size = %d\r\n", m_fs_open(NULL, M_FS_OPEN_MODE_WRITE, &root), root.sizeB);
	printf("Read result = %d\t%s\r\n", m_fs_read(&root, (uint8_t*) buff, 8), buff);


	// printf("Init result = %d\r\n", m_disk_io_init());
	// printf("Write result = %d\r\n", m_disk_io_write((const uint8_t*) "123", 15, 200, 3));
	// printf("Write result = %d\r\n", m_disk_io_write((const uint8_t*) "456", 16, 155, 3));
	// printf("Read result = %d, %s\r\n", m_disk_io_read((uint8_t*) buff, 15, 200, 3), buff);
	// printf("Read result = %d, %s\r\n", m_disk_io_read((uint8_t*) buff, 16, 155, 3), buff);
	/*
	FRESULT res = pf_mount(&_fs);
	printf("Mount result = %d\r\n", res);

	res = pf_open("ASDTXT~1.TXT");
	printf("Open result = %d\r\n", res);

	UINT _;
	res = pf_write("test_text\r\n", sizeof("test_text\r\n"), &_);
	printf("Write result = %d, written = %d\r\n", res, _);

	res = pf_mount(&_fs);
	printf("Mount result = %d\r\n", res);

	DIR dir;
	res = pf_opendir(&dir, "");
	printf("Open dir result = %d\r\n", res);

	while(true){
		FILINFO info;
		res = pf_readdir(&dir, &info);
		printf("Read dir result = %d\r\n", res);

		printf("Dir = %s\r\n", info.fname);
		if(strlen(info.fname) == 0){
			break;
		}
	}
	*/


    while (1) {
        // printf ("Cycle\r\n");
    }
}