#pragma once
#include "Drawable.h"

class TextObj :
	public Drawable
{
public:
	TextObj(std::string text);
	virtual ~TextObj(void);

	virtual	void	draw();
	virtual void	anim();

protected:
	std::string	_text;
};
