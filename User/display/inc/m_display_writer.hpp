#pragma once



#include <functional>
#include <stdint.h>
#include <stdio.h>



namespace display
{
	template<typename T>
	concept DisplayBasic = requires(T t, int x, int y, int width, int height, bool state, const uint8_t* data, std::function<void()> action)
	{
		t.setPixel(x, y, state);
		t.drawLine(x, y, x, y);
		t.drawRectangle(x, y, width, height);
		t.drawBitmap(x, y, width, height, data);
		t.flush();

		{ t.addDrawAction(std::move(action)) } -> std::same_as<bool>;
		t.clearDrawActions();
		{ reinterpret_cast<const T&>(t).getWidth() } -> std::integral;
		{ reinterpret_cast<const T&>(t).getHeight() } -> std::integral;
	};



	template<typename Derived>
	class DisplayWriter
	{
		public:
			using DrawAction = std::function<void()>;



			virtual ~DisplayWriter() = default;



			void setPixel(int x, int y, bool state)
			{
				static_cast<Derived*>(this)->_setPixel(x, y, state);
			}

			void drawLine(int x0, int y0, int x1, int y1)
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

			void drawRectangle(int x, int y, int width, int height)
			{
				for (int column = y; column < y + height; column++)
				{
					for(int row = x; row < x + width; row++){
						setPixel(row, column, true);
					}
				}
			}

			void drawBitmap(int x, int y, int width, int height, const uint8_t* data)
			{
				int i = 0;
				for (int column = y; column < y + height; column++)
				{
					for(int row = x; row < x + width; row++, i++){
						setPixel(row, column, (data[i / 8] >> (7 - (i % 8))) & 0x01);
					}
				}
			}

			void flush()
			{
				static_cast<Derived*>(this)->_flush();
			}

			bool addDrawAction(DrawAction&& action)
			{
				return static_cast<Derived*>(this)->_addDrawAction(std::move(action));
			}

			void clearDrawActions()
			{
				static_cast<Derived*>(this)->_clearDrawActions();
			}

			int getWidth() const
			{
				return static_cast<const Derived*>(this)->_getWidth();
			}

			int getHeight() const
			{
				return static_cast<const Derived*>(this)->_getHeight();
			}
	};
}