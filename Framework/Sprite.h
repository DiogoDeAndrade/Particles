// Sprite.h: interface for the Sprite class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SPRITE_H__46CE9DE0_C822_4B18_8018_C1EC8E3F5214__INCLUDED_)
#define AFX_SPRITE_H__46CE9DE0_C822_4B18_8018_C1EC8E3F5214__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <vector>
#include <d3d9.h>
#include <d3dx9.h>
#include "Drawable.h"

class Sprite : public Drawable
{
protected:
	struct AnimFrame
	{
		long img;
		Rect uv;
	};
	typedef std::vector<AnimFrame>	Animation;
public:
	Sprite();
	Sprite(float x,float y,float z);
	virtual ~Sprite();

	inline	void	set_pos(float x,float y,float z) { _pos.x=x; _pos.y=y; _pos.z=z; }
	virtual void	anim(float t);
	virtual	void	draw();
	
	inline	void	set_angle(float ang) { _angle=(ang>360)?(ang-360):(ang); }
	inline	float	get_angle() { return _angle; }
	inline	void	set_filter(const D3DTEXTUREFILTERTYPE filter) { _filter=filter; }
	// Animation commands
	inline	void	add_animation_frame(long i) { AnimFrame frame; frame.img=i; frame.uv.set(0.0f,0.0f,1.0f,1.0f); _animation_frames.push_back(frame); _current_image=0; }
	inline	void	add_animation_frame(long i,Rect r) { AnimFrame frame; frame.img=i; frame.uv=r; _animation_frames.push_back(frame); _current_image=0; }
			void	clear_animation();
	inline	void	set_animation_loop(bool b) { _animation_loop=b; _current_image=0; }
	inline	void	set_animation_rate(float rate) { _animation_rate=rate; _current_image=0; }
	inline	void	set_uv(float u1,float v1,float u2,float v2) { _u1=u1; _v1=v1; _u2=u2; _v2=v2;}
	inline	void	set_flip(bool h,bool v) { _flip_h=h; _flip_v=v; }
	inline	bool	get_flip_horizontal() { return _flip_h; }
	inline	bool	get_flip_vertical() { return _flip_v; }

protected:
	float					_animation_last_frame_time;
	Animation				_animation_frames;
	float					_animation_rate;
	bool					_animation_loop;
	float					_angle;
	float					_u1,_v1,_u2,_v2;
	D3DTEXTUREFILTERTYPE	_filter;
	bool					_flip_h,_flip_v;
};

#endif // !defined(AFX_SPRITE_H__46CE9DE0_C822_4B18_8018_C1EC8E3F5214__INCLUDED_)
