// Screen.cpp: implementation of the Screen class.
//
//////////////////////////////////////////////////////////////////////

#include "Screen.h"
#include "Gfx.h"
#include "Sound.h"
#include "Mouse.h"
#include "Keyboard.h"
#include "Timers.h"
#include "Defs.h"
#include "TextureManager.h"
#include "Tokenizer.h"
#include "Utils.h"
#include <algorithm>

void ScreenMouseCallback(unsigned long msg,unsigned long wparam,unsigned long lparam)
{
	SCREEN->mouse_callback(msg,wparam,lparam);
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Screen*	SCREEN=NULL;

Screen::Screen()
{
	_exit=false;
	_font_size=26;
	_sprite=NULL;
	_font=NULL;
	set_mouse_event_callback(ScreenMouseCallback);

	HWND desktop=GetDesktopWindow();
	RECT desktop_rect;
	GetWindowRect(desktop,&desktop_rect);

	_desktop_res.x=(float)(desktop_rect.right-desktop_rect.left);
	_desktop_res.y=(float)(desktop_rect.bottom-desktop_rect.right);
}

Screen::~Screen()
{
	if (SCREEN==this) SCREEN=NULL;
	if (_sprite) _sprite->Release(); _sprite=NULL;
	if (_font) _font->Release(); _font=NULL; 
	set_mouse_event_callback(NULL);
	remove_drawables();
	for (unsigned long i=0; i<_drawables.size(); i++)
	{
		if (_drawables[i])
		{
			delete _drawables[i];
			_drawables[i]=NULL;
		}
	}
	_drawables.clear();
}

long Screen::run()
{
	// Wait for keyboard to be idle
	while (1)
	{
		KEY->poll();
		if (!KEY->any_pressed()) break;
	}

	MSG				msg;
	unsigned long	t;

	unsigned long	lastTime;
	float			ms;

	SCREEN=this;

	_exit=false;

	lastTime=timeGetTime();

	while (!_exit)
	{
		ms=1000.0f/50.0f;

		while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		// Stop app going more than a given frames per second
		do
		{
			t=timeGetTime();
		} while (t-lastTime<ms);
		float elapsed=float(t)-float(lastTime);
		if (elapsed>(1000.0f/30.0f)) elapsed=1000.0f/30.0f;
		run(elapsed);
		lastTime=t;
	}

	SCREEN=NULL;

	return _exit_code;
}

class DrawableSortOrder
{
public:
	bool operator()(Drawable* o1,Drawable* o2)
	{
		return o1->_pos.z>o2->_pos.z;
	}
};

void Screen::run(float t)
{
	if (KEY) KEY->poll();
	if (MOUSE) MOUSE->poll();
	if (TIMERS) TIMERS->poll();

	anim(t);

	remove_drawables();

	// Sort drawables
	std::sort(_drawables.begin(),_drawables.end(),DrawableSortOrder());

	if (!_exit)
	{
		render();
		GFX->swap_buffers();
	}
}

void Screen::remove_drawables()
{
	Removes::iterator i=_removes.begin(); 
	while (i!=_removes.end())
	{
		Drawables::iterator j=_drawables.begin();
		while (j!=_drawables.end())
		{
			if ((*j)==(*i).drawable) 
			{
				j=_drawables.erase(j);
				break;
			}
			else ++j;
		}
		if ((*i).should_delete)
		{
			delete (*i).drawable;
		}
		i++;
	}
	_removes.clear();
}

void Screen::load_assets(std::string filename)
{
	FILE* file=NULL;
	fopen_s(&file,filename.c_str(),"rt");
	if (!file) 
	{
		ERROR_MSG("Can't find assets file!");
		return;
	}

	Tokenizer		tokens;
	char			buffer[1024];
	unsigned long	n;

	while (!feof(file))
	{
		fgets((char*)&buffer,1024,file);
		tokens.clear_tokens();
		n=tokens.tokenize((char*)&buffer);
		if (n>0)
		{
			char* type=tokens.get_token(0);
			if (!_stricmp(type,"texture"))
			{
				char* internal_name=tokens.get_token(1);
				char* filename=tokens.get_token(2);

				TEXTURES->load(internal_name,filename);
			}
			else if (!_stricmp(type,"sound"))
			{
				char* internal_name=tokens.get_token(1);
				char* filename=tokens.get_token(2);
				char* loops=tokens.get_token(3);
				char* streams=tokens.get_token(4);
				bool loop;
				if (!_stricmp(loops,"true")) loop=true;
				else loop=false;
				bool stream;
				if (!_stricmp(streams,"true")) stream=true;
				else stream=false;

				SOUND->load(internal_name,filename,stream,loop);
			}
		}
	}

	fclose(file);
}

bool Screen::create_font()
{
	_font=NULL;
	HRESULT hRet=D3DXCreateFont(GFX->get_device(),
								_font_size,0,
								FW_BOLD,
								0,
								FALSE,
								ANSI_CHARSET,
								OUT_DEFAULT_PRECIS,
								DEFAULT_QUALITY,
								DEFAULT_PITCH | FF_DONTCARE,
								"MV Boli",&_font);
	if (hRet!=D3D_OK)
	{
		ERROR_MSG("Can't create font - D3D error!");
		return false;
	}

	hRet=D3DXCreateSprite(GFX->get_device(),&_sprite);
	if (hRet!=D3D_OK)
	{
		_font->Release();
		_font=NULL;
		return false;
	}

	return true;
}

void Screen::delete_font()
{
	if (_font)
	{
		_font->Release();
		_font=NULL;
	}
}

void Screen::write_text(float x,float y,float r,float g,float b,float a,const char *string,const unsigned long flags)
{
	if (!_font) return;

	RECT		rect;
	D3DCOLOR	color;
	Vec3f		c=GFX->get_color_modulation();

	color=D3DCOLOR_COLORVALUE(r*c.x,g*c.y,b*c.z,a);

	float rx=1.0f;
	float ry=1.0f;

	rect.left=long(x*rx);
	rect.top=long(y*ry);
//	rect.left=long(x);
//	rect.top=long(y);
	rect.bottom=long(y+1);
	rect.right=long(x+1);

	GFX->begin_scene();
	GFX->set_pixel_shader(NULL);

	unsigned long f=DT_LEFT;
	if (flags&TEXT_CENTER) f=DT_CENTER;
	else f=DT_LEFT;

	if (flags&TEXT_BOTTOM) f|=DT_BOTTOM;

	_font->DrawText(NULL,string,-1,&rect,DT_CALCRECT|f,color);

	D3DXMATRIX	matrix;
//	D3DXMatrixTranslation(&matrix,0.0f,0.0f,0.5f);
	_sprite->Begin(D3DXSPRITE_ALPHABLEND|D3DXSPRITE_SORT_TEXTURE|D3DXSPRITE_SORT_DEPTH_BACKTOFRONT);
//	_sprite->SetTransform(&matrix);
	if (flags&TEXT_SHADOW) 
	{
		RECT rect2=rect;
		rect2.left++;
		rect2.right++;
		rect2.top++;
		rect2.bottom++;
		_font->DrawText(_sprite,string,-1,&rect2,f,color&0xFF000000);
	}
	_font->DrawText(_sprite,string,-1,&rect,f,color);
	_sprite->End();
}

Rect Screen::get_text_rect(float x,float y,const char *string,const unsigned long flags)
{
	if (!_font) return Rect(0.0f,0.0f,0.0f,0.0f);

	unsigned long f=DT_LEFT;
	if (flags&TEXT_CENTER) f=DT_CENTER;
	else f=DT_LEFT;

	if (flags&TEXT_BOTTOM) f|=DT_BOTTOM;

	RECT		rect;

	rect.left=long(x);
	rect.top=long(y);
	rect.bottom=long(y+1);
	rect.right=long(x+1);

	_font->DrawText(NULL,string,-1,&rect,DT_CALCRECT|f,0x00000000);

	return Rect((float)rect.left,(float)rect.top,(float)rect.right,(float)rect.bottom);
}

void Screen::mouse_callback(unsigned long msg,unsigned long wparam,unsigned long lparam)
{
}
