#pragma once



#include <array>
#include <string_view>
#include <stdint.h>

#include "m_display_writer.hpp"



namespace display
{
	class FontWriter
	{
		public:
			enum class FontSize : int
			{
				SMALL,
				MEDIUM,
				COUNT
			};



			FontWriter(DisplayWriter& disp);



			void drawChar(int x, int y, char c);
			void drawStr(int x, int y, std::string_view str);

			template<FontSize S>
			constexpr void changeSize()
			{
				static_assert(static_cast<int>(S) < static_cast<int>(FontSize::COUNT));
				_activeFont = &_fonts[static_cast<int>(S)];

			}


		private:
			struct FontEntry
			{
				int width;
				int height;
				int spaceSize;
				char firstLetter;
				char lastLetter;
				const uint8_t* symbols;
			};



			static const std::array<FontEntry, static_cast<int>(FontSize::COUNT)> _fonts;



			DisplayWriter& _disp;
			const FontEntry* _activeFont;
	};
}