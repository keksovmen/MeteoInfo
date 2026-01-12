#pragma once


#include "m_hal_ssd1315.hpp"
#include "m_graph_drawer.hpp"
#include "m_aht20.hpp"
#include "m_battery.hpp"
#include "m_pwm.hpp"



namespace globals
{
	periph::I2C_Hal& getI2c();

	display::HalDisplaySSD1315& getDisplayHal();
	display::PartitionBufferedWriter<128, display::HalDisplaySSD1315>& getDisplayWriter();
	display::FontWriter<display::PartitionBufferedWriter<128, display::HalDisplaySSD1315>>& getFontWriter();
	display::GraphDrawer<display::PartitionBufferedWriter<128, display::HalDisplaySSD1315>>& getGraphWriter();

	periph::Aht20& getSensor();
	periph::Battery& getBattery();
	periph::Pwm& getPwm();
}