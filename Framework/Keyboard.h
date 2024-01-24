// Keyboard.h: interface for the Keyboard class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_KEYBOARD_H__44FBF507_24FD_415A_9637_F21B1D3848F5__INCLUDED_)
#define AFX_KEYBOARD_H__44FBF507_24FD_415A_9637_F21B1D3848F5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Defs.h"

class Keyboard  
{
public:
	Keyboard();
	virtual ~Keyboard();

			bool	init(HINSTANCE instance);
			void	poll();
	inline	int		is_pressed(int key) { return (_last_state[key] & 0x80); }
			int		any_pressed();

protected:
	LPDIRECTINPUTDEVICE8	_keyboard;
	unsigned char			_last_state[256];
};

extern	Keyboard*		KEY;

#endif // !defined(AFX_KEYBOARD_H__44FBF507_24FD_415A_9637_F21B1D3848F5__INCLUDED_)
