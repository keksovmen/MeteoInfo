#include "ch32v00x.h"

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <utility>

#include "debug.h"

#include "m_button.hpp"
#include "m_fsm.hpp"
#include "m_sleep.hpp"
#include "m_sys_time.hpp"
#include "m_timer.hpp"



static timers::TimerCounter _sleepModeTimer(10000);
//how often to measure data
static timers::TimerCounter _sensorTimer(1000);

static states::DeepSleepState _deepSleepState;
static states::CurrentValueState _currentValueState;
static states::GraphValueState _valueState;
static states::LedState _ledState;
static etl::ifsm_state* _stateList[states::StateId::NUMBER_OF_STATES] = {
	&_deepSleepState,
	&_currentValueState,
	&_valueState,
	&_ledState
};
static states::GuiMachine _machine;
static bool _wakeupFlag = false;



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
	if(_wakeupFlag){
		_wakeupFlag = false;
		_machine.receive(states::WakeupEvent());
		_sleepModeTimer.reset();
	}

	const bool keyEvent = periph::buttons::pull();
	if(keyEvent){
		_sleepModeTimer.reset();
	}

	const bool dataEvent = _sensorTimer.update();
	if(dataEvent){
		_sensorTimer.reset();
		globals::getSensor().readTempAndHum();

		//push data to some sort of store
		// printf("Battery: %u, %u\r\n", globals::getBattery().readRaw(), globals::getBattery().readPercents());
		// printf("Time: %u / %ums\r\n", periph::sys_time::currentTick(), periph::sys_time::currentMs());
	}

	const bool sleepEvent = _sleepModeTimer.update();

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
	periph::sys_time::init(globals::LED_LOOP_FREQUENCY);
	periph::buttons::init(1500, [](){_wakeupFlag = true;});

	USART_Printf_Init (115200);
	
	printf ("SystemClk:%u\r\n", SystemCoreClock);
	printf ("ChipID:%08x\r\n", DBGMCU_GetCHIPID());

	globals::getI2c().init(100000);

	if (!globals::getSensor().init()) {
		printf("AHT20 initialization failed!\r\n");
		return -1;
	}
	
	globals::getDisplayHal().init();
	globals::getDisplayHal().clearScreen();
	//TODO: need calibration
	globals::getBattery().init();

	_machine.set_states(_stateList, ETL_ARRAY_SIZE(_stateList));
	_machine.start();

	while (1) {
		_loop();
	}
}