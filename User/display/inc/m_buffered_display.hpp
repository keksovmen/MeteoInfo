#pragma once



#include <functional>
#include <array>
#include <string.h>

#include "m_display_writer.hpp"
#include "m_hal_display.hpp"



namespace display
{
	template<int N, HalDisplayConcept D>
	class BufferedWriter : public DisplayWriter<BufferedWriter<N, D>>
	{
		public:
			using DisplayWriter<BufferedWriter<N, D>>::setPixel;
			using DisplayWriter<BufferedWriter<N, D>>::flush;
			using DisplayWriter<BufferedWriter<N, D>>::addDrawAction;
			using DisplayWriter<BufferedWriter<N, D>>::clearDrawActions;
			using DisplayWriter<BufferedWriter<N, D>>::getWidth;
			using DisplayWriter<BufferedWriter<N, D>>::getHeight;



			BufferedWriter(D& disp)
				: _disp(disp)
			{
				
			}



			void _setPixel(int x, int y, bool state)
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

			void _flush()
			{
				_disp.drawRegion(0, 0, _disp.getWidth(), _disp.getHeight(), _buffer);
			}

			bool _addDrawAction(DisplayWriter<BufferedWriter<N, D>>::DrawAction&& action)
			{
				std::invoke(action);
				return false;
			}

			void _clearDrawActions()
			{
				
			}

			int _getWidth() const
			{
				return _disp.getWidth();
			}

			int _getHeight() const
			{
				return _disp.getHeight();
			}

		
		protected:
			D& _disp;
			uint8_t _buffer[N] = {0};
	};



	template<int N, HalDisplayConcept D>
	class PartitionBufferedWriter : public DisplayWriter<PartitionBufferedWriter<N, D>>
	{
		public:
			using DisplayWriter<PartitionBufferedWriter<N, D>>::setPixel;
			using DisplayWriter<PartitionBufferedWriter<N, D>>::drawLine;
			using DisplayWriter<PartitionBufferedWriter<N, D>>::drawRectangle;
			using DisplayWriter<PartitionBufferedWriter<N, D>>::flush;
			using DisplayWriter<PartitionBufferedWriter<N, D>>::addDrawAction;
			using DisplayWriter<PartitionBufferedWriter<N, D>>::clearDrawActions;
			using DisplayWriter<PartitionBufferedWriter<N, D>>::getWidth;
			using DisplayWriter<PartitionBufferedWriter<N, D>>::getHeight;



			PartitionBufferedWriter(D& disp)
				: _disp(disp)
			{

			}



			void _setPixel(int x, int y, bool state)
			{
				if(_isPixelInRegion(x, y)){
					x = x - _currentX;
					y = y - _currentY;

					const int page = y / _HEIGHT;
					const int shift = y % _HEIGHT;
					const int offset = page * _disp.getWidth() + x;

					if(state){
						_buffer[offset] = _buffer[offset] | (1 << shift);
					}else{
						_buffer[offset] = _buffer[offset] & ~(1 << shift);
					}
					// BufferedWriter<N>::setPixel(x - _currentX, y - _currentY, state);
				}
			}

			void _flush()
			{
				for (int i = 0; i < _disp.getHeight() / _HEIGHT; i++)
				{
					_setRegion(i);
					_applyDrawActions();

					_disp.drawRegion(_currentX, _currentY, _WIDTH, _HEIGHT, _buffer);
				}
			}

			bool _addDrawAction(DisplayWriter<PartitionBufferedWriter<N, D>>::DrawAction&& action)
			{
				_actions[_length] = std::move(action);
				_length++;

				return true;
			}

			void _clearDrawActions()
			{
				_length = 0;
			}

			int _getWidth() const
			{
				return _disp.getWidth();
			}

			int _getHeight() const
			{
				return _disp.getHeight();
			}
		
		private:
			D& _disp;
			uint8_t _buffer[N] = {0};
			std::array<typename DisplayWriter<PartitionBufferedWriter<N, D>>::DrawAction, 10> _actions {};

			const int _WIDTH = N;
			const int _HEIGHT = 8;

			int _length = 0;
			int _currentX = 0;
			int _currentY = 0;



			void _setRegion(int region)
			{
				_currentY = region * 8;
				memset(_buffer, 0, sizeof(_buffer));
			}

			bool _isPixelInRegion(int x, int y) const
			{
				if(x < _currentX || x >= (_currentX + _WIDTH)){
					return false;
				}

				if(y < _currentY || y >= (_currentY + _HEIGHT)){
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