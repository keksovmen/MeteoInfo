#include "ch32v00x.h"

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <utility>

// #include "pff.h"
// #include "m_disk_io.h"
// #include "m_fs.h"

#include "debug.h"

#include "m_sys_time.hpp"
#include "m_sleep.hpp"
#include "m_button.hpp"
#include "m_timer.hpp"
#include "m_fsm.hpp"



static timers::TimerCounter _sleepModeTimer(10000);
//how often to measure data
static timers::TimerCounter _sensorTimer(1000);

static states::DeepSleepState _deepSleepState;
static states::CurrentValueState _currentValueState;
static states::GraphValueState _valueState;
static etl::ifsm_state* _stateList[states::StateId::NUMBER_OF_STATES] = {
	&_deepSleepState,
	&_currentValueState,
	&_valueState
};
static states::GuiMachine _machine;




static int _test_aht20(void)
{
	Delay_Ms(100);

	// Initialize sensor
	if (!globals::getSensor().init()) {
		printf("AHT20 initialization failed!\r\n");
		return -1;
	}
	
	printf("AHT20 initialized successfully\r\n");
	
	// Read sensor data
	const auto data = globals::getSensor().readTempAndHum();

	// if (aht20_read_temperature_humidity(&_i2c, &sensor_data)) {
	printf("Temperature: %d.%d °C\r\n", data.first / 100, abs(data.first % 100));
	printf("Humidity: %d.%d %%\r\n", data.second / 100, abs(data.second % 100));
	// } else {
	// 	printf("Failed to read sensor data\r\n");
	// }
	
	return 0;
}



static void _loop()
{
	const bool keyEvent = periph::buttons::hasEvent();
	if(keyEvent){
		_sleepModeTimer.reset();
		_sleepModeTimer.start();
	}

	const bool dataEvent = !_sensorTimer.update();
	if(dataEvent){
		_sensorTimer.reset();
		globals::getSensor().readTempAndHum();

		//push data to some sort of store
		printf("Battery: %u, %u\r\n", globals::getBattery().readRaw(), globals::getBattery().readPercents());
	}

	const bool sleepEvent = !_sleepModeTimer.update();

	if(dataEvent){
		_machine.receive(states::DataEvent());
	}

	if(keyEvent){
		_machine.receive(states::ButtonClickEvent(periph::buttons::getLastEvent()));
	}

	if(sleepEvent){
		_machine.receive(states::TimeoutEvent());
	}

	_machine.receive(states::TickEvent());
}



int main (void) {
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
	SystemCoreClockUpdate();
	Delay_Init();
	periph::sleep::init();
	periph::sys_time::init(100);
	periph::buttons::init(1500, {});
#if (SDI_PRINT == SDI_PR_OPEN)
	SDI_Printf_Enable();
#else
	USART_Printf_Init (115200);
#endif
	printf ("SystemClk:%u\r\n", SystemCoreClock);
	printf ("ChipID:%08x\r\n", DBGMCU_GetCHIPID());

	// USARTx_CFG();

	globals::getI2c().init(100000);

	_test_aht20();
	
	globals::getDisplayHal().init();
	globals::getDisplayHal().clearScreen();
	globals::getBattery().init();

	_machine.set_states(_stateList, ETL_ARRAY_SIZE(_stateList));
	_machine.start();

	while (1) {
		// printf ("Cycle\r\n");
		_loop();
	}

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

}