#pragma once

#include "Drawable.h"
#include "Mesh.h"
#include "MathLib.h"

class Explosions :
	public Drawable
{
protected:
	struct Particle
	{
		Vec3f			pos;
		Vec3f			old_pos;
		unsigned long	color1;
		unsigned long	color2;
		Vec3f			speed;
		float			life;
	};
	typedef std::list<Particle>		Particles;
public:
	Explosions();
	virtual ~Explosions(void);

	virtual	void	draw();
	virtual void	anim(float t);

			void			new_explosion(const Vec3f& pos,const unsigned long color1,const unsigned long color2);

protected:
	Mesh<VertexPC>*	_mesh;
	Particles		_particles;
};

