// BlitManager3d.h: interface for the BlitManager3d class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __BLITMANAGER3D_H
#define __BLITMANAGER3D_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Defs.h"
#include <vector>

#define MIN_BUFFER_SIZE	10

class Camera;

class BlitManager3d  
{
protected:
	struct Vertex
	{
		Vec3f		p;
		Vec3f		n;
		DWORD		color;
		float		u,v;
	};
	struct Quad
	{
		Vertex	vertex[4];
	};
	struct BlitGroup
	{
		long					texture_id;
		D3DTEXTUREFILTERTYPE	filter;
		long					number;
		long					index;
		LPDIRECT3DVERTEXBUFFER9	vertex_buffer;
		LPDIRECT3DINDEXBUFFER9	index_buffer;
		long					vallocated;
		long					iallocated;
		void*					vptr;
		WORD*					iptr;
	};
	typedef std::vector<BlitGroup*>	BlitGroups;
	class BlitSortFunction;
public:
	BlitManager3d(LPDIRECT3DDEVICE9 device);
	virtual ~BlitManager3d();

	void	add(const long texture,const Vec3f& p,const Vec2f& size,const Color& color,const Rect& uv);
	void	add_ground(const long texture,const Vec3f& p,const Vec2f& size,const Color& color,const Rect& uv);

	void	render();
	void	clear();
	void	clear_cache();

protected:
	LPDIRECT3DVERTEXBUFFER9	create_vertex_buffer(long size);
	void*					lock_vertex_buffer(LPDIRECT3DVERTEXBUFFER9 buffer);
	void					unlock_vertex_buffer(LPDIRECT3DVERTEXBUFFER9 buffer);
	LPDIRECT3DINDEXBUFFER9	create_index_buffer(long size);
	WORD*					lock_index_buffer(LPDIRECT3DINDEXBUFFER9 buffer);
	void					unlock_index_buffer(LPDIRECT3DINDEXBUFFER9 buffer);
	BlitGroup*				get_blit_group(long texture,D3DTEXTUREFILTERTYPE filter);

protected:
	LPDIRECT3DDEVICE9	_device;
	BlitGroups			_blits;
	D3DMATERIAL9		_material;
};

extern BlitManager3d* BLIT3D;

#endif // !defined(AFX_BLITMANAGER_H__2AD7D1D1_ECF1_4779_B147_598AF5F61AAA__INCLUDED_)
