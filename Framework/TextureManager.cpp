// TextureManager.cpp: implementation of the TextureManager class.
//
//////////////////////////////////////////////////////////////////////

#include "TextureManager.h"
#include "Utils.h"
#include "Image_A8R8G8B8.h"
#include <d3dx9.h>

TextureManager* TEXTURES=NULL;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

TextureManager::TextureManager(LPDIRECT3DDEVICE9 device)
{
	_device=device;
	if (!TEXTURES) TEXTURES=this;
}

TextureManager::~TextureManager()
{
	clear_all();
	if (TEXTURES==this) TEXTURES=NULL;
}

long TextureManager::load(const char *name,const char *filename)
{
	LPDIRECT3DTEXTURE9	texture;

	FILE *file=NULL;
	fopen_s(&file,filename,"rb");
	if (!file)
	{
		char buffer[512];
		char working_dir[512];
		GetCurrentDirectory(512,(char*)&working_dir);
		sprintf_s((char*)&buffer,512,"File not found (%s/%s)!",(char*)&working_dir,filename);
		ERROR_MSG((char*)&buffer);
		return -1;
	}
	fclose(file);
	D3DXIMAGE_INFO info;
	HRESULT hRet=D3DXCreateTextureFromFileEx(_device,filename,D3DX_DEFAULT,D3DX_DEFAULT,D3DX_DEFAULT,
											 0,D3DFMT_UNKNOWN, D3DPOOL_MANAGED, D3DX_DEFAULT, D3DX_DEFAULT, 0, &info, NULL,
											 &texture);
	if (hRet!=D3D_OK)
	{
		ERROR_MSG("Can't read texture!");
		return -1;
	}

	Texture* tex=new Texture;
	tex->filename=_strdup(filename);
	if (name) tex->name=_strdup(name); else tex->name=NULL;
	tex->texture=texture;

	D3DSURFACE_DESC	desc;
	texture->GetLevelDesc(0,&desc);

	tex->tx=desc.Width;
	tex->ty=desc.Height;
	tex->sx=info.Width;
	tex->sy=info.Height;

	_textures.push_back(tex);

	return (long)_textures.size()-1;
}

long TextureManager::load(const char *name,Image_A8R8G8B8* img)
{
	// Create texture
	LPDIRECT3DTEXTURE9	texture=NULL;

	if (_device->CreateTexture((UINT)img->get_size_x(),(UINT)img->get_size_y(),
							   1,0,D3DFMT_A8R8G8B8,D3DPOOL_MANAGED,&texture,NULL)!=D3D_OK)
	{
		return -1;
	}

	// Fill texture
	D3DLOCKED_RECT info;
	texture->LockRect(0,&info,NULL,0);

	unsigned char* src=img->get_data();
	unsigned char* dest=(unsigned char*)info.pBits;

	for (size_t y=0; y<img->get_size_y(); y++)
	{
		memcpy(dest,src,img->get_size_x()*4);
		src+=img->get_size_x()*4;
		dest+=info.Pitch;
	}

	texture->UnlockRect(0);

	// Add texture
	Texture* tex=new Texture;
	tex->filename=NULL;
	if (name) tex->name=_strdup(name); else tex->name=NULL;
	tex->texture=texture;

	tex->tx=(int)img->get_size_x();
	tex->ty=(int)img->get_size_y();
	tex->sx=(int)img->get_size_x();
	tex->sy=(int)img->get_size_y();

	_textures.push_back(tex);

	return (long)_textures.size()-1;
}

long TextureManager::add(const char *name,LPDIRECT3DTEXTURE9 texture)
{
	Texture* tex=new Texture;
	tex->filename=NULL;
	if (name) tex->name=_strdup(name); else tex->name=NULL;
	tex->texture=texture;

	D3DSURFACE_DESC	desc;
	texture->GetLevelDesc(0,&desc);

	tex->sx=desc.Width;
	tex->sy=desc.Height;

	_textures.push_back(tex);

	return (long)_textures.size()-1;
}

long TextureManager::find_by_name(const char *name)
{
	for (unsigned long i=0; i<_textures.size(); i++)
	{
		if (_textures[i]) 
		{
			if (_textures[i]->name)
			{
				if (!_stricmp(_textures[i]->name,name)) return i;
			}
		}
	}

	return -1;
}

void TextureManager::clear(const char* name)
{
	for (unsigned long i=0; i<_textures.size(); i++)
	{
		if (_textures[i]) 
		{
			if (!_stricmp(_textures[i]->name,name))
			{
				free(_textures[i]->filename);
				_textures[i]->texture->Release();
				delete _textures[i];
				_textures[i]=NULL;
				return;
			}
		}
	}
}

void TextureManager::clear_all()
{
	for (unsigned long i=0; i<_textures.size(); i++)
	{
		if (_textures[i]) 
		{
			free(_textures[i]->filename);
			_textures[i]->texture->Release();
			delete _textures[i];
		}
	}
	_textures.clear();
}

LPDIRECT3DTEXTURE9 TextureManager::get_texture(long id)
{
	if (id==-1) return NULL;
	if (_textures[id])
	{
		return _textures[id]->texture;
	}

	return NULL;
}

int	TextureManager::get_size_x(long id)
{
	if (id==-1) return 0;
	if (_textures[id]) return _textures[id]->sx;

	return 0;
}

int	TextureManager::get_size_y(long id)
{
	if (id==-1) return 0;
	if (_textures[id]) return _textures[id]->sy;

	return 0;
}

int	TextureManager::get_texture_size_x(long id)
{
	if (id==-1) return 0;
	if (_textures[id]) return _textures[id]->tx;

	return 0;
}

int	TextureManager::get_texture_size_y(long id)
{
	if (id==-1) return 0;
	if (_textures[id]) return _textures[id]->ty;

	return 0;
}

std::string	TextureManager::get_texture_name(long id)
{
	if (id==-1) return "";
	if (_textures[id]) return _textures[id]->name;

	return "";
}
