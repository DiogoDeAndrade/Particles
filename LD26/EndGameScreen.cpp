// EndGameScreen.cpp: implementation of the EndGameScreen class.
//
//////////////////////////////////////////////////////////////////////

#include "EndGameScreen.h"
#include "TextureManager.h"
#include "BlitManager.h"
#include "Gfx.h"
#include "Keyboard.h"
#include "Sound.h"
#include "MouseCursor.h"

#define FADE_SPEED			0.001f
#define BUTTON_FADE_SPEED	0.0025f

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

EndGameScreen::EndGameScreen()
{
	_fade_inc=-FADE_SPEED;
	_color.set(1.0f,1.0f,1.0f,1.0f);
}

EndGameScreen::~EndGameScreen()
{
	BLIT->clear_cache();
}

bool EndGameScreen::init(int start_level)
{
	_font_size=60;
	create_font();
	_time_since_last_frame=(float)timeGetTime();
	_exit_code=0;

	if (start_level==99)
	{
		_tex=TEXTURES->find_by_name("endgame2");
	}
	else
	{
		_tex=TEXTURES->find_by_name("endgame1");
	}

	return true;
}

	
void EndGameScreen::render()
{
	GFX->clear(D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER,0.0f,0.0f,0.0f,0.0f,1.0f,0);
	GFX->set_2d_render();
	GFX->begin_scene();

	Vec2f r=GFX->get_resolution();

	GFX->set_camera(r.x*0.5f,r.y*0.5f,0.0f);

	BLIT->clear();
	BLIT->add(_tex,0.0f,0.0f,0.5f,r.x,r.y,1.0f,1.0f,1.0f,1.0f,0.0f,0.0f,1.0f,1.0f,D3DTEXF_POINT);
	BLIT->add(0.0f,0.0f,0.3f,r.x,r.y,_color);
	BLIT->render();
	BLIT->clear();
	GFX->end_scene();
}

void EndGameScreen::anim(float tt)
{
	if (_fade_inc>0.0f)
	{
		_color.w=_color.w+_fade_inc*tt;
		if (_color.w>1.0f) 
		{ 
			_color.w=1.0f; _fade_inc=0.0f; 
			set_exit(true,_exit_code);
		}
	}
	else if (_fade_inc<0.0f)
	{
		_color.w=_color.w+_fade_inc*tt;
		if (_color.w<0.0f) 
		{ 
			_color.w=0.0f; 
			_fade_inc=0.0f; 
		}
	}

	if (KEY->is_pressed(DIK_ESCAPE)) 
	{
		_exit_code=-1;
		_fade_inc=FADE_SPEED;
	}
}

void EndGameScreen::mouse_callback(unsigned long msg,unsigned long wparam,unsigned long lparam)
{
	Screen::mouse_callback(msg,wparam,lparam);

	if (msg==WM_LBUTTONUP)
	{
		_fade_inc=FADE_SPEED;
		_exit_code=-1;
	}
}
