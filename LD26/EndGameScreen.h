
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Screen.h"

class MouseCursor;

class EndGameScreen : public Screen 
{
public:
	EndGameScreen();
	virtual ~EndGameScreen();

			bool	init(int level);
	virtual void	render();
	virtual void	anim(float t);

	virtual	void	mouse_callback(unsigned long msg,unsigned long wparam,unsigned long lparam);

protected:
	float			_fade_inc;
	Color			_color;
	float			_time_since_last_frame;
	int				_tex;
};

