#pragma once
#include "Object3d.h"
#include "Mesh.h"

class Tile
{
public:
	float	_height;
	int		_mat;
};

class Image_A8R8G8B8;

typedef enum { FACE_TOP, FACE_EAST, FACE_WEST, FACE_NORTH, FACE_SOUTH } FaceType;

class Tilemap3d :
	public Object3d
{
protected:
	typedef std::vector<VertexPNCCT>	QuadVertex;
	typedef Mesh<VertexPNCCT>			Geometry;
	struct TileMesh
	{
		QuadVertex*	_tmp_geom;
		Geometry*	_geometry;
		int			_tx,_ty;
	};
	typedef std::vector<TileMesh*>	TileMeshes;
	struct TileSubMaterial
	{
		unsigned long	_diffuse;
		unsigned long	_emissive;
		Rect			_uv_top;
		Rect			_uv_middle;
		Rect			_uv_bottom;
	};
	struct TileMaterial 
	{
		TileSubMaterial	_face_material[5];
	};
	typedef std::vector<TileMaterial*>	TileMaterials;
	typedef std::map<std::string,Rect> AtlasUV;
public:
	Tilemap3d(void);
	virtual ~Tilemap3d(void);

	virtual void	draw();

			void	create(const int sx,const int sy);
			void	set(const int x,const int y,const float height,const int mat);
			void	build_geom();
			void	set_tile_size(const float sx,const float sy);

			void	set_material_color_diffuse(const int idx,const unsigned long color);
			void	set_material_color_diffuse(const int idx,FaceType face,const unsigned long color);
			void	set_material_color_emissive(const int idx,const unsigned long color);
			void	set_material_color_emissive(const int idx,FaceType face,const unsigned long color);
			void	set_material_texture(const int idx,const std::string& filename);
			void	set_material_texture(const int idx,FaceType face,const std::string& filename);

protected:
			TileMesh*		get_tile_mesh_struct(const int x,const int y);
			void			load_quad(QuadVertex* target,const Vec3f& base_pos,const Vec3f& up_vector,const Vec3f& right_vector,const Vec3f& normal,const Rect& uv,const unsigned long color1,const unsigned long color2);
			TileMaterial*	get_material(const int idx);
			void			get_from_atlas(const std::string& filename,Rect& uv_rect);
			bool			search_free_image_space(Image_A8R8G8B8* img,const int sx,const int sy,int& x,int& y);
protected:
	Tile*			_tilemap;
	int				_sx,_sy;
	TileMeshes		_meshes;
	Vec2f			_tile_size;
	TileMaterials	_materials;
	Image_A8R8G8B8*	_atlas_image;
	int				_atlas_texture;
	AtlasUV			_atlas;
};
