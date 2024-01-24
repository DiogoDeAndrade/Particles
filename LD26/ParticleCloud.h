#pragma once

#include "Drawable.h"
#include "Mesh.h"
#include "MathLib.h"

class ParticleCloud :
	public Drawable
{
protected:
	struct Particle
	{
		Vec3f			pos;
		Vec2f			speed;
		float			phase;
		unsigned char	type;
	};
	typedef std::list<Particle>		Particles;
	struct Attractor
	{
		Vec3f			pos;
		unsigned char	type;
		float			strength;
		float			radius;
		float			angle;
		float			range;
	};
	typedef std::vector<Attractor>	Attractors;
public:
	ParticleCloud(int particle_count);
	ParticleCloud(int particle_count,const int sx,const int sy,const int radius,const bool spawn_color[3]);
	virtual ~ParticleCloud(void);

	virtual	void	draw();
	virtual void	anim(float t);

			int				new_attractor(unsigned char type,float radius,float strength);
			void			set_attractor_pos(const size_t idx,const Vec3f& p);
			void			set_attractor_type(const size_t idx,const unsigned char t);
			void			set_attractor_range(const size_t idx,const float r);
			unsigned char	get_attractor_type(const size_t idx);
			float			get_alive_particle_percentage();
			float			get_dead_particle_percentage();
			float			get_saved_particles_percentage();
			UINT			get_alive_particles();

protected:
	Mesh<VertexPSC>*	_mesh;
	Particles			_particles;
	Attractors			_attractors;
	UINT				_current_particles;
	UINT				_starting_particles;
	UINT				_saved_particles;
};

