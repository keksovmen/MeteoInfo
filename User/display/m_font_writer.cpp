#include "m_font_writer.hpp"



using namespace display;



extern const uint8_t glyphs_small[];
extern const uint8_t glyphs_medium[];



const std::array<FontWriter::FontEntry, static_cast<int>(FontWriter::FontSize::COUNT)> FontWriter::_fonts = {
	{{8, 8, 6, ' ', 'Z', glyphs_small},
	{8, 16, 8, ' ', 'Z', glyphs_medium},
	}
};

FontWriter::FontWriter(DisplayWriter& disp)
	: _disp(disp), _activeFont(&_fonts[0])
{

}

void FontWriter::drawChar(int x, int y, char c)
{
	const uint8_t* letter = _activeFont->symbols;
	if(c < _activeFont->firstLetter || c > _activeFont->lastLetter){
		//error case
	}else{
		letter = &_activeFont->symbols[(c - _activeFont->firstLetter) * _activeFont->height];
	}

	_disp.drawBitmap(x, y, _activeFont->width, _activeFont->height, letter);
}

void FontWriter::drawStr(int x, int y, std::string_view str)
{
	for (size_t i = 0; i < str.length(); i++) {
        drawChar(x + i * _activeFont->spaceSize, y, str[i]);
    }
}