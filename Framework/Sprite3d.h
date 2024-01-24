#pragma once
#include "object3d.h"

class Sprite3d :
	public Object3d
{
public:
	Sprite3d(void);
	virtual ~Sprite3d(void);

	inline	void		set_image(long img) { _current_image=img; }
	inline	long		get_image() { return _current_image; }

	inline	void		set_size(const Vec2f& s) { _size=s; }
	inline	Vec2f		get_size() { return _size; }

	inline	void		set_color(float r,float g,float b,float a) { _color.set(r,g,b,a); }
	inline	Color		get_color() { return _color; }

	inline	void		set_ground(const bool b) { _ground=b; }

	inline	void		set_uv(const float u1,const float v1,const float u2,const float v2) { _uv.set(u1,v1,u2,v2); }

	virtual	void		draw();

protected:
	long	_current_image;
	Color	_color;
	Vec2f	_size;
	Rect	_uv;
	bool	_ground;
};
