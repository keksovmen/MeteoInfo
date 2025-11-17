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

#include "m_i2c_hal.hpp"
#include "m_hal_ssd1315.hpp"
#include "m_buffered_display.hpp"
#include "m_font_writer.hpp"
#include "m_graph_drawer.hpp"
#include "m_sys_time.hpp"
#include "m_sleep.hpp"



#define _TEST_STORAGE_SIZE 10



static periph::I2C_Hal i2c;

static display::HalDisplaySSD1315 ssd1315(i2c);
static display::PartitionBufferedWriter<128, decltype(ssd1315)> writer(ssd1315);
static display::FontWriter<decltype(writer)> font(writer);
static display::GraphDrawer<decltype(writer)> graph(writer, font);
static periph::Aht20 aht(i2c);
static std::array<std::pair<int32_t, int32_t>, _TEST_STORAGE_SIZE> _dataEntries;
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
	const auto data = aht.readTempAndHum();

	// if (aht20_read_temperature_humidity(&i2c, &sensor_data)) {
	printf("Temperature: %d.%d °C\r\n", (int) data.first, (int) ((data.first - (int) data.first) * 100));
	printf("Humidity: %d.%d %%\r\n", (int) data.second, (int) ((data.second - (int) data.second) * 100));
	// } else {
	// 	printf("Failed to read sensor data\r\n");
	// }
	
	return 0;
}



int main (void) {
	// NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
	SystemCoreClockUpdate();
	Delay_Init();
	periph::sleep::init();
	periph::sys_time::init(100);
#if (SDI_PRINT == SDI_PR_OPEN)
	SDI_Printf_Enable();
#else
	USART_Printf_Init (115200);
#endif
	printf ("SystemClk:%lu\r\n", SystemCoreClock);
	printf ("ChipID:%08x\r\n", DBGMCU_GetCHIPID());

	// USARTx_CFG();

	i2c.init(100000);

	_test_aht20();

	
	ssd1315.init();
	ssd1315.clearScreen();

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
		ssd1315.turnOn();
		// printf ("Cycle\r\n");
		// std::copy(_dataEntries.begin(), _dataEntries.end() - 1, &_dataEntries[1]);
		std::copy_backward(_dataEntries.begin(), _dataEntries.end() - 1, _dataEntries.end());
		// for(int i = _dataEntries.size() - 1; i > 0; i--){
		// 	_dataEntries[i] = _dataEntries[i - 1];
		// }

		_dataEntries[0] = aht.readTempAndHum();
		auto& data = _dataEntries[0];
		
		printf("Temperature: %d.%d °C\r\n", data.first / 100, abs(data.second % 100));
		printf("Humidity: %d.%d %%\r\n", data.second / 100, abs(data.second % 100));

		std::array<int32_t, _TEST_STORAGE_SIZE> tmp;
		std::transform(_dataEntries.begin(), _dataEntries.end(), tmp.begin(), [](const auto& e){ return (_count < 5) ? e.first : e.second; });

		// std::for_each(_dataEntries.begin(), _dataEntries.end(), [](auto& e){printf("%d.%d\r\n", (int) e.first, (int) ((e.first - (int) e.first) * 100));});
		// std::for_each(tmp.begin(), tmp.end(), [](auto& e){printf("%d.%d\r\n", (int) e, (int) ((e - (int) e) * 100));});
		graph.setLabel((_count < 5) ? "TEMPERATURE C" : "HUMIDITY %");
		writer.addDrawAction([](){
			char buff[16] = {0};
			sprintf(buff, "%lu", periph::sys_time::currentMs());
			font.drawStr(0, 56, buff);
		});
		graph.drawGraph({tmp.begin(), _TEST_STORAGE_SIZE});

		writer.clearDrawActions();

		_count = (_count + 1) % 10;

		Delay_Ms(1000);

		ssd1315.turnOff();
		periph::sleep::sleepForMs<1000>();
	}
}