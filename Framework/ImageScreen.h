// ImageScreen.h: interface for the ImageScreen class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_IMAGESCREEN_H__17C467A6_9767_475A_8263_5FE9A6B923BF__INCLUDED_)
#define AFX_IMAGESCREEN_H__17C467A6_9767_475A_8263_5FE9A6B923BF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../Framework/Screen.h"
#include <d3dx9.h>

class ImageScreen : public Screen 
{
public:
	ImageScreen();
	virtual ~ImageScreen();

			bool	init(char *filename);
	virtual void	render();
	virtual void	anim(float t);

	virtual	void	mouse_callback(unsigned long msg,unsigned long wparam,unsigned long lparam);

protected:
	float			_fade_inc;
	Color			_color;
	float			_time_since_last_frame;
};

#endif // !defined(AFX_IMAGESCREEN_H__17C467A6_9767_475A_8263_5FE9A6B923BF__INCLUDED_)
