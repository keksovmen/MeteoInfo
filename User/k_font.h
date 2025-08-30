#pragma once



#include <stdint.h>



#define K_FONT_GLYPH_WIDTH 8
#define K_FONT_GLYPH_HEIGHT 16
//when write string determine next x position, x0 = x, x1 = x + SPACING
#define K_FONT_GLYPH_SPACING 0

#define K_FONT_FIRST_SYMBOL ' '
#define K_FONT_LAST_SYMBOL 'Z'


#ifdef __cplusplus
extern "C" {
#endif

//always return something, in case of miss, will return first symbol in the table
const uint8_t* k_font_get_glyph(char value);

#ifdef __cplusplus
}
#endif