#include "m_hal_ssd1315.hpp"



#define _CMD_DISPLAY_OFF 0xAE
#define _CMD_DISPLAY_ON 0xAF

#define _SCREEN_WIDTH 128
#define _SCREEN_HEIGHT 64

#define _I2C_ADDRESS 0x3C



using namespace display;



HalDisplaySSD1315::HalDisplaySSD1315(periph::I2C_Hal& i2c)
	: _i2c(i2c)
{

}

bool HalDisplaySSD1315::_init()
{
	turnOff();

    _writeCommand(0x20);  // Set Memory Addressing Mode
    _writeCommand(0x00);  // Horizontal addressing mode

    _writeCommand(0xC8);  // Set COM Output Scan Direction

    _writeCommand(0x40);  // Set start line address
    _writeCommand(0x81);  // Set contrast control
    _writeCommand(0x7F);  // Contrast value (50%)

    _writeCommand(0xA1);  // Set segment re-map
    _writeCommand(0xA6);  // Set normal display
    _writeCommand(0xA8);  // Set multiplex ratio
    _writeCommand(0x3F);  // 64MUX

    _writeCommand(0xA4);  // Output follows RAM content
    _writeCommand(0xD3);  // Set display offset
    _writeCommand(0x00);  // No offset

    _writeCommand(0xD5);  // Set display clock divide ratio
    _writeCommand(0x80);  // Default ratio

    _writeCommand(0xD9);  // Set pre-charge period
    _writeCommand(0xF1);  // Phase 1 = 15, Phase 2 = 1

    _writeCommand(0xDA);  // Set COM pins hardware configuration
    _writeCommand(0x12);  // Alternative COM pin configuration

    _writeCommand(0xDB);  // Set VCOMH
    _writeCommand(0x40);  // VCOMH = 0.83 * VCC

    _writeCommand(0x8D);  // Set DC-DC enable
    _writeCommand(0x14);  // Enable charge pump

    turnOn();

    return true;
}

void HalDisplaySSD1315::_turnOn()
{
    _writeCommand(_CMD_DISPLAY_ON);  // Display ON
	_isOn = true;
}

void HalDisplaySSD1315::_turnOff()
{
	_writeCommand(_CMD_DISPLAY_OFF);  // Display OFF
	_isOn = false;
}

bool HalDisplaySSD1315::_isTurnedOn() const
{
    return _isOn;
}

int HalDisplaySSD1315::_getWidth() const
{
    return _SCREEN_WIDTH;
}

int HalDisplaySSD1315::_getHeight() const
{
    return _SCREEN_HEIGHT;
}

void HalDisplaySSD1315::_drawPixel(int x, int y)
{
	_setDrawRect(x, y, 1, 1);
	int bitToLight = y % 8;

	_writeData(1 << bitToLight);
}

void HalDisplaySSD1315::_drawRegion(int x, int y, int width, int height, uint8_t* data)
{
	_setDrawRect(x, y, width, height);

	_i2c.startSignal();
	_i2c.writeRequest(_I2C_ADDRESS);
	_i2c.writeData(0x40);

    for (uint8_t row = 0; row < (height + 7) / 8; row++) {
        for (uint8_t i = 0; i < width; i++) {
			_i2c.writeData(data[row * width + i]);
        }
    }

	_i2c.stopSignal();
}

void HalDisplaySSD1315::_clearRegion(int x, int y, int width, int height)
{
	_setDrawRect(x, y, width, height);

	_i2c.startSignal();
	_i2c.writeRequest(_I2C_ADDRESS);
	_i2c.writeData(0x40);

	for (int i = 0; i < (width * height) / 8; i++)
	{
		_i2c.writeData(0x00);
	}
	
	_i2c.stopSignal();
}

void HalDisplaySSD1315::_writeCommand(uint8_t cmd)
{
	_i2c.startSignal();
	_i2c.writeRequest(_I2C_ADDRESS);
	_i2c.writeData(0x00);
	_i2c.writeData(cmd);
	_i2c.stopSignal();
}

void HalDisplaySSD1315::_writeData(uint8_t data)
{
	_i2c.startSignal();
	_i2c.writeRequest(_I2C_ADDRESS);
	_i2c.writeData(0x40);
	_i2c.writeData(data);
	_i2c.stopSignal();
}

void HalDisplaySSD1315::_setDrawRect(uint8_t x, uint8_t y, uint8_t width, uint8_t height)
{
	//TODO: add assertions

    //set low and high column window (X coordinate range)
    _writeCommand(0x21);    //cmd to start
    _writeCommand(x);    //low
    _writeCommand(x + width - 1);    //high

    //set low and high column window (Y coordinate range)
    _writeCommand(0x22);    //cmd to start
    _writeCommand(y / 8);    //low
    _writeCommand((y + height + 7) / 8);    //high
}