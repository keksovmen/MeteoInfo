#include "k_font.h"



extern const uint8_t glyphs_medium[];
// const uint16_t glyphs[58][16];



const uint8_t* k_font_get_glyph(char value)
{
	if(value < K_FONT_FIRST_SYMBOL || value > K_FONT_LAST_SYMBOL){
		return glyphs_medium;
	}

	return &glyphs_medium[(value - K_FONT_FIRST_SYMBOL) * K_FONT_GLYPH_HEIGHT];
}
