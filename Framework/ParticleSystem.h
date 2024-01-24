// ParticleSystem.h: interface for the ParticleSystem class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PARTICLESYSTEM_H__CEEBDC89_0356_49E9_A7C3_DA58FE97FF87__INCLUDED_)
#define AFX_PARTICLESYSTEM_H__CEEBDC89_0356_49E9_A7C3_DA58FE97FF87__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Drawable.h"
#include <d3dx9.h>
#include <list>

#define PSBUFFER	50

class ParticleSystem : public Drawable
{
protected:
	struct Particle
	{
		Vec3f			pos;
		Vec3f			speed;
		float			current_life;
		float			total_life;
	};
	typedef std::list<Particle*>	Particles;
	struct VBPart
	{
		float	x,y,z;
		float	size;
		DWORD	color;
	};
	struct ColorElem
	{
		float	t;
		Color	c;
	};
	typedef std::vector<ColorElem> ColorElems;
public:
	ParticleSystem();
	virtual ~ParticleSystem();

	virtual void	draw();
	virtual void	anim(float t);

	inline	void	set_life(float life) { _life=life; _total_life=1.0f/life; }
	inline	void	set_rate(float start,float end) { _rate_start=start; _rate_end=end; }
	inline	void	set_particle_life(float base,float variance) { _life_base=base; _life_variance=variance; }
	inline	void	set_particle_speed(float base,float variance) { _speed_base=base; _speed_variance=variance; }
	inline	void	set_particle_size(float start,float end) { _size_start=start; _size_end=end; }
	
	inline	void	set_particle_color(float t,float r,float g,float b,float a) { ColorElem elem; elem.t=t; elem.c.set(r,g,b,a); _color.push_back(elem); }

	inline	void	set_particle_color_start(float r,float g,float b,float a) { set_particle_color(0.0f,r,g,b,a); }
	inline	void	set_particle_color_end(float r,float g,float b,float a) { set_particle_color(1.0f,r,g,b,a); }

protected:
			void	fetch_color(float t,Color* c);

protected:
			D3DCOLOR		get_color(float life);
			float			get_size(float life);
			float			get_rate(float life);

	LPDIRECT3DVERTEXBUFFER9	create_vertex_buffer(long size);
	void*					lock_vertex_buffer(LPDIRECT3DVERTEXBUFFER9 buffer);
	void					unlock_vertex_buffer(LPDIRECT3DVERTEXBUFFER9 buffer);

public:
	float					_life;
protected:
	Particles				_particles;
	float					_rate_start;
	float					_rate_end;
	float					_total_life;
	float					_life_base;
	float					_life_variance;
	float					_speed_base;
	float					_speed_variance;
	LPDIRECT3DVERTEXBUFFER9	_buffer;
	long					_allocated;
	float					_size_start;
	float					_size_end;
	ColorElems				_color;
};

#endif // !defined(AFX_PARTICLESYSTEM_H__CEEBDC89_0356_49E9_A7C3_DA58FE97FF87__INCLUDED_)
