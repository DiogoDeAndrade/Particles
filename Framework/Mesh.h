#pragma once

#include "Defs.h"
#include "Gfx.h"
#include "Material.h"
#include <vector>
#include <string>

class Camera;
class Light;

//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
// Vertex defs
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
struct VertexPN
{
	float x,y,z;
	float nx,ny,nz;

	static DWORD get_fvf() { return D3DFVF_XYZ|D3DFVF_NORMAL; }
};

struct VertexPC
{
	float			x,y,z;
	unsigned long	color;

	static DWORD get_fvf() { return D3DFVF_XYZ|D3DFVF_DIFFUSE; }
};

struct VertexPSC
{
	float			x,y,z;
	float			s;
	unsigned long	color;

	static DWORD get_fvf() { return D3DFVF_XYZ|D3DFVF_PSIZE|D3DFVF_DIFFUSE; }
};

struct VertexPNT
{
	float x,y,z;
	float nx,ny,nz;
	float u,v;

	static DWORD get_fvf() { return D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_TEX1; }
};

struct VertexPNCT
{
	Vec3f			p;
	Vec3f			n;
	unsigned long	color;
	Vec2f			uv;

	static DWORD get_fvf() { return D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_DIFFUSE|D3DFVF_TEX1; }
};

struct VertexPNCCT
{
	Vec3f			p;
	Vec3f			n;
	unsigned long	color1;
	unsigned long	color2;
	Vec2f			uv;

	static DWORD get_fvf() { return D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_DIFFUSE|D3DFVF_SPECULAR|D3DFVF_TEX1; }
};

//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
// Base mesh (material, draw functions)
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
class BaseMesh
{
public:
	BaseMesh(const bool dynamic);
	virtual ~BaseMesh();

	virtual void		draw() = 0;

			bool		alloc_index(const int n);
			WORD*		get_write_pointer_i();
			void		release_write_pointer_i();

	inline	void		set_material(Material* material,const bool bound) { _material=material; _bound_material=bound; }

	inline	std::string	get_name() { return _name; }
	inline	void		set_name(const std::string& name) { _name=name; }
	inline	UINT		get_vertex_count() { return _n_verts; }
	inline	UINT		get_index_count() { return _n_index; }
	inline	void		push_index(WORD idx) { _index_array.push_back(idx); }

	inline	void		set_primitive(D3DPRIMITIVETYPE prim) { _primitive=prim; }
	inline	void		set_render_verts(UINT n) { _render_verts=n; }

protected:
	std::string				_name;
	bool					_dynamic;
	LPDIRECT3DVERTEXBUFFER9	_vb;
	LPDIRECT3DINDEXBUFFER9	_ib;
	UINT					_n_verts,_n_index;
	UINT					_render_verts;
	WORD*					_current_iterator_i;
	Material*				_material;
	bool					_bound_material;
	std::vector<WORD>		_index_array;
	D3DPRIMITIVETYPE		_primitive;
};

//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
// Mesh (accessor methods, etc)
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
template<class T>
class Mesh : public BaseMesh
{
public:
	Mesh(const bool dynamic);
	virtual ~Mesh(void);

	virtual	void	draw();

			bool	alloc_vert(const int n);
			T*		get_write_pointer_v();
			void	release_write_pointer_v();

			void	load(std::vector<T>* vertex,std::vector<WORD>* index)
			{
				alloc_vert((int)vertex->size());
				T* ptr_v=get_write_pointer_v();
				for (size_t i=0; i<vertex->size(); i++)
				{
					memcpy(ptr_v,&(vertex->at(i)),sizeof(T));
					ptr_v++;
				}

				alloc_index((int)index->size());
				WORD* ptr_i=get_write_pointer_i();
				for (size_t i=0; i<index->size(); i++)
				{
					*ptr_i=index->at(i);
					ptr_i++;
				}
			}

			size_t	get_current_vertex_count()
			{
				return _vertex_array.size();
			}

			void	push_vertex(const T& v)
			{
				_vertex_array.push_back(v);
			}

			void close(const bool clear_data)
			{
				load(&_vertex_array,&_index_array);
				if (clear_data)
				{
					_vertex_array.clear();
					_index_array.clear();
				}
			}

protected:
	T*				_current_iterator_v;
	std::vector<T>	_vertex_array;
};

//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
// Helper functions
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
BaseMesh*	build_cube_pn(const float sx,const float sy,const float sz);
BaseMesh*	build_cube_pnt(const float sx,const float sy,const float sz,long tex_id);
bool		load_3ds(const std::string& filename,
					 std::vector<BaseMesh*>* meshes,
					 std::vector<Camera*>* cameras,
					 std::vector<Light*>* lights);

void		build_normals_pnt(Mesh<VertexPNT>* mesh);

//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
// Templated functions
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
template<class T>
Mesh<T>::Mesh(const bool dynamic) : BaseMesh(dynamic)
{
	_current_iterator_v=NULL;
}

template<class T>
Mesh<T>::~Mesh(void)
{
	if (_current_iterator_v) release_write_pointer_v();
}

template<class T>
bool Mesh<T>::alloc_vert(const int n)
{
	if (_vb)
	{
		_vb->Release();
	}

	DWORD flags=D3DUSAGE_WRITEONLY;
	if (_dynamic) flags|=D3DUSAGE_DYNAMIC;
	long byte_size=n*sizeof(T);
	long fvf=T::get_fvf();
	if (GFX->get_device()->CreateVertexBuffer(byte_size,flags,fvf,D3DPOOL_DEFAULT,&_vb,NULL)!=D3D_OK)
	{
		_vb=NULL;
		return false;
	}

	_n_verts=n;
	_render_verts=n;

	return true;
}

template<class T>
T* Mesh<T>::get_write_pointer_v()
{
	if (_current_iterator_v) return _current_iterator_v;

	DWORD flags=0;
	if (_dynamic) flags|=D3DLOCK_DISCARD|D3DLOCK_NOOVERWRITE;

	T* ptr;
	if (_vb->Lock(0,0,(void**)&ptr,flags)!=D3D_OK)
	{
		return NULL;
	}
	_current_iterator_v=ptr;
	return ptr;
}

template<class T>
void Mesh<T>::release_write_pointer_v()
{
	if (!_current_iterator_v) return;

	_vb->Unlock();
	_current_iterator_v=NULL;
}

template<class T>
void Mesh<T>::draw()
{
	if (_current_iterator_v) release_write_pointer_v();
	if (_current_iterator_i) release_write_pointer_i();

	if (_ib)
	{
		if (_n_index==0) return;
	}
	else
	{
		if (_render_verts==0) return;
	}

	// Setup material
	if (_material) _material->set();

	// Setup VB/IB
	int s=sizeof(T);
	long fvf=T::get_fvf();
	GFX->set_vertex_declaration(NULL);
	GFX->set_fvf(fvf);
	GFX->set_stream_source(0,_vb,0,s);
	GFX->set_pixel_shader(NULL);

	if (_ib)
	{
		GFX->set_indices(_ib);
		if (_primitive==D3DPT_TRIANGLELIST) GFX->get_device()->DrawIndexedPrimitive(_primitive,0,0,_render_verts,0,_n_index/3);
		else if (_primitive==D3DPT_POINTLIST) GFX->get_device()->DrawIndexedPrimitive(_primitive,0,0,_render_verts,0,_n_index);
		else if (_primitive==D3DPT_LINELIST) GFX->get_device()->DrawIndexedPrimitive(_primitive,0,0,_render_verts,0,_n_index/2);
	}
	else
	{
		if (_primitive==D3DPT_TRIANGLELIST) GFX->get_device()->DrawPrimitive(_primitive,0,_render_verts/3);
		else if (_primitive==D3DPT_POINTLIST) GFX->get_device()->DrawPrimitive(_primitive,0,_render_verts);
		else if (_primitive==D3DPT_LINELIST) GFX->get_device()->DrawPrimitive(_primitive,0,_render_verts/2);
	}

	GFX->set_stream_source(0,NULL,0,0);
	GFX->set_indices(NULL);
}

//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
// Geometry building "library"
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
typedef Mesh<VertexPNCT> GeomMesh;

GeomMesh* geom_add_quad(GeomMesh* src_mesh,
						const Vec3f& center,const Vec3f& dir0,const Vec3f& dir1,
						const unsigned long color,const Rect& uv_rect);
GeomMesh* geom_add_cube(GeomMesh* src_mesh,
						const Vec3f& center,const Vec3f& dir_x,const Vec3f& dir_y,const Vec3f& dir_z,
						const unsigned long color,
						const bool minus_z_enable,const Rect& minus_z_uv,
						const bool plus_z_enable,const Rect& plus_z_uv,
						const bool minus_x_enable,const Rect& minus_x_uv,
						const bool plus_x_enable,const Rect& plus_x_uv,
						const bool minus_y_enable,const Rect& minus_y_uv,
						const bool plus_y_enable,const Rect& plus_y_uv);
GeomMesh* geom_add_spheroid(GeomMesh* src_mesh,
							const Vec3f& center,const Vec3f& dir_x,const Vec3f& dir_y,const Vec3f& dir_z,
							const float min_lat,const float max_lat,const float lat_inc,
							const float min_lon,const float max_lon,const float lon_inc,
							const unsigned long color,const Rect& rect_uv);
GeomMesh* geom_add_cap(GeomMesh* src_mesh,
					   const Vec3f& center,const Vec3f& dir_x,const Vec3f& dir_y,const Vec3f& dir_z,
					   const float min_lon,const float max_lon,const float lon_inc,
					   const unsigned long color,const Rect& rect_uv);
GeomMesh* geom_add_cylinder(GeomMesh* src_mesh,
							const Vec3f& bottom_center,const Vec3f& axis_x,const Vec3f& dir_y,const Vec3f& axis_z,
							const float bottom_radius,const float top_radius,
							const float y_div,
							const float min_lon,const float max_lon,const float lon_inc,
							const unsigned long color,const Rect& rect_uv);
