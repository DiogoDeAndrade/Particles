// BlitManager.h: interface for the BlitManager class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BLITMANAGER_H__2AD7D1D1_ECF1_4779_B147_598AF5F61AAA__INCLUDED_)
#define AFX_BLITMANAGER_H__2AD7D1D1_ECF1_4779_B147_598AF5F61AAA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Defs.h"
#include <vector>

#define MIN_BUFFER_SIZE	10

class BlitManager  
{
protected:
	struct Vertex
	{
		Vec3f		p;
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
		float					avg_z;
	};
	typedef std::vector<BlitGroup*>	BlitGroups;
	class BlitSortFunction;
public:
	BlitManager(LPDIRECT3DDEVICE9 device);
	virtual ~BlitManager();

	void	add(long texture,float x,float y,float z,float sx,float sy,float r,float g,float b,float a,float u1,float v1,float u2,float v2,D3DTEXTUREFILTERTYPE filter=D3DTEXF_LINEAR);
	void	add(long texture,Vec3f& p,Vec2f& hotspot,float angle,float sx,float sy,float r,float g,float b,float a,float u1,float v1,float u2,float v2,D3DTEXTUREFILTERTYPE filter=D3DTEXF_LINEAR);
	void	add(float x,float y,float z,float sx,float sy,const Color& c);
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
};

extern BlitManager* BLIT;

#endif // !defined(AFX_BLITMANAGER_H__2AD7D1D1_ECF1_4779_B147_598AF5F61AAA__INCLUDED_)
