#include "TextObj.h"
#include "Screen.h"

TextObj::TextObj(std::string text)
{
	_text=text;
}

TextObj::~TextObj(void)
{
}

void TextObj::draw()
{
	if (!_draw) return;
	
	SCREEN->write_text(_pos.x,_pos.y,_color.x,_color.y,_color.z,_color.w,_text.c_str());
}

void TextObj::anim()
{

}
