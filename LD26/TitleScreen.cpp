// TitleScreen.cpp: implementation of the TitleScreen class.
//
//////////////////////////////////////////////////////////////////////

#include "TitleScreen.h"
#include "TextureManager.h"
#include "BlitManager.h"
#include "Gfx.h"
#include "Keyboard.h"
#include "Sound.h"
#include "MouseCursor.h"
#include "Options.h"

#define FADE_SPEED			0.001f
#define BUTTON_FADE_SPEED	0.0025f

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

TitleScreen::TitleScreen()
{
	_fade_inc=-FADE_SPEED;
	_color.set(1.0f,1.0f,1.0f,1.0f);
}

TitleScreen::~TitleScreen()
{
	BLIT->clear_cache();
}

bool TitleScreen::init(int start_level)
{
	_font_size=60;
	create_font();
	_time_since_last_frame=(float)timeGetTime();
	_exit_code=0;

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Buttons

	if (start_level>1)
	{
		Button button;
		button._text="Continue";
		button._fade_inc=0.0f;
		button._pos.set(512.0f,400.0f,0.4f);
		button._color.set(0.5f,0.0f,0.0f,0.0f);

		_buttons.push_back(button);
	}

	Button button;
	button._text="Start";
	button._fade_inc=0.0f;
	button._pos.set(512.0f,450.0f,0.4f);
	button._color.set(0.5f,0.0f,0.0f,0.0f);

	_buttons.push_back(button);

	button._text="Options";
	button._fade_inc=0.0f;
	button._pos.set(512.0f,500.0f,0.4f);
	button._color.set(0.5f,0.0f,0.0f,0.0f);

	_buttons.push_back(button);

	button._text="Exit";
	button._fade_inc=0.0f;
	button._pos.set(512.0f,550.0f,0.4f);
	button._color.set(0.5f,0.0f,0.0f,0.0f);

	_buttons.push_back(button);

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Options
	refresh_options(0.0f,0.0f);
	_options_enable=false;
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Cursor
	_cursor=new MouseCursor("mouse_cursor");
	_cursor->set_hotspot(16.0f,16.0f);
	_cursor->set_scale(1.0f);
	_cursor->set_color(0.5f,0.5f,0.0f,1.0f);
	_cursor->_pos.z=0.05f;

	return true;
}

void TitleScreen::refresh_options(float alpha,const float fade_inc)
{	
	_options.clear();

	Button button;
	button._text=(g_colorblind)?("Colorblind Mode: Enabled"):("Colorblind Mode: Disabled");
	button._fade_inc=fade_inc;
	button._pos.set(512.0f,400.0f,0.4f);
	button._color.set(0.5f,0.0f,0.0f,alpha);
	_options.push_back(button);

	button._text=(g_tutorial)?("Tutorial: Enabled"):("Tutorial: Disabled");
	button._fade_inc=fade_inc;
	button._pos.set(512.0f,450.0f,0.4f);
	button._color.set(0.5f,0.0f,0.0f,alpha);
	_options.push_back(button);

	button._text=(is_music_enabled())?("Music: Enabled"):("Music: Disabled");
	button._fade_inc=fade_inc;
	button._pos.set(512.0f,500.0f,0.4f);
	button._color.set(0.5f,0.0f,0.0f,alpha);
	_options.push_back(button);

	button._text=(g_sound)?("Sound: Enabled"):("Sound: Disabled");
	button._fade_inc=fade_inc;
	button._pos.set(512.0f,550.0f,0.4f);
	button._color.set(0.5f,0.0f,0.0f,alpha);
	_options.push_back(button);

	button._text="Back";
	button._fade_inc=fade_inc;
	button._pos.set(512.0f,600.0f,0.4f);
	button._color.set(0.5f,0.0f,0.0f,alpha);
	_options.push_back(button);
}
	
void TitleScreen::render()
{
	GFX->clear(D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER,0.0f,0.0f,0.0f,0.0f,1.0f,0);
	GFX->set_2d_render();
	GFX->begin_scene();

	Vec2f r=GFX->get_resolution();

	GFX->set_camera(r.x*0.5f,r.y*0.5f,0.0f);

	int tex=TEXTURES->find_by_name("title");
	BLIT->clear();
	BLIT->add(tex,0.0f,0.0f,0.5f,r.x,r.y,1.0f,1.0f,1.0f,1.0f,0.0f,0.0f,1.0f,1.0f,D3DTEXF_POINT);
	BLIT->add(0.0f,0.0f,0.3f,r.x,r.y,_color);
	BLIT->render();
	BLIT->clear();
	auto it=_buttons.begin();
	while (it!=_buttons.end())
	{
		write_text(it->_pos.x,it->_pos.y,it->_color.r,it->_color.g,it->_color.b,it->_color.a,it->_text.c_str(),TEXT_CENTER|TEXT_SHADOW);
		++it;
	}
	it=_options.begin();
	while (it!=_options.end())
	{
		write_text(it->_pos.x,it->_pos.y,it->_color.r,it->_color.g,it->_color.b,it->_color.a,it->_text.c_str(),TEXT_CENTER|TEXT_SHADOW);
		++it;
	}
	GFX->set_renderstate(D3DRS_ZENABLE,0);
	_cursor->draw();
	BLIT->render();
	BLIT->clear();
	GFX->set_renderstate(D3DRS_ZENABLE,1);
	GFX->end_scene();
}

void TitleScreen::anim(float tt)
{
	_cursor->anim(tt);

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
			
			auto it=_buttons.begin();
			while (it!=_buttons.end())
			{
				it->_fade_inc=BUTTON_FADE_SPEED;
				++it;
			}
		}
	}

	std::string prev_selected_button=_selected_button;
	std::string prev_selected_option=_selected_option;

	_selected_button="";
	_selected_option="";

	auto it=_buttons.begin();
	while (it!=_buttons.end())
	{
		// Check if cursor is on top of this element and change the color
		Rect rect;
		rect.x1=it->_pos.x-100.0f;
		rect.y1=it->_pos.y;
		rect.x2=it->_pos.x+100.0f;
		rect.y2=it->_pos.y+50;
		if (rect.inside(Vec2f(_cursor->_pos.x,_cursor->_pos.y)))
		{
			it->_color.set(1.0f,1.0f,0.0f,it->_color.w);
			if (prev_selected_button!=it->_text)
			{
				if (!_options_enable) play_sound("hover",0.25f);
			}
			_selected_button=it->_text;
		}
		else
		{
			it->_color.set(0.5f,0.0f,0.0f,it->_color.w);
		}
		if (it->_fade_inc>0.0f)
		{
			it->_color.a+=it->_fade_inc*tt;
			if (it->_color.a>1.0f)
			{
				it->_color.a=1.0f;
				it->_fade_inc=0.0f;
			}
		}
		else if (it->_fade_inc<0.0f)
		{
			it->_color.a+=it->_fade_inc*tt;
			if (it->_color.a<0.0f)
			{
				it->_color.a=0.0f;
				it->_fade_inc=0.0f;
				if (_options_enable)
				{
					auto jt=_options.begin();
					while (jt!=_options.end())
					{
						jt->_fade_inc=BUTTON_FADE_SPEED;
						++jt;
					}
				}
				else
				{
					_fade_inc=FADE_SPEED;
				}
			}
		}
		++it;
	}

	auto jt=_options.begin();
	while (jt!=_options.end())
	{
		// Check if cursor is on top of this element and change the color
		Rect rect;
		rect.x1=jt->_pos.x-200.0f;
		rect.y1=jt->_pos.y;
		rect.x2=jt->_pos.x+200.0f;
		rect.y2=jt->_pos.y+50;
		if (rect.inside(Vec2f(_cursor->_pos.x,_cursor->_pos.y)))
		{
			jt->_color.set(1.0f,1.0f,0.0f,jt->_color.w);
			if (prev_selected_option!=jt->_text)
			{
				if (_options_enable) play_sound("hover",0.25f);
			}
			_selected_option=jt->_text;
		}
		else
		{
			jt->_color.set(0.5f,0.0f,0.0f,jt->_color.w);
		}
		if (jt->_fade_inc>0.0f)
		{
			jt->_color.a+=jt->_fade_inc*tt;
			if (jt->_color.a>1.0f)
			{
				jt->_color.a=1.0f;
				jt->_fade_inc=0.0f;
			}
		}
		else if (jt->_fade_inc<0.0f)
		{
			jt->_color.a+=jt->_fade_inc*tt;
			if (jt->_color.a<0.0f)
			{
				jt->_color.a=0.0f;
				jt->_fade_inc=0.0f;
				_options_enable=false;

				auto it=_buttons.begin();
				while (it!=_buttons.end())
				{
					it->_fade_inc=BUTTON_FADE_SPEED;
					++it;
				}
			}
		}
		++jt;
	}

	if (KEY->is_pressed(DIK_ESCAPE)) 
	{
		_exit_code=-1;
		auto it=_buttons.begin();
		while (it!=_buttons.end())
		{
			it->_fade_inc=-BUTTON_FADE_SPEED;
			++it;
		}
	}
}

void TitleScreen::mouse_callback(unsigned long msg,unsigned long wparam,unsigned long lparam)
{
	Screen::mouse_callback(msg,wparam,lparam);

	if (msg==WM_LBUTTONUP)
	{
		if (_options_enable)
		{
			if (_selected_option!="") play_sound("select",0.25f);

			if (_selected_option=="Colorblind Mode: Enabled") { g_colorblind=false; _selected_option="Colorblind Mode: Disabled"; }
			else if (_selected_option=="Colorblind Mode: Disabled") { g_colorblind=true; _selected_option="Colorblind Mode: Enabled"; }
			else if (_selected_option=="Tutorial: Enabled") { g_tutorial=false; _selected_option="Tutorial: Disabled"; }
			else if (_selected_option=="Tutorial: Disabled") { g_tutorial=true; _selected_option="Tutorial: Enabled"; }
			else if (_selected_option=="Music: Enabled") { set_music_enable(false); _selected_option="Music: Disabled"; }
			else if (_selected_option=="Music: Disabled") { set_music_enable(true); _selected_option="Music: Enabled"; }
			else if (_selected_option=="Sound: Enabled") { g_sound=false; _selected_option="Sound: Disabled"; }
			else if (_selected_option=="Sound: Disabled") { g_sound=true; _selected_option="Sound: Enabled"; }

			if (_selected_option=="Back")
			{
				auto it=_options.begin();
				while (it!=_options.end())
				{
					it->_fade_inc=-BUTTON_FADE_SPEED;
					++it;
				}
			}
			else
			{
				refresh_options(_options.begin()->_color.w,_options.begin()->_fade_inc);
				save_options();
			}
		}
		else
		{
			if (_selected_button!="")
			{
				play_sound("select",0.25f);

				if ((_selected_button=="Start") || (_selected_button=="Continue") || (_selected_button=="Options"))
				{
					auto it=_buttons.begin();
					while (it!=_buttons.end())
					{
						it->_fade_inc=-BUTTON_FADE_SPEED;
						++it;
					}
					if (_selected_button=="Options")
					{
						_options_enable=true;
					}
					else if (_selected_button=="Start") _exit_code=0;
					else _exit_code=1;
				}
				else if (_selected_button=="Exit")
				{
					auto it=_buttons.begin();
					while (it!=_buttons.end())
					{
						it->_fade_inc=-BUTTON_FADE_SPEED;
						++it;
					}
					_exit_code=-1;
				}
			}
		}
	}
}
