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
#include "m_i2c_impl.hpp"
#include "m_hal_ssd1315.hpp"
#include "m_buffered_display.hpp"
#include "m_font_writer.hpp"
#include "m_graph_drawer.hpp"



#define _TEST_STORAGE_SIZE 10



static periph::I2C_Peripheral i2c{
	&periph::i2c_init,
	&periph::i2c_start_sequence,
	&periph::i2c_read_request,
	&periph::i2c_write_request,
	&periph::i2c_write_byte,
	&periph::i2c_read_byte,
	&periph::i2c_stop_sequence,
	&Delay_Ms
};

static display::HalDisplaySSD1315 ssd1315(i2c);
static display::PartitionBufferedWriter<128> writer(ssd1315);
static display::FontWriter font(writer);
static display::GraphDrawer graph(writer, font);
static periph::Aht20 aht(i2c);
static std::array<std::pair<float, float>, _TEST_STORAGE_SIZE> _dataEntries;
static int _count = 0;

// static FATFS _fs;



static int _test_aht20(void)
{
	Delay_Ms(100);

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

	_test_aht20();

	
	ssd1315.init();
	ssd1315.clearScreen();

	// writer.addDrawAction([](){
		// w.drawRectangle(0, 16, 16, 8);
		// w.setPixel(127, 0, true);
		// w.setPixel(126, 1, true);
		// w.setPixel(125, 2, true);
		// w.setPixel(124, 3, true);
		// w.setPixel(123, 4, true);
		// w.setPixel(122, 5, true);
		// w.setPixel(121, 6, true);
		// w.setPixel(120, 7, true);

		// w.setPixel(127, 7, true);
		// w.setPixel(126, 6, true);
		// w.setPixel(125, 5, true);
		// w.setPixel(124, 4, true);
		// w.setPixel(123, 3, true);
		// w.setPixel(122, 2, true);
		// w.setPixel(121, 1, true);
		// w.setPixel(120, 0, true);

		// w.drawLine(120, 8, 127, 15);
		// w.drawLine(120, 15, 127, 8);

		// font.changeSize<display::FontWriter::FontSize::MEDIUM>();
		// font.drawStr(0, 0, "%+-_*!@#");

		// font.changeSize<display::FontWriter::FontSize::SMALL>();
		// font.drawStr(0, 32, "+ 0123456789 -");

		// font.changeSize<display::FontWriter::FontSize::MEDIUM>();
		// font.drawStr(64, 48, "ABCDEZ");

		
	// 	auto data = aht.readTempAndHum();

	// 	char buff[32] = {0};
	// 	sprintf(buff, "TEMP: %d.%d C", (int) data.first, (int) ((data.first - (int) data.first) * 100));

	// 	font.changeSize<display::FontWriter::FontSize::MEDIUM>();
	// 	font.drawStr(0, 0, buff);

	// 	sprintf(buff, "HUM: %d.%d %%", (int) data.second, (int) ((data.second - (int) data.second) * 100));
	// 	font.drawStr(0, 16, buff);
	// });


	// writer.flush();

	// float dataGraph[] = {5.0f, 3.0f, 1.0f, 4.0f, 6.0f, -1.0f};
	// graph.drawGraph(dataGraph);

	// char buff[8] = {0, 0, 0, 0};

	// m_fs_t fs;
	// printf("Init result = %d\r\n", m_fs_init(&fs));

	// m_fs_file_t root;
	// printf("Open result = %d, size = %d\r\n", m_fs_open(NULL, M_FS_OPEN_MODE_WRITE, &root), root.sizeB);
	// printf("Write result = %d, size = %d\r\n", m_fs_write(&root, (const uint8_t*) "AHAHAHA", 8), root.sizeB);
	// printf("Open result = %d, size = %d\r\n", m_fs_open(NULL, M_FS_OPEN_MODE_WRITE, &root), root.sizeB);
	// printf("Read result = %d\t%s\r\n", m_fs_read(&root, (uint8_t*) buff, 8), buff);


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
		// std::copy(_dataEntries.begin(), _dataEntries.end() - 1, &_dataEntries[1]);
		std::copy_backward(_dataEntries.begin(), _dataEntries.end() - 1, _dataEntries.end());
		// for(int i = _dataEntries.size() - 1; i > 0; i--){
		// 	_dataEntries[i] = _dataEntries[i - 1];
		// }

		_dataEntries[0] = aht.readTempAndHum();
		auto& data = _dataEntries[0];
		
		printf("Temperature: %d.%d °C\r\n", (int) data.first, (int) ((data.first - (int) data.first) * 100));
		printf("Humidity: %d.%d %%\r\n", (int) data.second, (int) ((data.second - (int) data.second) * 100));

		std::array<float, _TEST_STORAGE_SIZE> tmp;
		std::transform(_dataEntries.begin(), _dataEntries.end(), tmp.begin(), [](const auto& e){ return (_count < 5) ? e.first : e.second; });

		// std::for_each(_dataEntries.begin(), _dataEntries.end(), [](auto& e){printf("%d.%d\r\n", (int) e.first, (int) ((e.first - (int) e.first) * 100));});
		// std::for_each(tmp.begin(), tmp.end(), [](auto& e){printf("%d.%d\r\n", (int) e, (int) ((e - (int) e) * 100));});

		graph.drawGraph({tmp.begin(), _TEST_STORAGE_SIZE});

		_count = (_count + 1) % 10;

		Delay_Ms(1000);
    }
}