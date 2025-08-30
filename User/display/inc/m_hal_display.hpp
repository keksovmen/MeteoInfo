#pragma once



#include <stdint.h>



namespace display
{
	class HalDisplayI
	{
		public:
			virtual ~HalDisplayI() = default;
			
			virtual bool init() = 0;

			virtual void turnOn() = 0;
			virtual void turnOff() = 0;
			virtual bool isTurnedOn() const = 0;

			virtual int getWidth() const = 0;
			virtual int getHeight() const = 0;

			virtual void drawPixel(int x, int y) = 0;
			virtual void drawRegion(int x, int y, int width, int height, uint8_t* data) = 0;
			virtual void clearRegion(int x, int y, int width, int height) = 0;

			void clearScreen(){
				clearRegion(0, 0, getWidth(), getHeight());
			}
	};
}