#include "m_buffered_display.hpp"

#include <cmath>
#include <cstring>
// #include <assert.h>

#include "debug.h"



using namespace display;



template<int N>
BufferedWriter<N>::BufferedWriter(HalDisplayI& disp)
	: _disp(disp)
{

}

template<int N>
void BufferedWriter<N>::setPixel(int x, int y, bool state)
{
	const int page = y / 8;
	const int shift = y % 8;
	const int offset = page * _disp.getWidth() + x;

	if(page != 0){
		printf("x=%d\ty=%d\r\n", x, y);
	}

	// assert(page == 0);

	if(state){
		_buffer[offset] = _buffer[offset] | (1 << shift);
	}else{
		_buffer[offset] = _buffer[offset] & ~(1 << shift);
	}
}

template<int N>
void BufferedWriter<N>::drawLine(int x0, int y0, int x1, int y1)
{
	int dx = abs(x1 - x0);
    int dy = abs(y1 - y0);
    int sx = (x0 < x1) ? 1 : -1;
    int sy = (y0 < y1) ? 1 : -1;
    int err = (dx > dy ? dx : -dy) / 2;
    int e2;
    
    while (1) {
        setPixel(x0, y0, true);
        if (x0 == x1 && y0 == y1) break;
        
        e2 = err;
        if (e2 > -dx) {
            err -= dy;
            x0 += sx;
        }
        if (e2 < dy) {
            err += dx;
            y0 += sy;
        }
    }
}

template<int N>
void BufferedWriter<N>::drawRectangle(int x, int y, int width, int height)
{
	for (int column = y; column < y + height; column++)
	{
		for(int row = x; row < x + width; row++){
			setPixel(row, column, true);
		}
	}
}

template<int N>
void BufferedWriter<N>::drawBitmap(int x, int y, int width, int height, const uint8_t* data)
{
	int i = 0;
	for (int column = y; column < y + height; column++)
	{
		for(int row = x; row < x + width; row++, i++){
			setPixel(row, column, (data[i / 8] >> (7 - (i % 8))) & 0x01);
		}
	}
}

template<int N>
void BufferedWriter<N>::flush()
{
	_disp.drawRegion(0, 0, _disp.getWidth(), _disp.getHeight(), _buffer);
}




template<int N>
PartitionBufferedWriter<N>::PartitionBufferedWriter(HalDisplayI& disp)
	: BufferedWriter<N>(disp)
{

}

template<int N>
void PartitionBufferedWriter<N>::setPixel(int x, int y, bool state)
{
	if(_isPixelInRegion(x, y)){
		BufferedWriter<N>::setPixel(x - _currentX, y - _currentY, state);
	}
}

template<int N>
void PartitionBufferedWriter<N>::flush()
{
	for (int i = 0; i < BufferedWriter<N>::_disp.getHeight() / 8; i++)
	{
		_setRegion(i);
		_applyDrawActions();

		BufferedWriter<N>::_disp.drawRegion(_currentX, _currentY, _width, _height, BufferedWriter<N>::_buffer);
	}
}

template<int N>
bool PartitionBufferedWriter<N>::addDrawAction(DrawAction&& action)
{
	_actions[_length] = std::move(action);
	_length++;

	return true;
}

template<int N>
void PartitionBufferedWriter<N>::_setRegion(int region)
{
	_currentY = region * 8;
	memset(BufferedWriter<N>::_buffer, 0, sizeof(BufferedWriter<N>::_buffer));
}

template<int N>
bool PartitionBufferedWriter<N>::_isPixelInRegion(int x, int y) const
{
	if(x < _currentX || x >= (_currentX + _width)){
		return false;
	}

	if(y < _currentY || y >= (_currentY + _height)){
		return false;
	}

	return true;
}


template<int N>
void PartitionBufferedWriter<N>::_applyDrawActions()
{
	for (int i = 0; i < _length; i++)
	{
		std::invoke(_actions[i], *this);
	}
}
