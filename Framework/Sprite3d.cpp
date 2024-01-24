#include "Sprite3d.h"
#include "Gfx.h"
#include "BlitManager3d.h"

Sprite3d::Sprite3d(void)
{
	_color.set(1.0f,1.0f,1.0f,1.0f);
	_size.set(10.0f,10.0f);
	_uv.set(0.0f,0.0f,1.0f,1.0f);
	_ground=false;
}

Sprite3d::~Sprite3d(void)
{
}

void Sprite3d::draw()
{
	if (!_draw) return;
	if ((_size.x==0.0f) || (_size.y==0.0f)) return;

	Vec3f	color=GFX->get_color_modulation();
	color.x*=_color.x;
	color.y*=_color.y;
	color.z*=_color.z;
	Color	c(color.x,color.y,color.z,_color.a);

	if (!_ground)
		BLIT3D->add(_current_image,_pos,_size,c,_uv);
	else
		BLIT3D->add_ground(_current_image,_pos,_size,c,_uv);
}
