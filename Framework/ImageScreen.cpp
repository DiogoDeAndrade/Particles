// ImageScreen.cpp: implementation of the ImageScreen class.
//
//////////////////////////////////////////////////////////////////////

#include "ImageScreen.h"
#include "../framework/TextureManager.h"
#include "../framework/BlitManager.h"
#include "../framework/Gfx.h"
#include "../framework/Keyboard.h"
#include "../Framework/Sound.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

ImageScreen::ImageScreen()
{
	_fade_inc=0.00025f;
	_color.set(0.0f,0.0f,0.0f,1.0f);
}

ImageScreen::~ImageScreen()
{
	TEXTURES->clear("Image");
	BLIT->clear_cache();
}

bool ImageScreen::init(char *filename)
{
	if (TEXTURES->load("Image",filename)==-1) return false;

	_time_since_last_frame=(float)timeGetTime();
	_exit_code=0;

	return true;
}

	
void ImageScreen::render()
{
	GFX->clear(D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER,0.0f,0.0f,0.0f,0.0f,1.0f,0);
	GFX->set_2d_render();
	GFX->begin_scene();

	Vec2f r=GFX->get_resolution();

	GFX->set_camera(r.x*0.5f,r.y*0.5f,0.0f);


	int tex=TEXTURES->find_by_name("Image");
	BLIT->clear();
	BLIT->add(tex,0.0f,0.0f,0.5f,r.x,r.y,_color.x,_color.y,_color.z,1.0f,0.0f,0.0f,1.0f,1.0f);
	BLIT->render();
	BLIT->clear();
	GFX->end_scene();
}

void ImageScreen::anim(float tt)
{
	float t=timeGetTime()-_time_since_last_frame;
	if (t>50.0f) t=50.0f;

	_color.x=_color.x+_fade_inc*t; if (_color.x<0.0f) { _color.x=0.0f; set_exit(true,_exit_code); } else if (_color.x>1.0f) _color.x=1.0f;
	_color.y=_color.y+_fade_inc*t; if (_color.y<0.0f) { _color.y=0.0f; set_exit(true,_exit_code); } else if (_color.y>1.0f) _color.y=1.0f;
	_color.z=_color.z+_fade_inc*t; if (_color.z<0.0f) { _color.z=0.0f; set_exit(true,_exit_code); } else if (_color.z>1.0f) _color.z=1.0f;
	if ((KEY->is_pressed(DIK_SPACE)) ||
		(KEY->is_pressed(DIK_RETURN)) ||
		(KEY->is_pressed(DIK_ESCAPE)) ||
		(KEY->is_pressed(DIK_LSHIFT)))
	{
		if (KEY->is_pressed(DIK_ESCAPE)) _exit_code=-1;
		_fade_inc=-0.00025f;
	}
}

void ImageScreen::mouse_callback(unsigned long msg,unsigned long wparam,unsigned long lparam)
{
	switch (msg)
	{
		case WM_RBUTTONUP:
		case WM_LBUTTONUP:
			_fade_inc=-0.00025f;
			break;
	}
}
