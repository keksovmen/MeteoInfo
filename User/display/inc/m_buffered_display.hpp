#pragma once



#include <functional>
#include <array>

#include "m_display_writer.hpp"
#include "m_hal_display.hpp"



namespace display
{
	template<int N>
	class BufferedWriter : public DisplayWriter
	{
		public:
			BufferedWriter(HalDisplayI& disp);



			virtual void setPixel(int x, int y, bool state) override;
			virtual void drawLine(int x0, int y0, int x1, int y1) override;
			virtual void drawRectangle(int x, int y, int width, int height) override;
			virtual void drawBitmap(int x, int y, int width, int height, const uint8_t* data) override;
			virtual void flush() override;

			virtual bool addDrawAction(DisplayWriter::DrawAction&& action) override;

			virtual int getWidth() const override;
			virtual int getHeight() const override;

		
		protected:
			HalDisplayI& _disp;
			uint8_t _buffer[N] = {0};
	};



	template<int N>
	class PartitionBufferedWriter : public BufferedWriter<N>
	{
		public:
			// using DrawAction = std::function<void(PartitionBufferedWriter&)>;



			PartitionBufferedWriter(HalDisplayI& disp);



			virtual void setPixel(int x, int y, bool state) override;
			virtual void flush() override;

			virtual bool addDrawAction(DisplayWriter::DrawAction&& action) override;
		
		private:
			std::array<DisplayWriter::DrawAction, 10> _actions {};

			int _length = 0;
			int _currentX = 0;
			int _currentY = 0;
			const int _width = N;
			const int _height = 8;



			void _setRegion(int region);
			bool _isPixelInRegion(int x, int y) const;
			void _applyDrawActions();
	};

	

	template class PartitionBufferedWriter<128>;
}