#pragma once
#include "Sprite.h"

class MouseCursor :
	public Sprite
{
public:
	MouseCursor(const char* cursor_texture);
	virtual ~MouseCursor(void);

	virtual	void	anim(float t);
};

extern MouseCursor*	CURSOR;