#pragma once



#include <array>
#include <string_view>
#include <stdint.h>

#include "m_buffered_display.hpp"
#include "m_display_writer.hpp"



extern const uint8_t glyphs_small[];



namespace display
{	
	template<DisplayBasic D>
	class FontWriter
	{
		public:
			enum class FontSize : int
			{
				SMALL,
				// MEDIUM,
				COUNT
			};



			FontWriter(D& disp)
				: _disp(disp), _activeFont(&_fonts[0])
			{

			}

			void drawChar(int x, int y, char c)
			{
				const uint8_t* letter = _activeFont->symbols;
				if(c < _activeFont->firstLetter || c > _activeFont->lastLetter){
					//error case
				}else{
					letter = &_activeFont->symbols[(c - _activeFont->firstLetter) * _activeFont->height];
				}

				_disp.drawBitmap(x, y, _activeFont->width, _activeFont->height, letter);
			}

			void drawStr(int x, int y, std::string_view str)
			{
				for (size_t i = 0; i < str.length(); i++) {
					drawChar(x + i * _activeFont->spaceSize, y, str[i]);
				}
			}

			template<FontSize S>
			constexpr void changeSize()
			{
				static_assert(static_cast<int>(S) < static_cast<int>(FontSize::COUNT));
				_activeFont = &_fonts[static_cast<int>(S)];

			}

			template<FontSize S>
			constexpr int calcLength(std::string_view str) const
			{
				return str.length() * _fonts[static_cast<int>(S)].spaceSize;
			}

			template<FontSize S>
			constexpr int getFontHeight() const
			{
				return _fonts[static_cast<int>(S)].height;
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



			constexpr static const std::array<FontEntry, static_cast<int>(FontSize::COUNT)> _fonts = {
				{{8, 8, 6, ' ', 'Z', glyphs_small},
				// {8, 16, 8, ' ', 'Z', glyphs_medium},
				}
			};



			D& _disp;
			const FontEntry* _activeFont;
	};
}