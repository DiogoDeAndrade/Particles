// BlitManager3d.cpp: implementation of the BlitManager3d class.
//
//////////////////////////////////////////////////////////////////////

#include "BlitManager3d.h"
#include "TextureManager.h"
#include "Utils.h"
#include "Gfx.h"
#include "Camera.h"
#include <algorithm>

BlitManager3d* BLIT3D=NULL;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

BlitManager3d::BlitManager3d(LPDIRECT3DDEVICE9 device)
{
	_device=device;
	_material.Ambient.r=1.0f; _material.Ambient.g=1.0f; _material.Ambient.b=1.0f; _material.Ambient.a=1.0f; 
	_material.Diffuse.r=1.0f; _material.Diffuse.g=1.0f; _material.Diffuse.b=1.0f; _material.Diffuse.a=1.0f; 
	_material.Specular.r=0.0f; _material.Specular.g=0.0f; _material.Specular.b=0.0f; _material.Specular.a=0.0f; 
	_material.Emissive.r=0.0f; _material.Emissive.g=0.0f; _material.Emissive.b=0.0f; _material.Emissive.a=0.0f; 
	_material.Power=0.0f;

	if (!BLIT3D) BLIT3D=this;
}

BlitManager3d::~BlitManager3d()
{
	if (BLIT3D==this) BLIT3D=NULL;
	clear_cache();
}

void BlitManager3d::clear()
{
	BlitGroups::iterator i;
	for (i=_blits.begin(); i!=_blits.end(); ++i)
	{
		(*i)->number=0;
	}
}

void BlitManager3d::clear_cache()
{
	BlitGroups::iterator i;
	for (i=_blits.begin(); i!=_blits.end(); ++i)
	{
		(*i)->number=0;
		if ((*i)->vptr) unlock_vertex_buffer((*i)->vertex_buffer);
		if ((*i)->iptr) unlock_index_buffer((*i)->index_buffer);
		(*i)->vallocated=0;
		(*i)->iallocated=0;
		(*i)->vertex_buffer->Release(); (*i)->vertex_buffer=NULL;
		(*i)->index_buffer->Release(); (*i)->index_buffer=NULL;
	}
	_blits.clear();
}

#define SET_VERTEX(vertex,px,py,pz,nx,ny,nz,pc,pu,pv) vertex.p.x=px; vertex.p.y=py; vertex.p.z=pz; vertex.n.x=nx; vertex.n.y=ny; vertex.n.z=nz; vertex.color=pc; vertex.u=pu; vertex.v=pv;

void BlitManager3d::add(const long texture,const Vec3f& p,const Vec2f& size,const Color& color,const Rect& uv)
{
	float	hx=size.x*0.5f;
	Vec3f	n=CAMERA3D->_normal_axis;
	Vec3f	pos[4];
	
	pos[0]=p-CAMERA3D->_right_axis*hx+Vec3f(0.0f,size.y,0.0f);
	pos[1]=p+CAMERA3D->_right_axis*hx+Vec3f(0.0f,size.y,0.0f);
	pos[2]=p-CAMERA3D->_right_axis*hx;
	pos[3]=p+CAMERA3D->_right_axis*hx;

	Quad		quad;
	D3DCOLOR	c=D3DCOLOR_COLORVALUE(color.r,color.g,color.b,color.a);

	SET_VERTEX(quad.vertex[0],pos[0].x,pos[0].y,pos[0].z,n.x,n.y,n.z,c,uv.x1,uv.y1);
	SET_VERTEX(quad.vertex[1],pos[1].x,pos[1].y,pos[1].z,n.x,n.y,n.z,c,uv.x2,uv.y1);
	SET_VERTEX(quad.vertex[2],pos[2].x,pos[2].y,pos[2].z,n.x,n.y,n.z,c,uv.x1,uv.y2);
	SET_VERTEX(quad.vertex[3],pos[3].x,pos[3].y,pos[3].z,n.x,n.y,n.z,c,uv.x2,uv.y2);

	BlitGroup *grp=get_blit_group(texture,D3DTEXF_POINT);

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
}

void BlitManager3d::add_ground(const long texture,const Vec3f& p,const Vec2f& size,const Color& color,const Rect& uv)
{
	float	hx=size.x*0.5f;
	float	hy=size.y*0.5f;
	Vec3f	n(0.0f,1.0f,0.0f);
	Vec3f	pos[4];

	pos[0]=p-CAMERA3D->_right_axis*hx-CAMERA3D->_normal_axis*hy;
	pos[1]=p+CAMERA3D->_right_axis*hx-CAMERA3D->_normal_axis*hy;
	pos[2]=p-CAMERA3D->_right_axis*hx+CAMERA3D->_normal_axis*hy;
	pos[3]=p+CAMERA3D->_right_axis*hx+CAMERA3D->_normal_axis*hy;

	Quad		quad;
	D3DCOLOR	c=D3DCOLOR_COLORVALUE(color.r,color.g,color.b,color.a);

	SET_VERTEX(quad.vertex[0],pos[0].x,pos[0].y,pos[0].z,n.x,n.y,n.z,c,uv.x1,uv.y1);
	SET_VERTEX(quad.vertex[1],pos[1].x,pos[1].y,pos[1].z,n.x,n.y,n.z,c,uv.x2,uv.y1);
	SET_VERTEX(quad.vertex[2],pos[2].x,pos[2].y,pos[2].z,n.x,n.y,n.z,c,uv.x1,uv.y2);
	SET_VERTEX(quad.vertex[3],pos[3].x,pos[3].y,pos[3].z,n.x,n.y,n.z,c,uv.x2,uv.y2);

	BlitGroup *grp=get_blit_group(texture,D3DTEXF_POINT);

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
}

void BlitManager3d::render()
{
	GFX->set_world_transformation(0,0.0f,0.0f,0.0f,Quaternion(0.0f,0.0f,0.0f,1.0f));
	// Render states
	GFX->set_vertex_shader(NULL);
	GFX->set_vertex_declaration(NULL);
	GFX->set_fvf(D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_DIFFUSE | D3DFVF_TEX1);
	GFX->set_renderstate(D3DRS_COLORVERTEX,TRUE);
	GFX->set_renderstate(D3DRS_AMBIENTMATERIALSOURCE,D3DMCS_COLOR1);
	GFX->set_renderstate(D3DRS_DIFFUSEMATERIALSOURCE,D3DMCS_COLOR1);
	GFX->set_renderstate(D3DRS_EMISSIVEMATERIALSOURCE,D3DMCS_MATERIAL);
	GFX->set_renderstate(D3DRS_LIGHTING,TRUE);
	GFX->set_renderstate(D3DRS_CULLMODE,D3DCULL_CCW);
	GFX->set_renderstate(D3DRS_ALPHATESTENABLE,TRUE);
	GFX->set_renderstate(D3DRS_ALPHAFUNC,D3DCMP_GREATER);
	GFX->set_renderstate(D3DRS_ALPHAREF,10);
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

	GFX->get_device()->SetMaterial(&_material);

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
				GFX->set_sampler_state(0,D3DSAMP_ADDRESSU,D3DTADDRESS_WRAP);
				GFX->set_sampler_state(0,D3DSAMP_ADDRESSV,D3DTADDRESS_WRAP);
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

LPDIRECT3DVERTEXBUFFER9	BlitManager3d::create_vertex_buffer(long size)
{
	LPDIRECT3DVERTEXBUFFER9	tmp;

	HRESULT hRet=_device->CreateVertexBuffer(size*sizeof(Quad),D3DUSAGE_DYNAMIC|D3DUSAGE_WRITEONLY,D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_DIFFUSE | D3DFVF_TEX1,D3DPOOL_DEFAULT,&tmp,NULL);
	if (hRet==D3D_OK) return tmp;

	ERROR_MSG("Can't create verter buffer!");

	return NULL;
}

void* BlitManager3d::lock_vertex_buffer(LPDIRECT3DVERTEXBUFFER9 buffer)
{
	void* ptr;

	HRESULT hRet=buffer->Lock(0,0,&ptr,D3DLOCK_DISCARD|D3DLOCK_NOOVERWRITE);
	if (hRet==D3D_OK) return ptr;

	ERROR_MSG("Can't lock vertex buffer!");

	return NULL;
}

void BlitManager3d::unlock_vertex_buffer(LPDIRECT3DVERTEXBUFFER9 buffer)
{
	HRESULT hRet=buffer->Unlock();
	
	if (hRet!=D3D_OK) ERROR_MSG("Can't unlock vertex buffer!");
}

LPDIRECT3DINDEXBUFFER9 BlitManager3d::create_index_buffer(long size)
{
	LPDIRECT3DINDEXBUFFER9	tmp;

	HRESULT hRet=_device->CreateIndexBuffer(size*sizeof(WORD)*6,D3DUSAGE_DYNAMIC|D3DUSAGE_WRITEONLY,D3DFMT_INDEX16,D3DPOOL_DEFAULT,&tmp,NULL);
	if (hRet==D3D_OK) return tmp;

	ERROR_MSG("Can't create index buffer!");

	return NULL;
}

WORD* BlitManager3d::lock_index_buffer(LPDIRECT3DINDEXBUFFER9 buffer)
{
	WORD* ptr;

	HRESULT hRet=buffer->Lock(0,0,(void**)&ptr,D3DLOCK_DISCARD|D3DLOCK_NOOVERWRITE);
	if (hRet==D3D_OK) return ptr;

	ERROR_MSG("Can't lock index buffer!");

	return NULL;
}

void BlitManager3d::unlock_index_buffer(LPDIRECT3DINDEXBUFFER9 buffer)
{
	HRESULT hRet=buffer->Unlock();
	
	if (hRet!=D3D_OK) ERROR_MSG("Can't unlock index buffer!");
}

BlitManager3d::BlitGroup *BlitManager3d::get_blit_group(long texture,D3DTEXTUREFILTERTYPE filter)
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