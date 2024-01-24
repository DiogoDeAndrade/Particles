#include "Explosions.h"
#include "BlitManager.h"
#include "Utils.h"

#define MAX_LINE_LIFE	1000.0f

Explosions::Explosions()
{
	_mesh=new Mesh<VertexPC>(true);
	_mesh->alloc_vert(10000);
	_mesh->set_primitive(D3DPT_LINELIST);
	_mesh->set_render_verts(0);
}

Explosions::~Explosions(void)
{
	if (_mesh)
	{
		delete _mesh;
		_mesh=NULL;
	}
}

void Explosions::draw()
{
	if (!_draw) return;
	if (!_mesh) return;
	if (_particles.empty()) return;

	GFX->set_world_transformation(0,0.0f,0.0f,0.0f,Quaternion(0.0f,0.0f,0.0f,1.0f));

	GFX->set_renderstate(D3DRS_COLORVERTEX,TRUE);
	GFX->set_renderstate(D3DRS_AMBIENTMATERIALSOURCE,D3DMCS_COLOR1);
	GFX->set_renderstate(D3DRS_DIFFUSEMATERIALSOURCE,D3DMCS_COLOR1);
	GFX->set_renderstate(D3DRS_EMISSIVEMATERIALSOURCE,D3DMCS_COLOR1);
	GFX->set_renderstate(D3DRS_ALPHABLENDENABLE,TRUE);
	GFX->set_renderstate(D3DRS_SRCBLEND,D3DBLEND_SRCALPHA);
	GFX->set_renderstate(D3DRS_DESTBLEND,D3DBLEND_INVSRCALPHA);
	GFX->set_texture_stage_state(0,D3DTSS_COLOROP,D3DTOP_SELECTARG1);
	GFX->set_texture_stage_state(0,D3DTSS_ALPHAOP,D3DTOP_SELECTARG1);
	GFX->set_texture_stage_state(0,D3DTSS_COLORARG1,D3DTA_DIFFUSE);
	GFX->set_texture_stage_state(0,D3DTSS_ALPHAARG1,D3DTA_DIFFUSE);
	GFX->set_texture_stage_state(1,D3DTSS_COLOROP,D3DTOP_DISABLE);
	GFX->set_texture_stage_state(1,D3DTSS_ALPHAOP,D3DTOP_DISABLE);

	_mesh->draw();
}

void Explosions::anim(float t)
{
	if (_particles.empty()) return;

	VertexPC*	vp=_mesh->get_write_pointer_v();
	auto		it=_particles.begin();
	Vec2f		res=GFX->get_resolution();
	UINT		n=0;

	while (it!=_particles.end())
	{
		it->life-=t;
		if (it->life<0.0f)
		{
			it=_particles.erase(it);
			continue;
		}
		n++;

		float l=(it->life/MAX_LINE_LIFE);

		unsigned long c=((unsigned long)floorf(l*255))<<24;

		// Move particles
		it->old_pos=it->pos;
		it->pos=it->pos+it->speed*t;

		// Update the mesh
		vp->x=it->pos.x/res.x;
		vp->y=it->pos.y/res.y;
		vp->z=it->pos.z;
		vp->color=(it->color1&0x00FFFFFF)+c;

		vp++;

		vp->x=it->old_pos.x/res.x;
		vp->y=it->old_pos.y/res.y;
		vp->z=it->old_pos.z;
		vp->color=(it->color2&0x00FFFFFF)+c;

		vp++;

		++it;
	}
	_mesh->set_render_verts(n*2);
	_mesh->release_write_pointer_v();
}

void Explosions::new_explosion(const Vec3f& pos,const unsigned long color1,const unsigned long color2)
{
	for (int i=0; i<20; i++)
	{
		Particle p;
		p.old_pos=p.pos=Vec3f(pos.x,pos.y,pos.z-0.2f);
		p.life=MAX_LINE_LIFE;
		p.color1=color1;
		p.color2=color2;
		p.speed.set(((float)(rand()%200)-100.0f)/100.0f,
					((float)(rand()%200)-100.0f)/100.0f,
					0.0f);
		p.speed.normalize(); p.speed=p.speed*((float)(rand()%100)+100.0f)/1000.0f;

		_particles.push_back(p);
	}
}
