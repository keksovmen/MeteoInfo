#pragma once



#include <functional>
#include <array>
#include <string.h>

#include "m_display_writer.hpp"
#include "m_hal_display.hpp"



namespace display
{
	template<int N>
	class BufferedWriter : public DisplayWriter<BufferedWriter<N>>
	{
		public:
			BufferedWriter(HalDisplayI& disp)
				: _disp(disp)
			{
				
			}



			void setPixel(int x, int y, bool state)
			{
				const int page = y / 8;
				const int shift = y % 8;
				const int offset = page * _disp.getWidth() + x;

				if(state){
					_buffer[offset] = _buffer[offset] | (1 << shift);
				}else{
					_buffer[offset] = _buffer[offset] & ~(1 << shift);
				}
			}

			void flush()
			{
				_disp.drawRegion(0, 0, _disp.getWidth(), _disp.getHeight(), _buffer);
			}

			bool addDrawAction(DisplayWriter<BufferedWriter<N>>::DrawAction&& action)
			{
				std::invoke(action);
				return false;
			}

			void clearDrawActions()
			{
				
			}

			int getWidth() const
			{
				return _disp.getWidth();
			}

			int getHeight() const
			{
				return _disp.getHeight();
			}

		
		protected:
			HalDisplayI& _disp;
			uint8_t _buffer[N] = {0};
	};



	template<int N>
	class PartitionBufferedWriter : public DisplayWriter<PartitionBufferedWriter<N>>
	{
		public:
			// using BufferedWriter<N>::setPixel;



			PartitionBufferedWriter(HalDisplayI& disp)
				: _disp(disp)
			{

			}



			void setPixel(int x, int y, bool state)
			{
				if(_isPixelInRegion(x, y)){
					x = x - _currentX;
					y = y - _currentY;

					const int page = y / 8;
					const int shift = y % 8;
					const int offset = page * _disp.getWidth() + x;

					if(state){
						_buffer[offset] = _buffer[offset] | (1 << shift);
					}else{
						_buffer[offset] = _buffer[offset] & ~(1 << shift);
					}
					// BufferedWriter<N>::setPixel(x - _currentX, y - _currentY, state);
				}
			}

			void flush()
			{
				for (int i = 0; i < _disp.getHeight() / 8; i++)
				{
					_setRegion(i);
					_applyDrawActions();

					_disp.drawRegion(_currentX, _currentY, _width, _height, _buffer);
				}
			}

			bool addDrawAction(DisplayWriter<PartitionBufferedWriter<N>>::DrawAction&& action)
			{
				_actions[_length] = std::move(action);
				_length++;

				return true;
			}

			void clearDrawActions()
			{
				_length = 0;
			}

			int getWidth() const
			{
				return _disp.getWidth();
			}

			int getHeight() const
			{
				return _disp.getHeight();
			}
		
		private:
			HalDisplayI& _disp;
			uint8_t _buffer[N] = {0};
			std::array<typename DisplayWriter<PartitionBufferedWriter<N>>::DrawAction, 10> _actions {};

			int _length = 0;
			int _currentX = 0;
			int _currentY = 0;
			const int _width = N;
			const int _height = 8;



			void _setRegion(int region)
			{
				_currentY = region * 8;
				memset(_buffer, 0, sizeof(_buffer));
			}

			bool _isPixelInRegion(int x, int y) const
			{
				if(x < _currentX || x >= (_currentX + _width)){
					return false;
				}

				if(y < _currentY || y >= (_currentY + _height)){
					return false;
				}

				return true;
			}

			void _applyDrawActions()
			{
				for (int i = 0; i < _length; i++)
				{
					std::invoke(_actions[i]);
				}
			}
	};
}