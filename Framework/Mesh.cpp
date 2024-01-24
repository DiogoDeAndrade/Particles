#include "Mesh.h"
#include "Gfx.h"
#include "Utils.h"
#include "TextureManager.h"
#include "Camera.h"
#include "Light.h"

///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////
BaseMesh::BaseMesh(const bool dynamic)
{
	_vb=NULL;
	_ib=NULL;
	_dynamic=dynamic;
	_name="";
	_material=NULL;
	_current_iterator_i=NULL;
	_bound_material=false;
	_primitive=D3DPT_TRIANGLELIST;
}

BaseMesh::~BaseMesh()
{
	if (_current_iterator_i) release_write_pointer_i();
	if (_vb) 
	{
		_vb->Release();
		_vb=NULL;
	}
	if (_ib) 
	{
		_ib->Release();
		_ib=NULL;
	}
	if (_material)
	{
		if (_bound_material)
		{
			delete _material;
			_material=NULL;
		}
	}
}

bool BaseMesh::alloc_index(const int n)
{
	if (_ib)
	{
		_ib->Release();
	}

	DWORD flags=D3DUSAGE_WRITEONLY;
	if (_dynamic) flags|=D3DUSAGE_DYNAMIC;

	if (GFX->get_device()->CreateIndexBuffer(n*sizeof(WORD),flags,D3DFMT_INDEX16,D3DPOOL_DEFAULT,&_ib,NULL)!=D3D_OK)
	{
		_ib=NULL;
		return false;
	}

	_n_index=n;

	return true;
}

WORD* BaseMesh::get_write_pointer_i()
{
	if (_current_iterator_i) return _current_iterator_i;

	DWORD flags=0;
	if (_dynamic) flags|=D3DLOCK_DISCARD|D3DLOCK_NOOVERWRITE;

	WORD* ptr;
	if (_ib->Lock(0,0,(void**)&ptr,flags)!=D3D_OK)
	{
		return NULL;
	}
	_current_iterator_i=ptr;
	return ptr;
}

void BaseMesh::release_write_pointer_i()
{
	if (!_current_iterator_i) return;

	_ib->Unlock();

	_current_iterator_i=NULL;
}

///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////

BaseMesh* build_cube_pn(const float sx,const float sy,const float sz)
{
	Mesh<VertexPN>*	mesh=new Mesh<VertexPN>(false);

	if (!mesh->alloc_vert(24)) 
	{
		delete mesh;
		return NULL;
	}

	float dx=sx*0.5f;
	float dy=sy*0.5f;
	float dz=sz*0.5f;

	// Build vertices
	VertexPN* ptr=mesh->get_write_pointer_v();

	// Bottom face
	ptr->x=-dx; ptr->y=-dy; ptr->z=-dz; ptr->nx= 0.0f; ptr->ny=-1.0f; ptr->nz= 0.0f; ptr++;
	ptr->x= dx; ptr->y=-dy; ptr->z=-dz; ptr->nx= 0.0f; ptr->ny=-1.0f; ptr->nz= 0.0f; ptr++;
	ptr->x=-dx; ptr->y=-dy; ptr->z= dz; ptr->nx= 0.0f; ptr->ny=-1.0f; ptr->nz= 0.0f; ptr++;
	ptr->x= dx; ptr->y=-dy; ptr->z= dz; ptr->nx= 0.0f; ptr->ny=-1.0f; ptr->nz= 0.0f; ptr++;
	// Top face
	ptr->x=-dx; ptr->y= dy; ptr->z=-dz; ptr->nx= 0.0f; ptr->ny= 1.0f; ptr->nz= 0.0f; ptr++;
	ptr->x= dx; ptr->y= dy; ptr->z=-dz; ptr->nx= 0.0f; ptr->ny= 1.0f; ptr->nz= 0.0f; ptr++;
	ptr->x=-dx; ptr->y= dy; ptr->z= dz; ptr->nx= 0.0f; ptr->ny= 1.0f; ptr->nz= 0.0f; ptr++;
	ptr->x= dx; ptr->y= dy; ptr->z= dz; ptr->nx= 0.0f; ptr->ny= 1.0f; ptr->nz= 0.0f; ptr++;
	// Right face
	ptr->x= dx; ptr->y=-dy; ptr->z=-dz; ptr->nx= 1.0f; ptr->ny= 0.0f; ptr->nz= 0.0f; ptr++;
	ptr->x= dx; ptr->y=-dy; ptr->z= dz; ptr->nx= 1.0f; ptr->ny= 0.0f; ptr->nz= 0.0f; ptr++;
	ptr->x= dx; ptr->y= dy; ptr->z=-dz; ptr->nx= 1.0f; ptr->ny= 0.0f; ptr->nz= 0.0f; ptr++;
	ptr->x= dx; ptr->y= dy; ptr->z= dz; ptr->nx= 1.0f; ptr->ny= 0.0f; ptr->nz= 0.0f; ptr++;
	// Left face
	ptr->x=-dx; ptr->y=-dy; ptr->z=-dz; ptr->nx=-1.0f; ptr->ny= 0.0f; ptr->nz= 0.0f; ptr++;
	ptr->x=-dx; ptr->y=-dy; ptr->z= dz; ptr->nx=-1.0f; ptr->ny= 0.0f; ptr->nz= 0.0f; ptr++;
	ptr->x=-dx; ptr->y= dy; ptr->z=-dz; ptr->nx=-1.0f; ptr->ny= 0.0f; ptr->nz= 0.0f; ptr++;
	ptr->x=-dx; ptr->y= dy; ptr->z= dz; ptr->nx=-1.0f; ptr->ny= 0.0f; ptr->nz= 0.0f; ptr++;
	// Front face
	ptr->x=-dx; ptr->y=-dy; ptr->z=-dz; ptr->nx= 0.0f; ptr->ny= 0.0f; ptr->nz=-1.0f; ptr++;
	ptr->x= dx; ptr->y=-dy; ptr->z=-dz; ptr->nx= 0.0f; ptr->ny= 0.0f; ptr->nz=-1.0f; ptr++;
	ptr->x=-dx; ptr->y= dy; ptr->z=-dz; ptr->nx= 0.0f; ptr->ny= 0.0f; ptr->nz=-1.0f; ptr++;
	ptr->x= dx; ptr->y= dy; ptr->z=-dz; ptr->nx= 0.0f; ptr->ny= 0.0f; ptr->nz=-1.0f; ptr++;
	// Back face
	ptr->x=-dx; ptr->y=-dy; ptr->z= dz; ptr->nx= 0.0f; ptr->ny= 0.0f; ptr->nz= 1.0f; ptr++;
	ptr->x= dx; ptr->y=-dy; ptr->z= dz; ptr->nx= 0.0f; ptr->ny= 0.0f; ptr->nz= 1.0f; ptr++;
	ptr->x=-dx; ptr->y= dy; ptr->z= dz; ptr->nx= 0.0f; ptr->ny= 0.0f; ptr->nz= 1.0f; ptr++;
	ptr->x= dx; ptr->y= dy; ptr->z= dz; ptr->nx= 0.0f; ptr->ny= 0.0f; ptr->nz= 1.0f; ptr++;

	mesh->release_write_pointer_v();

	// Build index
	if (!mesh->alloc_index(36))
	{
		delete mesh;
		return NULL;
	}

	WORD* iptr=mesh->get_write_pointer_i();

	*iptr= 0; iptr++; *iptr= 2; iptr++; *iptr= 1; iptr++;
	*iptr= 1; iptr++; *iptr= 2; iptr++; *iptr= 3; iptr++;
	*iptr= 4; iptr++; *iptr= 5; iptr++; *iptr= 6; iptr++;
	*iptr= 5; iptr++; *iptr= 7; iptr++; *iptr= 6; iptr++;
	*iptr= 8; iptr++; *iptr= 9; iptr++; *iptr=10; iptr++;
	*iptr= 9; iptr++; *iptr=11; iptr++; *iptr=10; iptr++;
	*iptr=12; iptr++; *iptr=14; iptr++; *iptr=13; iptr++;
	*iptr=13; iptr++; *iptr=14; iptr++; *iptr=15; iptr++;
	*iptr=16; iptr++; *iptr=17; iptr++; *iptr=18; iptr++;
	*iptr=17; iptr++; *iptr=19; iptr++; *iptr=18; iptr++;
	*iptr=20; iptr++; *iptr=22; iptr++; *iptr=21; iptr++;
	*iptr=21; iptr++; *iptr=22; iptr++; *iptr=23; iptr++;

	mesh->release_write_pointer_i();

	return mesh;
}

BaseMesh* build_cube_pnt(const float sx,const float sy,const float sz,long tex_id)
{
	Mesh<VertexPNT>*	mesh=new Mesh<VertexPNT>(false);

	if (!mesh->alloc_vert(24)) 
	{
		delete mesh;
		return NULL;
	}

	float dx=sx*0.5f;
	float dy=sy*0.5f;
	float dz=sz*0.5f;

	// Build vertices
	VertexPNT* ptr=mesh->get_write_pointer_v();

	// Bottom face
	ptr->x=-dx; ptr->y=-dy; ptr->z=-dz; ptr->nx= 0.0f; ptr->ny=-1.0f; ptr->nz= 0.0f; ptr->u=0.0f; ptr->v=0.0f; ptr++;
	ptr->x= dx; ptr->y=-dy; ptr->z=-dz; ptr->nx= 0.0f; ptr->ny=-1.0f; ptr->nz= 0.0f; ptr->u=1.0f; ptr->v=0.0f; ptr++;
	ptr->x=-dx; ptr->y=-dy; ptr->z= dz; ptr->nx= 0.0f; ptr->ny=-1.0f; ptr->nz= 0.0f; ptr->u=0.0f; ptr->v=1.0f; ptr++;
	ptr->x= dx; ptr->y=-dy; ptr->z= dz; ptr->nx= 0.0f; ptr->ny=-1.0f; ptr->nz= 0.0f; ptr->u=1.0f; ptr->v=1.0f; ptr++;
	// Top face
	ptr->x=-dx; ptr->y= dy; ptr->z=-dz; ptr->nx= 0.0f; ptr->ny= 1.0f; ptr->nz= 0.0f; ptr->u=0.0f; ptr->v=1.0f; ptr++;
	ptr->x= dx; ptr->y= dy; ptr->z=-dz; ptr->nx= 0.0f; ptr->ny= 1.0f; ptr->nz= 0.0f; ptr->u=1.0f; ptr->v=1.0f; ptr++;
	ptr->x=-dx; ptr->y= dy; ptr->z= dz; ptr->nx= 0.0f; ptr->ny= 1.0f; ptr->nz= 0.0f; ptr->u=0.0f; ptr->v=0.0f; ptr++;
	ptr->x= dx; ptr->y= dy; ptr->z= dz; ptr->nx= 0.0f; ptr->ny= 1.0f; ptr->nz= 0.0f; ptr->u=1.0f; ptr->v=0.0f; ptr++;
	// Right face
	ptr->x= dx; ptr->y=-dy; ptr->z=-dz; ptr->nx= 1.0f; ptr->ny= 0.0f; ptr->nz= 0.0f; ptr->u=0.0f; ptr->v=1.0f; ptr++;
	ptr->x= dx; ptr->y=-dy; ptr->z= dz; ptr->nx= 1.0f; ptr->ny= 0.0f; ptr->nz= 0.0f; ptr->u=1.0f; ptr->v=1.0f; ptr++;
	ptr->x= dx; ptr->y= dy; ptr->z=-dz; ptr->nx= 1.0f; ptr->ny= 0.0f; ptr->nz= 0.0f; ptr->u=0.0f; ptr->v=0.0f; ptr++;
	ptr->x= dx; ptr->y= dy; ptr->z= dz; ptr->nx= 1.0f; ptr->ny= 0.0f; ptr->nz= 0.0f; ptr->u=1.0f; ptr->v=0.0f; ptr++;
	// Left face
	ptr->x=-dx; ptr->y=-dy; ptr->z=-dz; ptr->nx=-1.0f; ptr->ny= 0.0f; ptr->nz= 0.0f; ptr->u=1.0f; ptr->v=1.0f; ptr++;
	ptr->x=-dx; ptr->y=-dy; ptr->z= dz; ptr->nx=-1.0f; ptr->ny= 0.0f; ptr->nz= 0.0f; ptr->u=0.0f; ptr->v=1.0f; ptr++;
	ptr->x=-dx; ptr->y= dy; ptr->z=-dz; ptr->nx=-1.0f; ptr->ny= 0.0f; ptr->nz= 0.0f; ptr->u=1.0f; ptr->v=0.0f; ptr++;
	ptr->x=-dx; ptr->y= dy; ptr->z= dz; ptr->nx=-1.0f; ptr->ny= 0.0f; ptr->nz= 0.0f; ptr->u=0.0f; ptr->v=0.0f; ptr++;
	// Front face
	ptr->x=-dx; ptr->y=-dy; ptr->z=-dz; ptr->nx= 0.0f; ptr->ny= 0.0f; ptr->nz=-1.0f; ptr->u=0.0f; ptr->v=1.0f; ptr++;
	ptr->x= dx; ptr->y=-dy; ptr->z=-dz; ptr->nx= 0.0f; ptr->ny= 0.0f; ptr->nz=-1.0f; ptr->u=1.0f; ptr->v=1.0f; ptr++;
	ptr->x=-dx; ptr->y= dy; ptr->z=-dz; ptr->nx= 0.0f; ptr->ny= 0.0f; ptr->nz=-1.0f; ptr->u=0.0f; ptr->v=0.0f; ptr++;
	ptr->x= dx; ptr->y= dy; ptr->z=-dz; ptr->nx= 0.0f; ptr->ny= 0.0f; ptr->nz=-1.0f; ptr->u=1.0f; ptr->v=0.0f; ptr++;
	// Back face
	ptr->x=-dx; ptr->y=-dy; ptr->z= dz; ptr->nx= 0.0f; ptr->ny= 0.0f; ptr->nz= 1.0f; ptr->u=1.0f; ptr->v=1.0f; ptr++;
	ptr->x= dx; ptr->y=-dy; ptr->z= dz; ptr->nx= 0.0f; ptr->ny= 0.0f; ptr->nz= 1.0f; ptr->u=0.0f; ptr->v=1.0f; ptr++;
	ptr->x=-dx; ptr->y= dy; ptr->z= dz; ptr->nx= 0.0f; ptr->ny= 0.0f; ptr->nz= 1.0f; ptr->u=1.0f; ptr->v=0.0f; ptr++;
	ptr->x= dx; ptr->y= dy; ptr->z= dz; ptr->nx= 0.0f; ptr->ny= 0.0f; ptr->nz= 1.0f; ptr->u=0.0f; ptr->v=0.0f; ptr++;

	mesh->release_write_pointer_v();

	// Build index
	if (!mesh->alloc_index(36))
	{
		delete mesh;
		return NULL;
	}

	WORD* iptr=mesh->get_write_pointer_i();

	*iptr= 0; iptr++; *iptr= 2; iptr++; *iptr= 1; iptr++;
	*iptr= 1; iptr++; *iptr= 2; iptr++; *iptr= 3; iptr++;
	*iptr= 4; iptr++; *iptr= 5; iptr++; *iptr= 6; iptr++;
	*iptr= 5; iptr++; *iptr= 7; iptr++; *iptr= 6; iptr++;
	*iptr= 8; iptr++; *iptr= 9; iptr++; *iptr=10; iptr++;
	*iptr= 9; iptr++; *iptr=11; iptr++; *iptr=10; iptr++;
	*iptr=12; iptr++; *iptr=14; iptr++; *iptr=13; iptr++;
	*iptr=13; iptr++; *iptr=14; iptr++; *iptr=15; iptr++;
	*iptr=16; iptr++; *iptr=17; iptr++; *iptr=18; iptr++;
	*iptr=17; iptr++; *iptr=19; iptr++; *iptr=18; iptr++;
	*iptr=20; iptr++; *iptr=22; iptr++; *iptr=21; iptr++;
	*iptr=21; iptr++; *iptr=22; iptr++; *iptr=23; iptr++;

	mesh->release_write_pointer_i();

	return mesh;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 3ds loader (very simple, just for meshes)
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void load_and_parse_chunk(FILE* file,std::vector<BaseMesh*> *meshes,std::vector<Camera*>* cameras,std::vector<Light*>* lights)
{
	static	std::string		current_name;
			unsigned short	chunk_id;
			unsigned long	chunk_len;
			unsigned long	start_pos=ftell(file);

	fread (&chunk_id,sizeof(unsigned short),1,file); //Read the chunk header
	if (feof(file)) return;
	fread (&chunk_len,sizeof(unsigned long),1,file); //Read the length of the chunk 
	if (feof(file)) return;

	Mesh<VertexPNT>*	mesh=NULL;
	if (meshes)
	{
		if (!meshes->empty())
		{
			mesh=(Mesh<VertexPNT>*)meshes->at(meshes->size()-1);
		}
	}
	Light*				lgt=NULL;
	if (lights)
	{
		if (!lights->empty())
		{
			lgt=lights->at(lights->size()-1);
		}
	}

	switch (chunk_id)
	{
		// Ignore chunks
		case 0x4D4D:		// Main chunk
		case 0x4100:		// Triangular Mesh chunk (parent)
		case 0xA200:		// Texture Map 1 chunk (parent)
		case 0x3D3D:		// Object Block chunk
			break;
		// Interesting chunks
		case 0xAFFF:		// Material chunk (parent)
			{
				fseek(file,-6,SEEK_CUR);
				load_material_3ds(file);
			}
			break;
		case 0x4000:		// Object Block chunk
			{
				char buffer[512];
				int		i=0;
				char	c;
				do 
				{
					fread(&c,sizeof(unsigned char),1,file);
					buffer[i]=c;
					i++;
				} while ((c!='\0') && (i<20));
				buffer[i+1]=0;
				current_name=buffer;
			}
			break;
		case 0x4110:		// Vertices List chunk
			{
				mesh=new Mesh<VertexPNT>(false);
				meshes->push_back(mesh);
				mesh->set_name(current_name);

				unsigned short n_vert;
				fread(&n_vert,sizeof(unsigned short),1,file);
				mesh->alloc_vert(n_vert);
				VertexPNT* dest=mesh->get_write_pointer_v();
				for (unsigned short i=0; i<n_vert; i++)
				{
					fread(&(dest->x),sizeof(float),1,file);
					fread(&(dest->z),sizeof(float),1,file);
					fread(&(dest->y),sizeof(float),1,file);
					dest++;
				}
			}
			break;
		case 0x4120:		// Faces Description chunk
			{
				unsigned short n_idx;
				unsigned short face_flags;
				fread(&n_idx,sizeof(unsigned short),1,file);
				mesh->alloc_index(n_idx*3);
				WORD* dest=mesh->get_write_pointer_i();
				for (unsigned short i=0; i<n_idx; i++)
				{
					fread(dest,sizeof(unsigned short),1,file); dest++;
					fread(dest,sizeof(unsigned short),1,file); dest++;
					fread(dest,sizeof(unsigned short),1,file); dest++;
					fread(&face_flags,sizeof(unsigned short),1,file);
				}
			}
			break;
		case 0x4130:		// Faces Material List chunk
			{
				char buffer[512];
				int		i=0;
				char	c;
				do 
				{
					fread(&c,sizeof(unsigned char),1,file);
					buffer[i]=c;
					i++;
				} while ((c!='\0') && (i<20));
				buffer[i+1]=0;
				
				Material* material=get_material(buffer);
				mesh->set_material(material,false);

				WORD n_entries;
				fread(&n_entries,sizeof(WORD),1,file);
				fseek(file,n_entries*sizeof(WORD),SEEK_CUR);
			}
			break;
		case 0x4140:		// Mapping Coordinates List chunk
			{
				unsigned short n_vert;
				fread(&n_vert,sizeof(unsigned short),1,file);
				VertexPNT* dest=mesh->get_write_pointer_v();
				for (unsigned short i=0; i<n_vert; i++)
				{
					fread(&(dest->u),sizeof(float),1,file);
					fread(&(dest->v),sizeof(float),1,file);
					dest->v=1.0f-dest->v;
					dest++;
				}
			}
			break;
		case 0x4600:
			{
				Vec3f	pos;
				fread(&(pos.x),sizeof(float),1,file);
				fread(&(pos.z),sizeof(float),1,file);
				fread(&(pos.y),sizeof(float),1,file);
		
				lgt=new Light;
				lgt->set_name(current_name);
				lgt->_pos=pos;
				lgt->_light.Type=D3DLIGHT_POINT;

				lights->push_back(lgt);
			}
			break;
		case 0x4700:
			{
				Vec3f	pos;
				Vec3f	target;
				float	bank;
				float	lens;

				fread(&(pos.x),sizeof(float),1,file);
				fread(&(pos.z),sizeof(float),1,file);
				fread(&(pos.y),sizeof(float),1,file);
				fread(&(target.x),sizeof(float),1,file);
				fread(&(target.z),sizeof(float),1,file);
				fread(&(target.y),sizeof(float),1,file);
				fread(&bank,sizeof(float),1,file);
				fread(&lens,sizeof(float),1,file);

				Camera* cam=new Camera;
				cam->set_name(current_name);
				cam->set_lookat(pos,target);
				cam->_fov=lens;
				cameras->push_back(cam);
			}
			break;
		// Jump unparsed
		default:
			fseek(file,start_pos+chunk_len,SEEK_SET);
			break;
	}

	unsigned long bytes_read=0;
	unsigned long current_pos=ftell(file);
	while (current_pos<(start_pos+chunk_len))
	{
		load_and_parse_chunk(file,meshes,cameras,lights);
		current_pos=ftell(file);
	}
}

bool load_3ds(const std::string& filename,std::vector<BaseMesh*>* meshes,std::vector<Camera*>* cameras,std::vector<Light*>* lights)
{
	FILE* file=NULL;
	fopen_s(&file,filename.c_str(),"rb");
	if (!file) return false;
	
	while (!feof(file))
	{
		load_and_parse_chunk(file,meshes,cameras,lights);
	}

	for (size_t i=0; i<meshes->size(); i++)
	{
		build_normals_pnt((Mesh<VertexPNT>*)meshes->at(i));
	}

	fclose(file);

	return true;
}

void build_normals_pnt(Mesh<VertexPNT>* mesh)
{
	VertexPNT* v=mesh->get_write_pointer_v();

	VertexPNT* dest=v;
	for (unsigned long i=0; i<mesh->get_vertex_count(); i++)
	{
		dest->nx=dest->ny=dest->nz=0.0f;
		dest++;
	}
	
	WORD*	i_ptr=mesh->get_write_pointer_i();
	Vec3f	n;
	Vec3f	a,b,c;
	Vec3f	v1,v2;
	for (unsigned long i=0; i<mesh->get_index_count()/3; i++)
	{
		// Get triangle normal
		a=Vec3f(v[i_ptr[0]].x,v[i_ptr[0]].y,v[i_ptr[0]].z);
		b=Vec3f(v[i_ptr[1]].x,v[i_ptr[1]].y,v[i_ptr[1]].z);
		c=Vec3f(v[i_ptr[2]].x,v[i_ptr[2]].y,v[i_ptr[2]].z);
		v1=c-a;
		v2=b-a;
		n=cross_product(v1,v2);
		v[i_ptr[0]].nx+=n.x; v[i_ptr[0]].ny+=n.y; v[i_ptr[0]].nz+=n.z; 
		v[i_ptr[1]].nx+=n.x; v[i_ptr[1]].ny+=n.y; v[i_ptr[1]].nz+=n.z; 
		v[i_ptr[2]].nx+=n.x; v[i_ptr[2]].ny+=n.y; v[i_ptr[2]].nz+=n.z; 

		i_ptr+=3;
	}

	dest=v;
	for (unsigned long i=0; i<mesh->get_vertex_count(); i++)
	{
		float d=1.0f/sqrt(dest->nx*dest->nx+dest->ny*dest->ny+dest->nz*dest->nz);
		dest->nx=dest->nx*d;
		dest->ny=dest->ny*d;
		dest->nz=dest->nz*d;
		dest++;
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
GeomMesh* geom_add_quad(GeomMesh* src_mesh,const Vec3f& center,const Vec3f& dir0,const Vec3f& dir1,const unsigned long color,const Rect& uv_rect)
{
	GeomMesh* dest=(src_mesh)?(src_mesh):(new GeomMesh(false));

	Vec3f		normal=cross_product(dir1,dir0); normal.normalize();
	VertexPNCT	vertex;
	vertex.n=normal;
	vertex.color=color;

	size_t base_idx=dest->get_current_vertex_count();
	vertex.p=center-dir0*0.5f+dir1*0.5f; vertex.uv.set(uv_rect.x1,uv_rect.y1); dest->push_vertex(vertex);
	vertex.p=center+dir0*0.5f+dir1*0.5f; vertex.uv.set(uv_rect.x2,uv_rect.y1); dest->push_vertex(vertex);
	vertex.p=center+dir0*0.5f-dir1*0.5f; vertex.uv.set(uv_rect.x2,uv_rect.y2); dest->push_vertex(vertex);
	vertex.p=center-dir0*0.5f-dir1*0.5f; vertex.uv.set(uv_rect.x1,uv_rect.y2); dest->push_vertex(vertex);
	
	dest->push_index(base_idx); dest->push_index(base_idx+2); dest->push_index(base_idx+1);
	dest->push_index(base_idx); dest->push_index(base_idx+3); dest->push_index(base_idx+2);

	return dest;
}

GeomMesh* geom_add_cube(GeomMesh* src_mesh,
						const Vec3f& center,const Vec3f& dir_x,const Vec3f& dir_y,const Vec3f& dir_z,
						const unsigned long color,
						const bool minus_z_enable,const Rect& minus_z_uv,
						const bool plus_z_enable,const Rect& plus_z_uv,
						const bool minus_x_enable,const Rect& minus_x_uv,
						const bool plus_x_enable,const Rect& plus_x_uv,
						const bool minus_y_enable,const Rect& minus_y_uv,
						const bool plus_y_enable,const Rect& plus_y_uv)
{
	GeomMesh* dest=(src_mesh)?(src_mesh):(new GeomMesh(false));

	if (minus_z_enable) geom_add_quad(dest,center-dir_z*0.5f,dir_x,dir_y,color,minus_z_uv);
	if (plus_z_enable) geom_add_quad(dest,center+dir_z*0.5f,-dir_x,dir_y,color,plus_z_uv);
	if (minus_x_enable) geom_add_quad(dest,center-dir_x*0.5f,-dir_z,dir_y,color,minus_x_uv);
	if (plus_x_enable) geom_add_quad(dest,center+dir_x*0.5f,dir_z,dir_y,color,plus_x_uv);
	if (minus_y_enable) geom_add_quad(dest,center-dir_y*0.5f,dir_x,-dir_z,color,minus_y_uv);
	if (plus_y_enable) geom_add_quad(dest,center+dir_y*0.5f,dir_x,dir_z,color,plus_y_uv);

	return dest;
}

GeomMesh* geom_add_spheroid(GeomMesh* src_mesh,
							const Vec3f& center,const Vec3f& dir_x,const Vec3f& dir_y,const Vec3f& dir_z,
							const float min_lat,const float max_lat,const float lat_inc,
							const float min_lon,const float max_lon,const float lon_inc,
							const unsigned long color,const Rect& rect_uv)
{
	GeomMesh* dest=(src_mesh)?(src_mesh):(new GeomMesh(false));

	VertexPNCT	v;
	v.color=color;

	float	lat=min_lat;
	int		lat_count=0;
	int		lon_count=0;

	size_t	base_idx=dest->get_current_vertex_count();

	while (lat<=max_lat)
	{
		lat_count++;

		float lon=min_lon;
		
		lon_count=0;
		
		float a=-cosf(deg_to_rad(lat));
		float ia=sinf(deg_to_rad(lat));

		v.uv.y=(lat-min_lat)/(max_lat-min_lat);

		while (lon<=max_lon)
		{
			lon_count++;

			float s=sinf(deg_to_rad(lon))*ia;
			float c=cosf(deg_to_rad(lon))*ia;

			v.p=dir_x*c*0.5f+dir_y*a*0.5f+dir_z*s*0.5f;
			v.n=v.p; v.n.normalize();
			v.p=v.p+center;
			v.uv.x=(lon-min_lon)/(max_lon-min_lon);
			dest->push_vertex(v);

			lon+=lon_inc;
		}

		lat+=lat_inc;
	}

	int ly=lat_count;
	int lx=lon_count;

	for (int yy=0; yy<ly-1; yy++)
	{
		for (int xx=0; xx<lx-1; xx++)
		{
			dest->push_index(base_idx+yy*lx+xx); dest->push_index(base_idx+yy*lx+xx+1); dest->push_index(base_idx+(yy+1)*lx+xx+1);
			dest->push_index(base_idx+yy*lx+xx); dest->push_index(base_idx+(yy+1)*lx+xx+1); dest->push_index(base_idx+(yy+1)*lx+xx);
		}
	}

	return dest;
}

GeomMesh* geom_add_cap(GeomMesh* src_mesh,
					   const Vec3f& center,const Vec3f& dir_x,const Vec3f& dir_y,const Vec3f& dir_z,
					   const float min_lon,const float max_lon,const float lon_inc,
					   const unsigned long color,const Rect& rect_uv)
{
	GeomMesh* dest=(src_mesh)?(src_mesh):(new GeomMesh(false));

	size_t base_index=dest->get_current_vertex_count();
	// Apex vertex
	VertexPNCT	appex;
	appex.p=center+dir_y;
	appex.n=cross_product(dir_z,dir_x); appex.n.normalize();
	appex.color=color;
	appex.uv.set(rect_uv.x1+(rect_uv.x2-rect_uv.x1)*0.5f,
				 rect_uv.y1+(rect_uv.y2-rect_uv.y1)*0.5f);
	dest->push_vertex(appex);

	// Outlying area
	int			lon_count=0;
	float		lon=min_lon;
	float		su=(rect_uv.x2-rect_uv.x1)*0.5f;
	float		sv=(rect_uv.x2-rect_uv.x1)*0.5f;
	VertexPNCT	v;

	while (lon<=max_lon)
	{
		lon_count++;

		float s=sinf(deg_to_rad(lon));
		float c=cosf(deg_to_rad(lon));

		v.p=center+dir_x*c*0.5f+dir_z*s*0.5f;
		v.n=appex.n;
		v.color=color;
		v.uv.set(s*su,c*sv);
		dest->push_vertex(v);

		lon+=lon_inc;
	}

	int lx=lon_count;

	for (int xx=0; xx<lx-1; xx++)
	{
		dest->push_index(base_index); dest->push_index(base_index+xx+1); dest->push_index(base_index+xx+2); 
	}

	return dest;
}

GeomMesh* geom_add_cylinder(GeomMesh* src_mesh,
							const Vec3f& bottom_center,const Vec3f& axis_x,const Vec3f& dir_y,const Vec3f& axis_z,
							const float bottom_radius,const float top_radius,
							const float y_div,
							const float min_lon,const float max_lon,const float lon_inc,
							const unsigned long color,const Rect& rect_uv)
{
	GeomMesh* dest=(src_mesh)?(src_mesh):(new GeomMesh(false));

	VertexPNCT	v;
	v.color=color;

	float	c1=(bottom_radius-top_radius);
	float	c2=dir_y.norm_squared();
	float	h=sqrt(c1*c1+c2);
	float	ny=c1/h;
	float	sny=sqrtf(c2)/h;

	float	lat=0.0f;
	int		lat_count=0;
	int		lon_count=0;
	Vec3f	inc_y=dir_y/y_div;
	size_t	base_idx=dest->get_current_vertex_count();

	while (lat<=y_div)
	{
		lat_count++;

		float lon=min_lon;

		lon_count=0;

		float t=(lat/y_div);
		float radius=(1.0f-t)*bottom_radius+t*top_radius;
			
		v.uv.y=t;

		while (lon<=max_lon)
		{
			lon_count++;

			float s=sinf(deg_to_rad(lon));
			float c=cosf(deg_to_rad(lon));

			v.p=radius*c*axis_x+radius*s*axis_z;
			v.n=v.p;
			v.n.normalize();
			v.n.set(v.n.x*sny,ny,v.n.z*sny);
			

			v.p=v.p+bottom_center+dir_y*t;
			v.uv.x=(lon-min_lon)/(max_lon-min_lon);
			dest->push_vertex(v);

			lon+=lon_inc;
		}

		lat+=1.0f;
	}

	int ly=lat_count;
	int lx=lon_count;

	for (int yy=0; yy<ly-1; yy++)
	{
		for (int xx=0; xx<lx-1; xx++)
		{
			dest->push_index(base_idx+yy*lx+xx); dest->push_index(base_idx+yy*lx+xx+1); dest->push_index(base_idx+(yy+1)*lx+xx+1);
			dest->push_index(base_idx+yy*lx+xx); dest->push_index(base_idx+(yy+1)*lx+xx+1); dest->push_index(base_idx+(yy+1)*lx+xx);
		}
	}

	return dest;
}
