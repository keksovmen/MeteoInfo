#pragma once



#include <functional>
#include <stdint.h>



namespace display
{
	class DisplayWriter
	{
		public:
			using DrawAction = std::function<void()>;



			virtual ~DisplayWriter() = default;



			virtual void setPixel(int x, int y, bool state) = 0;
			virtual void drawLine(int x0, int y0, int x1, int y1) = 0;
			virtual void drawRectangle(int x, int y, int width, int height) = 0;
			virtual void drawBitmap(int x, int y, int width, int height, const uint8_t* data) = 0;
			virtual void flush() = 0;

			virtual bool addDrawAction(DrawAction&& action) = 0;
			virtual void clearDrawActions() = 0;

			virtual int getWidth() const = 0;
			virtual int getHeight() const = 0;
	};
}