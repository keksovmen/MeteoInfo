#pragma once


#include "m_aht20.hpp"
#include "m_battery.hpp"
#include "m_graph_drawer.hpp"
#include "m_hal_ssd1315.hpp"
#include "m_led_driver.hpp"
#include "m_pwm.hpp"



namespace globals
{
	using RgbDriver = periph::LedDriver<periph::Pwm::Pin::PIN_0, periph::Pwm::Pin::PIN_2, periph::Pwm::Pin::PIN_3>;
	using DisplayWriter = display::PartitionBufferedWriter<128, display::HalDisplaySSD1315>;



	constexpr uint32_t PWM_RESOLUTION = 255;
	constexpr uint32_t LED_CYCLE_MS = 100;
	constexpr uint32_t LED_LOOP_FREQUENCY = (PWM_RESOLUTION * 1000u) / LED_CYCLE_MS;



	periph::I2C_Hal& getI2c();

	display::HalDisplaySSD1315& getDisplayHal();
	DisplayWriter& getDisplayWriter();
	display::FontWriter<DisplayWriter>& getFontWriter();
	display::GraphDrawer<DisplayWriter>& getGraphWriter();

	periph::Aht20& getSensor();
	periph::Battery& getBattery();
	periph::Pwm& getPwm();
	RgbDriver& getLedDriver();
}