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

	globals::getI2c().init(100000);

	_test_aht20();
	
	globals::getDisplayHal().init();
	globals::getDisplayHal().clearScreen();
	globals::getBattery().init();

	globals::getPwm().enable();
	globals::getPwm().enablePin(periph::Pwm::Pin::PIN_0);
	globals::getPwm().writePin(periph::Pwm::Pin::PIN_0, 500);


	_machine.set_states(_stateList, ETL_ARRAY_SIZE(_stateList));
	_machine.start();

	while (1) {
		// printf ("Cycle\r\n");
		_loop();
	}
}