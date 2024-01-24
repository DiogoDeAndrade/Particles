// ParticleSystem3d.cpp: implementation of the ParticleSystem3d class.
//
//////////////////////////////////////////////////////////////////////

#include "ParticleSystem3d.h"
#include "TextureManager.h"
#include "BlitManager.h"
#include "Gfx.h"
#include "Utils.h"
#include "Screen.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

ParticleSystem3d::ParticleSystem3d()
{
	_rate_start=_rate_end=0.0f;
	_life_base=2000.0f;
	_life_variance=0.0f;
	_speed_base=0.0f;
	_speed_variance=0.0f;
	_buffer=NULL;
	_allocated=0;
	set_particle_size(1.0f,1.0f);
	set_particle_color_start(1.0f,1.0f,1.0f,1.0f);
	set_particle_color_end(1.0f,1.0f,1.0f,1.0f);
	_life=999999.0f;
	_last_frame_time=(float)timeGetTime();
	_current_image=0;
	_blend_mode=BLEND_ALPHA;
	_emitter=EMITTER3D_OMNI;
	_last_frac_part_count=0.0f;
}

ParticleSystem3d::~ParticleSystem3d()
{
	_particles.clear();
	if (_buffer) _buffer->Release();
}

void ParticleSystem3d::draw()
{
	if (_particles.size()==0) return;

	if (_link)
	{
		_pos=_link->_pos;
		_ori=_link->_ori;
	}

	BLIT->render();
	BLIT->clear();

	if (!_buffer)
	{
		// Create vertex buffer for the particle
		_buffer=create_vertex_buffer((long)(_particles.size()+PSBUFFER));
		_allocated=(long)_particles.size()+PSBUFFER;
	}
	else
	{
		// Check if they have space
		if ((size_t)_allocated<_particles.size())
		{
			// Resize buffer
			_buffer->Release();
			_buffer=create_vertex_buffer(_allocated*2);
			_allocated=_allocated*2;
		}
	}

	// Fill buffer
	VBPart* ptr=(VBPart*)lock_vertex_buffer(_buffer);
	if (!ptr) return;

	Particles::iterator par;
	for (par=_particles.begin(); par!=_particles.end(); ++par)
	{
		Particle* tmp=*par;
		ptr->x=(*par)->pos.x;
		ptr->y=(*par)->pos.y;
		ptr->z=(*par)->pos.z;
		float life=((*par)->current_life)*((*par)->total_life);
		ptr->color=get_color(life);
		ptr->size=get_size(life);
		ptr++;
	}
	unlock_vertex_buffer(_buffer);

	// Render buffer
	LPDIRECT3DDEVICE9 device=GFX->get_device();

	GFX->set_world_transformation(0,0.0f,0.0f,0.0f,Quaternion(0,0,0,1));
	GFX->set_vertex_shader(NULL);
	GFX->set_vertex_declaration(NULL);
	GFX->set_fvf(D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_PSIZE);
	GFX->set_renderstate(D3DRS_COLORVERTEX,TRUE);
	GFX->set_renderstate(D3DRS_AMBIENTMATERIALSOURCE,D3DMCS_COLOR1);
	GFX->set_renderstate(D3DRS_DIFFUSEMATERIALSOURCE,D3DMCS_COLOR1);
	GFX->set_renderstate(D3DRS_EMISSIVEMATERIALSOURCE,D3DMCS_COLOR1);
	GFX->set_renderstate(D3DRS_LIGHTING,FALSE);
	GFX->set_renderstate(D3DRS_CULLMODE,D3DCULL_NONE);
	GFX->set_renderstate(D3DRS_POINTSPRITEENABLE,TRUE);
	GFX->set_renderstate(D3DRS_POINTSCALEENABLE,TRUE);
	GFX->set_renderstate(D3DRS_POINTSIZE_MIN,FtoDW(0.00f));
	GFX->set_renderstate(D3DRS_POINTSCALE_A,FtoDW(0.0f));
	GFX->set_renderstate(D3DRS_POINTSCALE_B,FtoDW(0.0f));
	GFX->set_renderstate(D3DRS_POINTSCALE_C,FtoDW(1.0f));
	GFX->set_renderstate(D3DRS_ZWRITEENABLE,FALSE);
	if (_blend_mode==BLEND_NONE)
	{
		GFX->set_renderstate(D3DRS_ALPHABLENDENABLE,FALSE);
		GFX->set_renderstate(D3DRS_SRCBLEND,D3DBLEND_ONE);
		GFX->set_renderstate(D3DRS_DESTBLEND,D3DBLEND_ZERO);
	}
	else if (_blend_mode==BLEND_ALPHA)
	{
		GFX->set_renderstate(D3DRS_ALPHABLENDENABLE,TRUE);
		GFX->set_renderstate(D3DRS_SRCBLEND,D3DBLEND_SRCALPHA);
		GFX->set_renderstate(D3DRS_DESTBLEND,D3DBLEND_INVSRCALPHA);
	}
	else if (_blend_mode==BLEND_ADDALPHA )
	{
		GFX->set_renderstate(D3DRS_ALPHABLENDENABLE,TRUE);
		GFX->set_renderstate(D3DRS_SRCBLEND,D3DBLEND_SRCALPHA);
		GFX->set_renderstate(D3DRS_DESTBLEND,D3DBLEND_ONE);
	}
	else if (_blend_mode==BLEND_ADD)
	{
		GFX->set_renderstate(D3DRS_ALPHABLENDENABLE,TRUE);
		GFX->set_renderstate(D3DRS_SRCBLEND,D3DBLEND_ONE);
		GFX->set_renderstate(D3DRS_DESTBLEND,D3DBLEND_ONE);
	}
	GFX->set_texture_stage_state(0,D3DTSS_COLOROP,D3DTOP_MODULATE);
	GFX->set_texture_stage_state(0,D3DTSS_ALPHAOP,D3DTOP_MODULATE);
	GFX->set_texture_stage_state(0,D3DTSS_COLORARG1,D3DTA_TEXTURE);
	GFX->set_texture_stage_state(0,D3DTSS_COLORARG2,D3DTA_DIFFUSE);
	GFX->set_texture_stage_state(0,D3DTSS_ALPHAARG1,D3DTA_TEXTURE);
	GFX->set_texture_stage_state(0,D3DTSS_ALPHAARG2,D3DTA_DIFFUSE);
	GFX->set_texture_stage_state(1,D3DTSS_COLOROP,D3DTOP_DISABLE);
	GFX->set_texture_stage_state(1,D3DTSS_ALPHAOP,D3DTOP_DISABLE);

	// Set the texture
	GFX->set_texture(0,TEXTURES->get_texture(_current_image));
	// Draw!
	GFX->set_stream_source(0,_buffer,0,sizeof(VBPart));
	HRESULT hRet=device->DrawPrimitive(D3DPT_POINTLIST,0,long(_particles.size()));
	if (hRet!=D3D_OK)
	{
		ERROR_MSG("Can't draw indexed primitive!");
	}

	GFX->set_stream_source(0,NULL,0,0);
	GFX->set_texture(0,NULL);
	GFX->set_renderstate(D3DRS_ZWRITEENABLE,TRUE);
}

void ParticleSystem3d::anim(float t)
{
	// Spawn particles
	if (_life!=FLT_MAX) _life=_life-t;

	long	n_part;

	float tmp_part;
	if (_life!=FLT_MAX)
	{
		tmp_part=get_rate(_life*_total_life)*t/1000.0f;
	}
	else
	{
		tmp_part=get_rate(1.0f)*t/1000.0f;
	}
	n_part=(long)(tmp_part);
	if (_last_frac_part_count>1)
	{
		n_part+=(long)floorf(_last_frac_part_count);
		_last_frac_part_count=_last_frac_part_count-(long)floorf(_last_frac_part_count);
	}
	else _last_frac_part_count+=(tmp_part-n_part);

	float	px=_pos.x;
	float	py=_pos.y;
	float	pz=_pos.z;

	if (_life>0.0f)
	{
		if (_emitter==EMITTER3D_OMNI)
		{
			for (long i=0; i<n_part; i++)
			{
				Particle* tmp=new Particle;

				tmp->pos.set(px,py,pz);
				tmp->speed.set((float)(rand()%100-50),(float)(rand()%100-50),0.0f);
				tmp->speed.normalize();
				tmp->speed*=_speed_base+get_random_number(-_speed_variance,_speed_variance);
				tmp->current_life=_life_base+get_random_number(-_life_variance,_life_variance);
				tmp->total_life=1.0f/tmp->current_life;
				_particles.push_back(tmp);
			}
		}
		else if ((_emitter==EMITTER3D_CONE_GRAV) || (_emitter==EMITTER3D_CONE))
		{
			for (long i=0; i<n_part; i++)
			{
				Particle* tmp=new Particle;

				tmp->pos.set(px,py,pz);
				tmp->speed.set((float)(rand()%100-50),100.0f,(float)(rand()%100-50));
				tmp->speed.normalize();
				tmp->speed*=_speed_base+get_random_number(-_speed_variance,_speed_variance);
				tmp->current_life=_life_base+get_random_number(-_life_variance,_life_variance);
				tmp->total_life=1.0f/tmp->current_life;
				_particles.push_back(tmp);
			}
		}
		if (_emitter==EMITTER3D_CIRCLE_XZ)
		{
			for (long i=0; i<n_part; i++)
			{
				Particle* tmp=new Particle;

				Vec3f delta=Vec3f(rand()%512-256.0f,0,rand()%512-256.0f); 
				delta.normalize(); delta=delta*_emitter_param[0];

				tmp->pos.set(px+delta.x,py+delta.y,pz+delta.z);
				tmp->speed.set(0.0f,1.0f,0.0f);
				tmp->speed.normalize();
				tmp->speed*=_speed_base+get_random_number(-_speed_variance,_speed_variance);
				tmp->current_life=_life_base+get_random_number(-_life_variance,_life_variance);
				tmp->total_life=1.0f/tmp->current_life;
				_particles.push_back(tmp);
			}
		}

	}
	// Animate particles
	Particles::iterator par=_particles.begin();
	while (par!=_particles.end())
	{
		Particle *tmp=*par;
		tmp->pos=tmp->pos+tmp->speed*t;
		if (_emitter==EMITTER3D_CONE_GRAV)
		{
			tmp->speed.y-=t*0.00001f;
		}
		tmp->current_life-=t;
		if (tmp->current_life<0.0f) 
		{
			par=_particles.erase(par);
		}
		else
		{
			++par;
		}
	}
	_last_frame_time=(float)timeGetTime();
}

LPDIRECT3DVERTEXBUFFER9	ParticleSystem3d::create_vertex_buffer(long size)
{
	LPDIRECT3DVERTEXBUFFER9	tmp;

	HRESULT hRet=GFX->get_device()->CreateVertexBuffer(size*sizeof(VBPart),D3DUSAGE_DYNAMIC|D3DUSAGE_WRITEONLY|D3DUSAGE_POINTS,D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_PSIZE,D3DPOOL_DEFAULT,&tmp,NULL);
	if (hRet==D3D_OK) return tmp;

	ERROR_MSG("Can't create verter buffer!");

	return NULL;
}

void* ParticleSystem3d::lock_vertex_buffer(LPDIRECT3DVERTEXBUFFER9 buffer)
{
	void* ptr;

	HRESULT hRet=buffer->Lock(0,0,&ptr,D3DLOCK_DISCARD|D3DLOCK_NOOVERWRITE);
	if (hRet==D3D_OK) return ptr;

	ERROR_MSG("Can't lock vertex buffer!");

	return NULL;
}

void ParticleSystem3d::unlock_vertex_buffer(LPDIRECT3DVERTEXBUFFER9 buffer)
{
	HRESULT hRet=buffer->Unlock();
	
	if (hRet!=D3D_OK) ERROR_MSG("Can't unlock vertex buffer!");
}

float ParticleSystem3d::get_size(float life)
{
	return _size_end*(1.0f-life)+_size_start*life;
}

D3DCOLOR ParticleSystem3d::get_color(float life)
{
	Color final_color;
	fetch_color(1.0f-life,&final_color);

	Vec3f	tmp=GFX->get_color_modulation();

	return D3DCOLOR_COLORVALUE(tmp.x*final_color.x,tmp.y*final_color.y,tmp.z*final_color.z,final_color.w);
}

float ParticleSystem3d::get_rate(float life)
{
	return _rate_end*(1.0f-life)+_rate_start*life;
}

void ParticleSystem3d::fetch_color(float t,Color* c)
{
	size_t lower_bound=0xFFFF;

	ColorElems::reverse_iterator r=_color.rbegin();
	while (r!=_color.rend())
	{
		if ((*r).t<t) break;
		++r;
	}
	if (r==_color.rend()) return;
	// R has the start t (t0)
	float t0=(*r).t;

	ColorElems::iterator e=r.base();
	if (e==_color.end()) { *c=(*r).c; return; }

	float t1=(*e).t;

	float tt=(t-t0)/(t1-t0);

	*c=(*r).c*(1-tt)+(*e).c*tt;
}
