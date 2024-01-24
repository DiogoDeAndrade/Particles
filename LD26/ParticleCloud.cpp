#include "ParticleCloud.h"
#include "BlitManager.h"
#include "Utils.h"

#include "GameScreen.h"
#include "Options.h"

ParticleCloud::ParticleCloud(int max_particle_count)
{
	for (int i=0; i<max_particle_count; i++)
	{
		Particle	p;
		p.pos.set((float)(rand()%1024),(float)(rand()%768),0.5f);
		p.speed.set(0.0f,0.0f);
		p.phase=deg_to_rad((float)(rand()%360));
		p.type=(rand()%3)+1;
		_particles.push_back(p);
	}

	_current_particles=max_particle_count;
	_starting_particles=max_particle_count;
	_saved_particles=0;

	_mesh=new Mesh<VertexPSC>(true);
	_mesh->alloc_vert(max_particle_count);
	_mesh->set_primitive(D3DPT_POINTLIST);
}

ParticleCloud::ParticleCloud(int max_particle_count,const int sx,const int sy,const int radius,const bool spawn_color[3])
{
	for (int i=0; i<max_particle_count; i++)
	{
		float ang=deg_to_rad((float)(rand()%360));
		float r=((float)(rand()%radius));

		Particle	p;
		p.pos.set(sx+cosf(ang)*r,sy+sinf(ang)*r,0.5f);
		p.speed.set(0.0f,0.0f);
		p.phase=ang;
		p.type=(rand()%3);
		while (!spawn_color[p.type]) p.type=(rand()%3);
		p.type++;
		_particles.push_back(p);
	}

	_current_particles=max_particle_count;
	_starting_particles=max_particle_count;
	_saved_particles=0;

	_mesh=new Mesh<VertexPSC>(true);
	_mesh->alloc_vert(max_particle_count);
	_mesh->set_primitive(D3DPT_POINTLIST);
}

ParticleCloud::~ParticleCloud(void)
{
	if (_mesh)
	{
		delete _mesh;
		_mesh=NULL;
	}
}

void ParticleCloud::draw()
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
	GFX->set_renderstate(D3DRS_POINTSPRITEENABLE,TRUE);
	GFX->set_renderstate(D3DRS_POINTSCALEENABLE,TRUE);
	GFX->set_renderstate(D3DRS_POINTSIZE_MIN,FtoDW(0.00f));
	GFX->set_renderstate(D3DRS_POINTSCALE_A,FtoDW(1.0f));
	GFX->set_renderstate(D3DRS_POINTSCALE_B,FtoDW(0.0f));
	GFX->set_renderstate(D3DRS_POINTSCALE_C,FtoDW(0.0f));
	GFX->set_texture_stage_state(0,D3DTSS_COLOROP,D3DTOP_SELECTARG1);
	GFX->set_texture_stage_state(0,D3DTSS_ALPHAOP,D3DTOP_SELECTARG1);
	GFX->set_texture_stage_state(0,D3DTSS_COLORARG1,D3DTA_DIFFUSE);
	GFX->set_texture_stage_state(0,D3DTSS_ALPHAARG1,D3DTA_DIFFUSE);
	GFX->set_texture_stage_state(1,D3DTSS_COLOROP,D3DTOP_DISABLE);
	GFX->set_texture_stage_state(1,D3DTSS_ALPHAOP,D3DTOP_DISABLE);

	_mesh->draw();

	GFX->set_renderstate(D3DRS_POINTSPRITEENABLE,FALSE);
	GFX->set_renderstate(D3DRS_POINTSCALEENABLE,FALSE);
	GFX->set_renderstate(D3DRS_POINTSIZE_MIN,FtoDW(0.00f));
	GFX->set_renderstate(D3DRS_POINTSCALE_A,FtoDW(1.0f));
	GFX->set_renderstate(D3DRS_POINTSCALE_B,FtoDW(0.0f));
	GFX->set_renderstate(D3DRS_POINTSCALE_C,FtoDW(0.0f));
}

void ParticleCloud::anim(float t)
{
	if (_particles.empty()) return;

	Vec2f		res=GFX->get_resolution();
	float		s=5.0f/res.x;
	VertexPSC*	vp=_mesh->get_write_pointer_v();
	auto		it=_particles.begin();
	
	_current_particles=0;

	auto	jt=_attractors.begin();
	while (jt!=_attractors.end())
	{
		jt->angle+=0.01f*t;

		++jt;
	}
	while (it!=_particles.end())
	{
		// Move particles
		Vec2f	force(0.0f,0.0f);
		auto	jt=_attractors.begin();
		while (jt!=_attractors.end())
		{
			if ((jt->type==it->type) || (jt->type==4))
			{
				Vec2f ap;

				ap.set(jt->pos.x+cosf(jt->angle+it->phase)*jt->radius,jt->pos.y+sinf(jt->angle+it->phase)*jt->radius);

				Vec2f delta(it->pos.x,it->pos.y);
				delta=ap-delta;
				float dist=delta.normalize();
				if (dist<jt->range)
				{
					if (dist<50.0f)
					{
						if (dist>5.0f)
						{
							ap.set(jt->pos.x+cosf(jt->angle+it->phase)*jt->radius,jt->pos.y+sinf(jt->angle+it->phase)*jt->radius);
							delta=ap-Vec2f(it->pos.x,it->pos.y);
							dist=delta.normalize();
							force=force+jt->strength*(delta/sqrtf(dist));
						}
					}
					else force=force+jt->strength*(delta/sqrtf(dist));
				}
			}

			++jt;
		}

		it->speed=it->speed*0.75f+force;
		it->pos.x+=it->speed.x*t;
		it->pos.y+=it->speed.y*t;

		// Check if there was a collision
		bool add_to_mesh=true;
		int col=GameScreen::OBJ->get_color(it->pos.x,it->pos.y,true);
		if (col!=-1)
		{
			switch (col)
			{
				case 0xA0: _saved_particles++; GameScreen::OBJ->new_explosion(it->pos,get_particle_color(it->type),get_particle_color(it->type)&0x00FFFFFF); it=_particles.erase(it); play_sound("yeah",0.5f,50); continue; break;
				case 0: GameScreen::OBJ->new_explosion(it->pos,0xFFFFFF00,0xFFFF0000); it=_particles.erase(it); play_sound("boom",0.5f,50); continue; break;
				default:
					if (col!=it->type) { GameScreen::OBJ->new_explosion(it->pos,0xFFFFFF00,0xFFFF0000); it=_particles.erase(it); play_sound("boom",0.5f,50); continue; }
					break;
			}
		}

		// Update the mesh
		vp->x=it->pos.x/res.x;
		vp->y=it->pos.y/res.y;
		vp->z=it->pos.z;
		vp->s=s;
		
		vp->color=get_particle_color(it->type);

		// Step next
		vp++;
		_current_particles++;

		++it;
	}
	_mesh->set_render_verts(_current_particles);
	_mesh->release_write_pointer_v();
}

int	ParticleCloud::new_attractor(unsigned char type,float radius,float strength)
{
	Attractor	p;
	p.pos.set(0.0f,0.0f,0.5f);
	p.radius=radius;
	p.type=type;
	p.radius=radius;
	p.strength=strength;
	p.angle=0.0f;
	p.range=FLT_MAX;
	
	_attractors.push_back(p);

	return _attractors.size()-1;
}

void ParticleCloud::set_attractor_pos(const size_t idx,const Vec3f& p)
{
	_attractors[idx].pos=p;
}

void ParticleCloud::set_attractor_type(const size_t idx,const unsigned char t)
{
	_attractors[idx].type=t;
}

void ParticleCloud::set_attractor_range(const size_t idx,const float r)
{
	_attractors[idx].range=r;
}

unsigned char ParticleCloud::get_attractor_type(const size_t idx)
{
	return _attractors[idx].type;
}

float ParticleCloud::get_alive_particle_percentage()
{
	return ((float)_current_particles)/((float)_starting_particles);
}

UINT ParticleCloud::get_alive_particles()
{
	return _current_particles;
}

float ParticleCloud::get_dead_particle_percentage()
{
	UINT dead_particles=_starting_particles-(_current_particles+_saved_particles);

	return ((float)dead_particles)/((float)_starting_particles);
}

float ParticleCloud::get_saved_particles_percentage()
{
	return ((float)_saved_particles)/((float)_starting_particles);
}

