#pragma once



#include <concepts>
#include <stdint.h>



namespace display
{
	template<typename T>
	concept HalDisplayConcept = requires(T t, int x, int y, int width, int height, uint8_t* data)
	{
		{ t.init() } -> std::same_as<bool>;
		t.turnOn();
		t.turnOff();
		{ reinterpret_cast<const T&>(t).isTurnedOn() } -> std::same_as<bool>;
		{ reinterpret_cast<const T&>(t).getWidth() } -> std::integral;
		{ reinterpret_cast<const T&>(t).getHeight() } -> std::integral;
		t.drawPixel(x, y);
		t.drawRegion(x, y, width, height, data);
		t.clearRegion(x, y, width, height);
		t.clearScreen();
	};



	template<typename Derived>
	class HalDisplayI
	{
		public:
			~HalDisplayI() = default;
			
			bool init()
			{
				return static_cast<Derived*>(this)->_init();	
			}

			void turnOn()
			{
				static_cast<Derived*>(this)->_turnOn();	
			}

			void turnOff()
			{
				static_cast<Derived*>(this)->_turnOff();	
			}

			bool isTurnedOn() const
			{
				return static_cast<const Derived*>(this)->_isTurnedOn();	
			}

			int getWidth() const
			{
				return static_cast<const Derived*>(this)->_getWidth();	
			}

			int getHeight() const
			{
				return static_cast<const Derived*>(this)->_getHeight();	
			}

			void drawPixel(int x, int y)
			{
				static_cast<Derived*>(this)->_drawPixel(x, y);	
			}

			void drawRegion(int x, int y, int width, int height, uint8_t* data)
			{
				static_cast<Derived*>(this)->_drawRegion(x, y, width, height, data);	
			}

			void clearRegion(int x, int y, int width, int height)
			{
				static_cast<Derived*>(this)->_clearRegion(x, y, width, height);	
			}

			void clearScreen(){
				clearRegion(0, 0, getWidth(), getHeight());
			}
	};
}