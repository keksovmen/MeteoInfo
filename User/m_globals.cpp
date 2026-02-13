#include "m_globals.hpp"



#define _BATTERY_ADC_CHANNEL 7



static periph::I2C_Hal _i2c;

static display::HalDisplaySSD1315 _ssd1315(_i2c);
static globals::DisplayWriter _displayWriter(_ssd1315);
static display::FontWriter<decltype(_displayWriter)> _font(_displayWriter);
static display::GraphDrawer<decltype(_displayWriter)> _graph(_displayWriter, _font);

static periph::Aht20 _aht(_i2c);
static periph::Battery _battery(_BATTERY_ADC_CHANNEL);
static periph::Pwm _pwm(5000);
static globals::RgbDriver _ledDriver(_pwm);



periph::I2C_Hal& globals::getI2c()
{
	return _i2c;
}

display::HalDisplaySSD1315& globals::getDisplayHal()
{
	return _ssd1315;
}

globals::DisplayWriter& globals::getDisplayWriter()
{
	return _displayWriter;
}

display::FontWriter<globals::DisplayWriter>& globals::getFontWriter()
{
	return _font;
}

display::GraphDrawer<globals::DisplayWriter>& globals::getGraphWriter()
{
	return _graph;
}

periph::Aht20& globals::getSensor()
{
	return _aht;
}

periph::Battery& globals::getBattery()
{
	return _battery;
}

periph::Pwm& globals::getPwm()
{
	return _pwm;
}

globals::RgbDriver& globals::getLedDriver()
{
	return _ledDriver;
}