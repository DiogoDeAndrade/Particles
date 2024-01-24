#pragma once

#define MAX_MOUSE_BUTTONS	3

class Mouse
{
public:
	Mouse(void);
	virtual ~Mouse(void);

	virtual	bool	init();
	virtual	void	poll();

	virtual	bool	button_pressed(int n);
	virtual	bool	button_clicked(int n);

protected:
	bool	_button_state[MAX_MOUSE_BUTTONS];
	bool	_last_button_state[MAX_MOUSE_BUTTONS];
};

extern Mouse* MOUSE;