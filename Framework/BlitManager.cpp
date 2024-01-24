// BlitManager.cpp: implementation of the BlitManager class.
//
//////////////////////////////////////////////////////////////////////

#include "BlitManager.h"
#include "TextureManager.h"
#include "Utils.h"
#include "Gfx.h"
#include <algorithm>

BlitManager* BLIT=NULL;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

BlitManager::BlitManager(LPDIRECT3DDEVICE9 device)
{
	_device=device;
	if (!BLIT) BLIT=this;
}

BlitManager::~BlitManager()
{
	if (BLIT==this) BLIT=NULL;
	clear_cache();
}

void BlitManager::clear()
{
	BlitGroups::iterator i;
	for (i=_blits.begin(); i!=_blits.end(); ++i)
	{
		(*i)->number=0;
		(*i)->avg_z=0.0f;
	}
}

void BlitManager::clear_cache()
{
	BlitGroups::iterator i;
	for (i=_blits.begin(); i!=_blits.end(); ++i)
	{
		(*i)->number=0;
		(*i)->avg_z=0.0f;
		if ((*i)->vptr) unlock_vertex_buffer((*i)->vertex_buffer);
		if ((*i)->iptr) unlock_index_buffer((*i)->index_buffer);
		(*i)->vallocated=0;
		(*i)->iallocated=0;
		(*i)->vertex_buffer->Release(); (*i)->vertex_buffer=NULL;
		(*i)->index_buffer->Release(); (*i)->index_buffer=NULL;
	}
	_blits.clear();
}

#define SET_VERTEX(vertex,px,py,pz,pc,pu,pv) vertex.p.x=px; vertex.p.y=py; vertex.p.z=pz; vertex.color=pc; vertex.u=pu; vertex.v=pv;

void BlitManager::add(long texture,float x,float y,float z,float sx,float sy,float r,float g,float b,float a,float u1,float v1,float u2,float v2,D3DTEXTUREFILTERTYPE filter)
{
	BlitGroup *grp=get_blit_group(texture,filter);

	// Setup extra stuff
	D3DCOLOR	color=D3DCOLOR_COLORVALUE(r,g,b,a);
	float		sizex=(sx==0.0f)?(TEXTURES->get_size_x(texture)*(u2-u1)):(sx);
	float		sizey=(sy==0.0f)?(TEXTURES->get_size_y(texture)*(v2-v1)):(sy);
	float		dx=x/GFX->get_resolution().x;
	float		dy=y/GFX->get_resolution().y;
	sizex/=GFX->get_resolution().x;
	sizey/=GFX->get_resolution().y;
	// Add the blit proper
	Quad	quad;

	SET_VERTEX(quad.vertex[0],dx,dy,z,color,u1,v1);
	SET_VERTEX(quad.vertex[1],dx+sizex,dy,z,color,u2,v1);
	SET_VERTEX(quad.vertex[2],dx,dy+sizey,z,color,u1,v2);
	SET_VERTEX(quad.vertex[3],dx+sizex,dy+sizey,z,color,u2,v2);

	memcpy(grp->vptr,&quad,sizeof(Quad));
	grp->vptr=((BYTE*)grp->vptr)+sizeof(Quad);

	*(grp->iptr)=(WORD)grp->index; grp->iptr++;
	*(grp->iptr)=(WORD)grp->index+1; grp->iptr++;
	*(grp->iptr)=(WORD)grp->index+2; grp->iptr++;
	*(grp->iptr)=(WORD)grp->index+1; grp->iptr++;
	*(grp->iptr)=(WORD)grp->index+3; grp->iptr++;
	*(grp->iptr)=(WORD)grp->index+2; grp->iptr++;
	grp->index=grp->index+4;

	grp->number++;
	grp->avg_z+=z;
}

void BlitManager::add(long texture,Vec3f& p,Vec2f& hotspot,float angle,float sx,float sy,float r,float g,float b,float a,float u1,float v1,float u2,float v2,D3DTEXTUREFILTERTYPE filter)
{
	BlitGroup *grp=get_blit_group(texture,filter);

	// Setup extra stuff
	D3DCOLOR	color=D3DCOLOR_COLORVALUE(r,g,b,a);
	float		sizex=(sx==0.0f)?(TEXTURES->get_size_x(texture)):(sx);
	float		sizey=(sy==0.0f)?(TEXTURES->get_size_y(texture)):(sy);
	// Add the blit proper
	Quad	quad;

	SET_VERTEX(quad.vertex[0],-hotspot.x,-hotspot.y,p.z,color,u1,v1);
	SET_VERTEX(quad.vertex[1],sizex-hotspot.x,-hotspot.y,p.z,color,u2,v1);
	SET_VERTEX(quad.vertex[2],-hotspot.x,sizey-hotspot.y,p.z,color,u1,v2);
	SET_VERTEX(quad.vertex[3],sizex-hotspot.x,sizey-hotspot.y,p.z,color,u2,v2);

	Vec3f	t(p.x,p.y,0);
	for (unsigned long i=0; i<4; i++)
	{
		quad.vertex[i].p=rotate_point2d(quad.vertex[i].p,angle);

		quad.vertex[i].p=quad.vertex[i].p+t;

		quad.vertex[i].p.x/=GFX->get_resolution().x;
		quad.vertex[i].p.y/=GFX->get_resolution().y;
	}

	memcpy(grp->vptr,&quad,sizeof(Quad));
	grp->vptr=((BYTE*)grp->vptr)+sizeof(Quad);

	*(grp->iptr)=(WORD)grp->index; grp->iptr++;
	*(grp->iptr)=(WORD)grp->index+1; grp->iptr++;
	*(grp->iptr)=(WORD)grp->index+2; grp->iptr++;
	*(grp->iptr)=(WORD)grp->index+1; grp->iptr++;
	*(grp->iptr)=(WORD)grp->index+3; grp->iptr++;
	*(grp->iptr)=(WORD)grp->index+2; grp->iptr++;
	grp->index=grp->index+4;

	grp->number++;
	grp->avg_z+=p.z;
}

void BlitManager::add(float x,float y,float z,float sx,float sy,const Color& c)
{
	BlitGroup *grp=get_blit_group(-1,D3DTEXF_LINEAR);

	// Setup extra stuff
	D3DCOLOR	color=D3DCOLOR_COLORVALUE(c.r,c.g,c.b,c.a);
	float		sizex=sx;
	float		sizey=sy;
	// Add the blit proper
	Quad	quad;

	SET_VERTEX(quad.vertex[0],0.0f,0.0f,z,color,0.0f,0.0f);
	SET_VERTEX(quad.vertex[1],sizex,0.0f,z,color,0.0f,0.0f);
	SET_VERTEX(quad.vertex[2],0.0f,sizey,z,color,0.0f,0.0f);
	SET_VERTEX(quad.vertex[3],sizex,sizey,z,color,0.0f,0.0f);

	Vec3f	t(x,y,0);
	for (unsigned long i=0; i<4; i++)
	{
		quad.vertex[i].p=quad.vertex[i].p+t;

		quad.vertex[i].p.x/=GFX->get_resolution().x;
		quad.vertex[i].p.y/=GFX->get_resolution().y;
	}

	memcpy(grp->vptr,&quad,sizeof(Quad));
	grp->vptr=((BYTE*)grp->vptr)+sizeof(Quad);

	*(grp->iptr)=(WORD)grp->index; grp->iptr++;
	*(grp->iptr)=(WORD)grp->index+1; grp->iptr++;
	*(grp->iptr)=(WORD)grp->index+2; grp->iptr++;
	*(grp->iptr)=(WORD)grp->index+1; grp->iptr++;
	*(grp->iptr)=(WORD)grp->index+3; grp->iptr++;
	*(grp->iptr)=(WORD)grp->index+2; grp->iptr++;
	grp->index=grp->index+4;

	grp->number++;
	grp->avg_z+=z;
}

class BlitManager::BlitSortFunction
{
public:
	bool operator()(const BlitGroup* o1,const BlitGroup* o2)
	{
		return o1->avg_z>o2->avg_z;
	}
};

void BlitManager::render()
{
	// Sort grps by z
	for (size_t i=0; i<_blits.size(); i++)
	{
		if (_blits[i]->number==0) _blits[i]->avg_z=1.0f;
		else _blits[i]->avg_z=_blits[i]->avg_z/(float)_blits[i]->number;
	}
	std::sort(_blits.begin(),_blits.end(),BlitSortFunction());

	// Render states
	GFX->set_vertex_shader(NULL);
	GFX->set_vertex_declaration(NULL);
	GFX->set_fvf(D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1);
	GFX->set_renderstate(D3DRS_COLORVERTEX,TRUE);
	GFX->set_renderstate(D3DRS_AMBIENTMATERIALSOURCE,D3DMCS_COLOR1);
	GFX->set_renderstate(D3DRS_DIFFUSEMATERIALSOURCE,D3DMCS_COLOR1);
	GFX->set_renderstate(D3DRS_EMISSIVEMATERIALSOURCE,D3DMCS_COLOR1);
	GFX->set_renderstate(D3DRS_LIGHTING,FALSE);
	GFX->set_renderstate(D3DRS_CULLMODE,D3DCULL_CCW);
	GFX->set_renderstate(D3DRS_ALPHATESTENABLE,TRUE);
	GFX->set_renderstate(D3DRS_ALPHAFUNC,D3DCMP_GREATER);
	GFX->set_renderstate(D3DRS_ALPHAREF,50);
	GFX->set_texture_stage_state(0,D3DTSS_COLOROP,D3DTOP_MODULATE);
	GFX->set_texture_stage_state(0,D3DTSS_ALPHAOP,D3DTOP_MODULATE);
	GFX->set_texture_stage_state(0,D3DTSS_COLORARG1,D3DTA_TEXTURE);
	GFX->set_texture_stage_state(0,D3DTSS_COLORARG2,D3DTA_DIFFUSE);
	GFX->set_texture_stage_state(0,D3DTSS_ALPHAARG1,D3DTA_TEXTURE);
	GFX->set_texture_stage_state(0,D3DTSS_ALPHAARG2,D3DTA_DIFFUSE);
	GFX->set_texture_stage_state(1,D3DTSS_COLOROP,D3DTOP_DISABLE);
	GFX->set_texture_stage_state(1,D3DTSS_ALPHAOP,D3DTOP_DISABLE);

	GFX->set_renderstate(D3DRS_ALPHABLENDENABLE,TRUE);
	GFX->set_renderstate(D3DRS_SRCBLEND,D3DBLEND_SRCALPHA);
	GFX->set_renderstate(D3DRS_DESTBLEND,D3DBLEND_INVSRCALPHA);

	// Render
	BlitGroups::iterator i;
	for (i=_blits.begin(); i!=_blits.end(); ++i)
	{
		if (*i)
		{
			if ((*i)->number>0)
			{
				// Set the texture
				GFX->set_texture(0,TEXTURES->get_texture((*i)->texture_id));
				GFX->set_sampler_state(0,D3DSAMP_MINFILTER,(*i)->filter);
				GFX->set_sampler_state(0,D3DSAMP_MAGFILTER,(*i)->filter);
				GFX->set_sampler_state(0,D3DSAMP_MIPFILTER,(*i)->filter);
				GFX->set_sampler_state(0,D3DSAMP_ADDRESSU,D3DTADDRESS_CLAMP);
				GFX->set_sampler_state(0,D3DSAMP_ADDRESSV,D3DTADDRESS_CLAMP);
				// Unlock vertex buffers, etc	
				if ((*i)->vptr) { unlock_vertex_buffer((*i)->vertex_buffer); (*i)->vptr=NULL; }
				if ((*i)->iptr) { unlock_index_buffer((*i)->index_buffer); (*i)->iptr=NULL; }
				// Draw!
				GFX->set_stream_source(0,(*i)->vertex_buffer,0,sizeof(Vertex));
				// Set indices
				GFX->set_indices((*i)->index_buffer);
				HRESULT hRet=_device->DrawIndexedPrimitive(D3DPT_TRIANGLELIST,0,0,(*i)->number*4,0,(*i)->number*2);
				if (hRet!=D3D_OK)
				{
					ERROR_MSG("Can't draw indexed primitive!");
					continue;
				}

			}
		}
	}

	GFX->set_indices(NULL);
	GFX->set_stream_source(0,NULL,0,0);
	GFX->set_texture(0,NULL);
}

LPDIRECT3DVERTEXBUFFER9	BlitManager::create_vertex_buffer(long size)
{
	LPDIRECT3DVERTEXBUFFER9	tmp;

	HRESULT hRet=_device->CreateVertexBuffer(size*sizeof(Quad),D3DUSAGE_DYNAMIC|D3DUSAGE_WRITEONLY,D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1,D3DPOOL_DEFAULT,&tmp,NULL);
	if (hRet==D3D_OK) return tmp;

	ERROR_MSG("Can't create verter buffer!");

	return NULL;
}

void* BlitManager::lock_vertex_buffer(LPDIRECT3DVERTEXBUFFER9 buffer)
{
	void* ptr;

	HRESULT hRet=buffer->Lock(0,0,&ptr,D3DLOCK_DISCARD|D3DLOCK_NOOVERWRITE);
	if (hRet==D3D_OK) return ptr;

	ERROR_MSG("Can't lock vertex buffer!");

	return NULL;
}

void BlitManager::unlock_vertex_buffer(LPDIRECT3DVERTEXBUFFER9 buffer)
{
	HRESULT hRet=buffer->Unlock();
	
	if (hRet!=D3D_OK) ERROR_MSG("Can't unlock vertex buffer!");
}

LPDIRECT3DINDEXBUFFER9 BlitManager::create_index_buffer(long size)
{
	LPDIRECT3DINDEXBUFFER9	tmp;

	HRESULT hRet=_device->CreateIndexBuffer(size*sizeof(WORD)*6,D3DUSAGE_DYNAMIC|D3DUSAGE_WRITEONLY,D3DFMT_INDEX16,D3DPOOL_DEFAULT,&tmp,NULL);
	if (hRet==D3D_OK) return tmp;

	ERROR_MSG("Can't create index buffer!");

	return NULL;
}

WORD* BlitManager::lock_index_buffer(LPDIRECT3DINDEXBUFFER9 buffer)
{
	WORD* ptr;

	HRESULT hRet=buffer->Lock(0,0,(void**)&ptr,D3DLOCK_DISCARD|D3DLOCK_NOOVERWRITE);
	if (hRet==D3D_OK) return ptr;

	ERROR_MSG("Can't lock index buffer!");

	return NULL;
}

void BlitManager::unlock_index_buffer(LPDIRECT3DINDEXBUFFER9 buffer)
{
	HRESULT hRet=buffer->Unlock();
	
	if (hRet!=D3D_OK) ERROR_MSG("Can't unlock index buffer!");
}

BlitManager::BlitGroup *BlitManager::get_blit_group(long texture,D3DTEXTUREFILTERTYPE filter)
{
	BlitGroup *grp=NULL;

	// Seek proper blit group
	BlitGroups::iterator i;
	for (i=_blits.begin(); i!=_blits.end(); ++i)
	{
		if (((*i)->texture_id==texture) && ((*i)->filter==filter))
		{
			grp=*i;
			break;
		}
	}
	if (!grp)
	{
		grp=new BlitGroup;
		grp->vallocated=0;
		grp->iallocated=0;
		grp->number=0;
		grp->avg_z=0.0f;
		grp->vertex_buffer=NULL;
		grp->index_buffer=NULL;
		grp->texture_id=texture;
		grp->filter=filter;
		grp->vptr=NULL;
		grp->iptr=NULL;
		grp->index=0;
		_blits.push_back(grp);
	}
	if (!grp->vertex_buffer)
	{
		grp->vertex_buffer=create_vertex_buffer(MIN_BUFFER_SIZE);
		grp->vallocated=MIN_BUFFER_SIZE;
		grp->vptr=lock_vertex_buffer(grp->vertex_buffer);
	}
	else
	{
		if (grp->vallocated==grp->number)
		{
			// Resize
			LPDIRECT3DVERTEXBUFFER9	tmp=create_vertex_buffer(grp->vallocated*10);
			void*					ptr=lock_vertex_buffer(tmp);

			// Copy old data
			if (grp->vptr) unlock_vertex_buffer(grp->vertex_buffer);
			grp->vptr=lock_vertex_buffer(grp->vertex_buffer);
			memcpy(ptr,grp->vptr,sizeof(Quad)*grp->vallocated);
			// Delete old vb
			unlock_vertex_buffer(grp->vertex_buffer);
			grp->vertex_buffer->Release();
			grp->vertex_buffer=tmp;
			grp->vptr=((BYTE*)ptr)+(sizeof(Quad)*grp->vallocated);
			grp->vallocated*=2;
		}
	}
	if (!grp->index_buffer)
	{
		grp->index_buffer=create_index_buffer(MIN_BUFFER_SIZE);
		grp->iallocated=MIN_BUFFER_SIZE;
		grp->iptr=lock_index_buffer(grp->index_buffer);
		grp->index=0;
	}
	else
	{
		if (grp->iallocated==grp->number)
		{
			// Resize
			LPDIRECT3DINDEXBUFFER9	tmp=create_index_buffer(grp->iallocated*2);
			WORD*					ptr=lock_index_buffer(tmp);

			// Copy old data
			if (grp->iptr) unlock_index_buffer(grp->index_buffer);
			grp->iptr=lock_index_buffer(grp->index_buffer);
			memcpy(ptr,grp->iptr,sizeof(WORD)*6*grp->iallocated);
			// Delete old vb
			unlock_index_buffer(grp->index_buffer);
			grp->index_buffer->Release();
			grp->index_buffer=tmp;
			grp->iptr=((WORD*)ptr)+(6*grp->iallocated);
			grp->iallocated*=2;
		}
	}
	if (!grp->vptr)
	{
		grp->vptr=lock_vertex_buffer(grp->vertex_buffer);
		grp->index=0;
	}
	if (!grp->iptr)
	{
		grp->iptr=lock_index_buffer(grp->index_buffer);
	}
	return grp;
}