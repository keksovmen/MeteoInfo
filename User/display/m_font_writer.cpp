#include "m_font_writer.hpp"



using namespace display;



extern const uint8_t glyphs[];



const std::array<FontWriter::FontEntry, 1> FontWriter::_fonts = {
	{{8, 16, ' ', 'Z', glyphs}}
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
        drawChar(x + i * _activeFont->width, y, str[i]);
    }
}