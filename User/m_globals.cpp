#include "m_globals.hpp"



static periph::I2C_Hal _i2c;

static display::HalDisplaySSD1315 _ssd1315(_i2c);
static display::PartitionBufferedWriter<128, decltype(_ssd1315)> _displayWriter(_ssd1315);
static display::FontWriter<decltype(_displayWriter)> _font(_displayWriter);
static display::GraphDrawer<decltype(_displayWriter)> _graph(_displayWriter, _font);

static periph::Aht20 _aht(_i2c);



periph::I2C_Hal& globals::getI2c()
{
	return _i2c;
}

display::HalDisplaySSD1315& globals::getDisplayHal()
{
	return _ssd1315;
}

display::PartitionBufferedWriter<128, display::HalDisplaySSD1315>& globals::getDisplayWriter()
{
	return _displayWriter;
}

display::FontWriter<display::PartitionBufferedWriter<128, display::HalDisplaySSD1315>>& globals::getFontWriter()
{
	return _font;
}

display::GraphDrawer<display::PartitionBufferedWriter<128, display::HalDisplaySSD1315>>& globals::getGraphWriter()
{
	return _graph;
}

periph::Aht20& globals::getSensor()
{
	return _aht;
}
