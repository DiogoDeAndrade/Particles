
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Screen.h"

class MouseCursor;

class TitleScreen : public Screen 
{
protected:
	struct Button
	{
		std::string	_text;
		Vec3f		_pos;
		float		_fade_inc;
		Color		_color;
	};
	typedef std::vector<Button>	Buttons;
public:
	TitleScreen();
	virtual ~TitleScreen();

			bool	init(int start_level);
	virtual void	render();
	virtual void	anim(float t);

	virtual	void	mouse_callback(unsigned long msg,unsigned long wparam,unsigned long lparam);

protected:
			void	refresh_options(float alpha,const float fade_inc);
protected:
	float			_fade_inc;
	Color			_color;
	float			_time_since_last_frame;
	Buttons			_buttons;
	Buttons			_options;
	bool			_options_enable;
	MouseCursor*	_cursor;
	std::string		_selected_button;
	std::string		_selected_option;
	
};

