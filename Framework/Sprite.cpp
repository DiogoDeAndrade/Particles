// Sprite.cpp: implementation of the Sprite class.
//
//////////////////////////////////////////////////////////////////////

#include "Sprite.h"
#include "BlitManager.h"
#include "TextureManager.h"
#include "Gfx.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Sprite::Sprite()
{
	_angle=0.0f;
	_animation_last_frame_time=(float)timeGetTime();
	_u1=_v1=0.0f;
	_u2=_v2=1.0f;
	_filter=D3DTEXF_LINEAR;
	_flip_h=false;
	_flip_v=false;
	set_color(1.0f,1.0f,1.0f,1.0f);
}

Sprite::Sprite(float x,float y,float z)
{
	_pos.x=x;
	_pos.y=y;
	_pos.z=z;
	_scale_x=_scale_y=1.0f;
	_angle=0.0f;
	_animation_last_frame_time=(float)timeGetTime();
	_u1=_v1=0.0f;
	_u2=_v2=1.0f;
	_filter=D3DTEXF_LINEAR;
	_flip_h=false;
	_flip_v=false;
	set_hotspot(0.0f,0.0f);
	set_color(1.0f,1.0f,1.0f,1.0f);
}

Sprite::~Sprite()
{
	_animation_frames.clear();
}

void Sprite::anim(float t)
{
	if (timeGetTime()-_animation_last_frame_time>_animation_rate)
	{
		if (_animation_frames.size()>0)
		{
			_current_image++;
			if ((size_t)_current_image>=_animation_frames.size())
			{
				if (_animation_loop) _current_image=0;
				else _current_image--;
			}
		}
		_animation_last_frame_time=(float)timeGetTime();
	}

	_last_frame_time=(float)timeGetTime();
}

void Sprite::draw()
{
	if (!_draw) return;
	if ((_scale_x==0.0f) || (_scale_y==0.0f)) return;

	long tex;
	if (_animation_frames.size()>0) 
	{
		tex=_animation_frames[_current_image].img;
		_u1=_animation_frames[_current_image].uv.x1;
		_u2=_animation_frames[_current_image].uv.x2;
		_v1=_animation_frames[_current_image].uv.y1;
		_v2=_animation_frames[_current_image].uv.y2;
	}
	else	tex=_current_image;

	if (_current_image==-1)
	{
		Vec3f	color=GFX->get_color_modulation();
		color.x*=_color.x;
		color.y*=_color.y;
		color.z*=_color.z;

		BLIT->add(_pos.x,_pos.y,_pos.z,_size.x,_size.y,Color(color.x,color.y,color.z,_color.a));
	}
	else
	{
		float	sx=TEXTURES->get_size_x(tex)*_scale_x*(_u2-_u1);
		float	sy=TEXTURES->get_size_y(tex)*_scale_y*(_v2-_v1);
		Vec3f	color=GFX->get_color_modulation();
		color.x*=_color.x;
		color.y*=_color.y;
		color.z*=_color.z;
		float	u1=(_flip_h)?(_u2):(_u1);
		float	v1=(_flip_v)?(_v2):(_v1);
		float	u2=(_flip_h)?(_u1):(_u2);
		float	v2=(_flip_v)?(_v1):(_v2);
		if (_angle==0.0f)
		{
			BLIT->add(tex,_pos.x-_hotspot.x*_scale_x,_pos.y-_hotspot.y*_scale_y,_pos.z,sx,sy,color.x,color.y,color.z,_color.w,
				u1,v1,u2,v2,_filter);
		}
		else
		{
			BLIT->add(tex,_pos,_hotspot,_angle,sx,sy,color.x,color.y,color.z,_color.w,
				u1,v1,u2,v2,_filter);
		}
	}
}

void Sprite::clear_animation() 
{ 
	if (_animation_frames.size()==0) return;

	_u1=_animation_frames[_current_image].uv.x1;
	_u2=_animation_frames[_current_image].uv.x2;
	_v1=_animation_frames[_current_image].uv.y1;
	_v2=_animation_frames[_current_image].uv.y2;
	_current_image=_animation_frames[_current_image].img;
	_animation_frames.clear(); 
}
