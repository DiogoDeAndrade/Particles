#include "Mouse.h"
#include <windows.h>

Mouse* MOUSE=NULL;

Mouse::Mouse(void)
{
	MOUSE=this;
}

Mouse::~Mouse(void)
{
	MOUSE=NULL;
}

bool Mouse::init()
{
	memset(&_button_state,0,sizeof(bool)*MAX_MOUSE_BUTTONS);
	memset(&_last_button_state,0,sizeof(bool)*MAX_MOUSE_BUTTONS);

	return true;
}

void Mouse::poll()
{
	memcpy(&_last_button_state,&_button_state,sizeof(bool)*MAX_MOUSE_BUTTONS);

	if (GetAsyncKeyState(VK_LBUTTON)&0x8000) _button_state[0]=true; else _button_state[0]=false;
	if (GetAsyncKeyState(VK_RBUTTON)&0x8000) _button_state[1]=true; else _button_state[1]=false;
	if (GetAsyncKeyState(VK_MBUTTON)&0x8000) _button_state[2]=true; else _button_state[2]=false;
}

bool Mouse::button_pressed(int n)
{
	return _button_state[n];
}

bool Mouse::button_clicked(int n)
{
	if (_last_button_state[n])
	{
		return !_button_state[n];
	}
	return false;
}
