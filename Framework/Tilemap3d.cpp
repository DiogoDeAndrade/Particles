#include "Tilemap3d.h"
#include "TextureManager.h"
#include "Image_A8R8G8B8.h"

Tilemap3d::Tilemap3d(void)
{
	_tilemap=NULL;
	_tile_size.set(1.0f,1.0f);	
	_atlas_texture=-1;
	_atlas_image=NULL;
}

Tilemap3d::~Tilemap3d(void)
{
	if (_tilemap) 
	{
		delete[] _tilemap;
		_tilemap=NULL;
	}
	for (size_t i=0; i<_meshes.size(); i++)
	{
		if (_meshes[i]->_tmp_geom) delete _meshes[i]->_tmp_geom;
		if (_meshes[i]->_geometry) delete _meshes[i]->_geometry;
		delete _meshes[i];
	}
	_meshes.clear();

	for (size_t i=0; i<_materials.size(); i++)
	{
		delete _materials[i];
	}
	_materials.clear();

	if (_atlas_image)
	{
		delete _atlas_image;
		_atlas_image=NULL;
	}
	_meshes.clear();
}

void Tilemap3d::draw()
{
	GFX->set_world_transformation(0,_pos.x,_pos.y,_pos.z,Quaternion(0,0,0,1));
	for (size_t i=0; i<_meshes.size(); i++)
	{
		if (_meshes[i]->_geometry)
		{
			_meshes[i]->_geometry->draw();
		}
	}
}

void Tilemap3d::create(const int sx,const int sy)
{
	if (_tilemap) 
	{
		delete[] _tilemap;
		_tilemap=NULL;
	}
	_tilemap=new Tile[sx*sy];
	for (int i=0; i<sx*sy; i++)
	{
		_tilemap[i]._height=0.0f;
		_tilemap[i]._mat=0;
	}
	_sx=sx;
	_sy=sy;
}

void Tilemap3d::set(const int x,const int y,const float height,const int mat)
{
	_tilemap[y*_sx+x]._height=height;
	_tilemap[y*_sx+x]._mat=mat;
}

void Tilemap3d::build_geom()
{
	for (size_t i=0; i<_meshes.size(); i++)
	{
		if (_meshes[i]->_tmp_geom) delete _meshes[i]->_tmp_geom;
		if (_meshes[i]->_geometry) delete _meshes[i]->_geometry;
		delete _meshes[i];
	}
	_meshes.clear();

	// Create geometry
	for (int yy=0; yy<_sy; yy++)
	{
		for (int xx=0; xx<_sx; xx++)
		{
			TileMesh*		tile_mesh_struct=get_tile_mesh_struct(xx,yy);
			Tile*			tile=&(_tilemap[yy*_sx+xx]);
			QuadVertex*		geom=tile_mesh_struct->_tmp_geom;
			TileMaterial*	mat=get_material(tile->_mat);

			Rect			floor_uv(0.0f,0.0f,1.0f,1.0f);
			unsigned long	color1=0xFFFFFFFF;
			unsigned long	color2=0x00000000;
			
			load_quad(geom,
					  Vec3f(xx*_tile_size.x,tile->_height,yy*_tile_size.y),
					  Vec3f(_tile_size.x,0.0f,0.0f),
					  Vec3f(0.0f,0.0f,_tile_size.y),
					  Vec3f(0.0f,1.0f,0.0f),
					  mat->_face_material[FACE_TOP]._uv_middle,
					  mat->_face_material[FACE_TOP]._diffuse,mat->_face_material[FACE_TOP]._emissive);

			bool	north=false,south=false,east=false,west=false;
			float	h2_n,h2_s,h2_e,h2_w;

			if (xx>0) 
			{
				if (_tilemap[yy*_sx+xx-1]._height<tile->_height) 
				{ 
					west=true; 
					h2_w=_tilemap[yy*_sx+xx-1]._height; 
				}
			}
			else 
			{ 
				west=true; 
				h2_w=0.0f;
			}
			if (xx<(_sx-1)) 
			{
				if (_tilemap[yy*_sx+xx+1]._height<tile->_height) 
				{ 
					east=true; 
					h2_e=_tilemap[yy*_sx+xx+1]._height; 
				}
			}
			else 
			{ 
				east=true; 
				h2_e=0.0f;
			}
			if (yy>0) 
			{
				if (_tilemap[(yy-1)*_sx+xx]._height<tile->_height) 
				{ 
					south=true; 
					h2_s=_tilemap[(yy-1)*_sx+xx]._height; 
				}
			}
			else 
			{ 
				south=true; 
				h2_s=0.0f;
			}
			if (yy<(_sy-1)) 
			{
				if (_tilemap[(yy+1)*_sx+xx]._height<tile->_height) 
				{ 
					north=true; 
					h2_n=_tilemap[(yy+1)*_sx+xx]._height; 
				}
			}
			else 
			{ 
				north=true; 
				h2_n=0.0f;
			}

			if (west)
			{
				load_quad(geom,
						  Vec3f(xx*_tile_size.x,h2_w,yy*_tile_size.y),
						  Vec3f(0.0f,tile->_height-h2_w,0.0f),
						  Vec3f(0.0f,0.0f,_tile_size.y),
						  Vec3f(-1.0f,0.0f,0.0f),
						 mat->_face_material[FACE_WEST]._uv_middle,
						 mat->_face_material[FACE_WEST]._diffuse,mat->_face_material[FACE_WEST]._emissive);
			}
			if (east)
			{
				load_quad(geom,
						  Vec3f((xx+1)*_tile_size.x,h2_e,yy*_tile_size.y),
						  Vec3f(0.0f,tile->_height-h2_e,0.0f),
						  Vec3f(0.0f,0.0f,_tile_size.y),
						  Vec3f(1.0f,0.0f,0.0f),
						  mat->_face_material[FACE_EAST]._uv_middle,
						  mat->_face_material[FACE_EAST]._diffuse,mat->_face_material[FACE_EAST]._emissive);
			}
			if (north)
			{
				load_quad(geom,
						  Vec3f(xx*_tile_size.x,h2_n,(yy+1)*_tile_size.y),
						  Vec3f(0.0f,tile->_height-h2_n,0.0f),
						  Vec3f(_tile_size.x,0.0f,0.0f),
						  Vec3f(0.0f,0.0f,1.0f),
						  mat->_face_material[FACE_NORTH]._uv_middle,
						  mat->_face_material[FACE_NORTH]._diffuse,mat->_face_material[FACE_NORTH]._emissive);
			}
			if (south)
			{
				load_quad(geom,
						  Vec3f(xx*_tile_size.x,h2_s,yy*_tile_size.y),
						  Vec3f(0.0f,tile->_height-h2_s,0.0f),
						  Vec3f(_tile_size.x,0.0f,0.0f),
						  Vec3f(0.0f,0.0f,-1.0f),
						  mat->_face_material[FACE_SOUTH]._uv_middle,
						  mat->_face_material[FACE_SOUTH]._diffuse,mat->_face_material[FACE_SOUTH]._emissive);
			}
		}
	}

	// Create atlas material
	if (_atlas_texture==-1)
	{
		_atlas_texture=TEXTURES->load("tile_atlas",_atlas_image);
		delete _atlas_image;
		_atlas_image=NULL;
	}

	Material* _material=new Material;
	_material->set_cull_mode(D3DCULL_NONE);
	_material->set_color_vertex(true,true);
	_material->set_alpha(false,D3DBLEND_ONE,D3DBLEND_ZERO);
	_material->set_material_color(1.0f,1.0f,1.0f,1.0f);
	_material->set_fx(FX_TEXTURED_POINTSAMP);
	_material->set_texture(0,_atlas_texture);

	for (size_t k=0; k<_meshes.size(); k++)
	{
		QuadVertex*			source=_meshes[k]->_tmp_geom;
		if (source->size()==0) continue;
		Geometry*			target=_meshes[k]->_geometry=new Geometry(false);

		target->alloc_vert((int)source->size());
		target->alloc_index((int)(6*source->size()/4));

		int			len=(int)(sizeof(VertexPNCCT)*source->size());
		VertexPNCCT* dest_v=target->get_write_pointer_v();
		memcpy(dest_v,&(source->at(0)),len);
		target->release_write_pointer_v();

		WORD* dest_i=target->get_write_pointer_i();
		for (WORD i=0; i<source->size()/4; i++)
		{
			*dest_i=(i*4); dest_i++; 
			*dest_i=(i*4)+1; dest_i++; 
			*dest_i=(i*4)+2; dest_i++; 
			*dest_i=(i*4)+1; dest_i++; 
			*dest_i=(i*4)+3; dest_i++; 
			*dest_i=(i*4)+2; dest_i++; 
		}
		target->release_write_pointer_i();
		target->set_material(_material,false);

		source->clear();
		delete _meshes[k]->_tmp_geom;
		_meshes[k]->_tmp_geom=NULL;
	}
}

Tilemap3d::TileMesh* Tilemap3d::get_tile_mesh_struct(const int x,const int y)
{
	int tx=(int)floorf(((float)x)/8.0f);
	int ty=(int)floorf(((float)y)/8.0f);

	for (size_t i=0; i<_meshes.size(); i++)
	{
		if ((tx==_meshes[i]->_tx) &&
			(ty==_meshes[i]->_ty))
		{
			return _meshes[i];
		}
	}

	TileMesh* tm=new TileMesh;
	tm->_geometry=NULL;
	tm->_tmp_geom=new QuadVertex;
	tm->_tx=tx;
	tm->_ty=ty;
	
	_meshes.push_back(tm);

	return tm;
}

void Tilemap3d::load_quad(QuadVertex* target,const Vec3f& base_pos,const Vec3f& up_vector,const Vec3f& right_vector,const Vec3f& normal,const Rect& uv,const unsigned long color1,const unsigned long color2)
{
	VertexPNCCT p[4];

	p[0].p=base_pos; p[0].n=normal; p[0].color1=color1; p[0].color2=color2; p[0].uv.set(uv.x1,uv.y1);
	p[1].p=base_pos+up_vector; p[1].n=normal; p[1].color1=color1; p[1].color2=color2; p[1].uv.set(uv.x2,uv.y1);
	p[2].p=base_pos+right_vector; p[2].n=normal; p[2].color1=color1; p[2].color2=color2; p[2].uv.set(uv.x1,uv.y2);
	p[3].p=base_pos+up_vector+right_vector; p[3].n=normal; p[3].color1=color1; p[3].color2=color2; p[3].uv.set(uv.x2,uv.y2);

	target->push_back(p[0]);
	target->push_back(p[1]);
	target->push_back(p[2]);
	target->push_back(p[3]);
}

void Tilemap3d::set_tile_size(const float sx,const float sy)
{
	_tile_size.set(sx,sy);
}

void Tilemap3d::set_material_color_diffuse(const int idx,const unsigned long color)
{
	TileMaterial* mat=get_material(idx);
	for (int i=0; i<5; i++) mat->_face_material[i]._diffuse=color;
}

void Tilemap3d::set_material_color_diffuse(const int idx,FaceType face,const unsigned long color)
{
	TileMaterial* mat=get_material(idx);
	mat->_face_material[face]._diffuse=color;
}

void Tilemap3d::set_material_color_emissive(const int idx,const unsigned long color)
{
	TileMaterial* mat=get_material(idx);
	for (int i=0; i<5; i++) mat->_face_material[i]._emissive=color;
}

void Tilemap3d::set_material_color_emissive(const int idx,FaceType face,const unsigned long color)
{
	TileMaterial* mat=get_material(idx);
	mat->_face_material[face]._emissive=color;
}

Tilemap3d::TileMaterial* Tilemap3d::get_material(const int idx)
{
	if (_materials.size()<=(size_t)idx)
	{
		for (size_t i=_materials.size(); i<=(size_t)idx; i++)
		{
			_materials.push_back(NULL);
		}
	}

	TileMaterial* mat=_materials[idx];
	if (!mat)
	{
		mat=new TileMaterial;
		for (int i=0; i<5; i++)
		{
			mat->_face_material[i]._uv_top.set(0.0f,0.0f,0.0f,0.0f);
			mat->_face_material[i]._uv_middle.set(0.0f,0.0f,0.0f,0.0f);
			mat->_face_material[i]._uv_bottom.set(0.0f,0.0f,0.0f,0.0f);
			mat->_face_material[i]._diffuse=0xFFFFFFFF;
			mat->_face_material[i]._emissive=0x00000000;
		}
		_materials[idx]=mat;
	}
	return mat;
}

void Tilemap3d::set_material_texture(const int idx,const std::string& filename)
{
	Rect rect;
	get_from_atlas(filename,rect);

	TileMaterial* mat=get_material(idx);
	for (int i=0; i<5; i++)
	{
		mat->_face_material[i]._uv_top=rect;
		mat->_face_material[i]._uv_middle=rect;
		mat->_face_material[i]._uv_bottom=rect;
	}
}

void Tilemap3d::set_material_texture(const int idx,FaceType face,const std::string& filename)
{
	Rect rect;
	get_from_atlas(filename,rect);

	TileMaterial* mat=get_material(idx);
	mat->_face_material[face]._uv_top=rect;
	mat->_face_material[face]._uv_middle=rect;
	mat->_face_material[face]._uv_bottom=rect;
}

void Tilemap3d::get_from_atlas(const std::string& filename,Rect& uv_rect)
{
	if (_atlas.count(filename)>0)
	{
		uv_rect=_atlas[filename];
		return;
	}

	// Add image to atlas
	Image_A8R8G8B8* img=new Image_A8R8G8B8;
	if (img->load_dds(filename.c_str()))
	{
		// Put this image into the atlaas
		if (!_atlas_image)
		{
			_atlas_image=new Image_A8R8G8B8;
			_atlas_image->create(1024,1024);
			_atlas_image->clear(0.0f,0.0f,0.0f,0.0f);
		}
		int x,y;
		if (!search_free_image_space(_atlas_image,(int)img->get_size_x(),(int)img->get_size_y(),x,y))
		{
			__asm int 3
		}
		_atlas_image->blit(img,x,y);
		uv_rect.x1=((float)x)/_atlas_image->get_size_x();
		uv_rect.y1=((float)y)/_atlas_image->get_size_y();
		uv_rect.x2=((float)(x+img->get_size_x()))/_atlas_image->get_size_x();
		uv_rect.y2=((float)(y+img->get_size_y()))/_atlas_image->get_size_y();
	}
	delete img;

	_atlas.insert(std::make_pair(filename,uv_rect));
}

bool Tilemap3d::search_free_image_space(Image_A8R8G8B8* img,const int sx,const int sy,int& x,int& y)
{
	unsigned char*	ptr1=img->get_data();
	for (size_t yy=0; yy<img->get_size_y()-sy-1; yy++)
	{
		for (size_t xx=0; xx<img->get_size_x()-sx-1; xx++)
		{
			unsigned char alpha=(*(ptr1+3));
			if (alpha==0)
			{
				bool fail=false;
				for (int dy=0; dy<sy; dy++)
				{
					unsigned char* ptr2=ptr1+(dy*img->get_size_x()*4);
					for (int dx=0; dx<sx; dx++)
					{
						if ((*(ptr2+3))!=0)
						{
							fail=true;
							break;
						}
						ptr2+=4;
					}
					if (fail) break;
				}
				if (!fail)
				{
					x=(int)xx;
					y=(int)yy;
					return true;
				}
			}
			ptr1+=4;
		}
	}
	return false;
}
