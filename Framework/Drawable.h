// Drawable.h: interface for the Drawable class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DRAWABLE_H__BEB0C095_88E8_44DA_9F2A_1CA800247BED__INCLUDED_)
#define AFX_DRAWABLE_H__BEB0C095_88E8_44DA_9F2A_1CA800247BED__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <vector>
#include "Defs.h"

typedef enum { BLEND_NONE, BLEND_ALPHA, BLEND_ADD, BLEND_ADDALPHA } BlendMode;

class Drawable  
{
public:
	Drawable();
	virtual ~Drawable();

	virtual	void	draw() = 0;
	virtual void	anim(float t) = 0;

	// Query
	inline	void		set_pos(float x,float y,float z) { _pos.x=x; _pos.y=y; _pos.z=z; }
	inline	Vec3f		get_pos() { return _pos; }
	inline	float		get_x_pos() { return _pos.x; }
	inline	float		get_y_pos() { return _pos.y; }
	inline	float		get_z_pos() { return _pos.z; }

	// Settings
	inline	void	set_image(long img) { _current_image=img; }
	inline	long	get_image() { return _current_image; }

	inline	void		set_color(float r,float g,float b,float a) { _color.set(r,g,b,a); }
	inline	Color		get_color() { return _color; }

	inline	void	set_priority(float priority) { _priority=priority; }
	inline	float	get_priority() { return _priority; }

	inline	void	set_blend_mode(BlendMode bm) { _blend_mode=bm; }

	inline	void	set_draw(bool b) { _draw=b; }
	inline	bool	get_draw() { return _draw; }

	inline	void	set_scale(float s) { _scale_x=s; _scale_y=s; }
	inline	void	set_scale(float sx,float sy) { _scale_x=sx; _scale_y=sy; }
	inline	float	get_scale_x() { return _scale_x; }
	inline	float	get_scale_y() { return _scale_y; }
	inline	void	set_hotspot(float hx,float hy) { _hotspot.x=hx; _hotspot.y=hy; }

	inline	float	get_x_hotspot() { return _hotspot.x; }
	inline	float	get_y_hotspot() { return _hotspot.y; }

	inline	void	set_size(const Vec2f& s) { _size=s; }

public:
	Color		_color;
	Vec3f		_pos;
	Vec2f		_hotspot;
protected:
	float		_scale_x,_scale_y;
	float		_priority;
	bool		_draw;
	long		_current_image;
	BlendMode	_blend_mode;
	float		_last_frame_time;
	Vec2f		_size;
};

typedef std::vector<Drawable*>	Drawables;

#endif // !defined(AFX_DRAWABLE_H__BEB0C095_88E8_44DA_9F2A_1CA800247BED__INCLUDED_)
