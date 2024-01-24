// TextureManager.h: interface for the TextureManager class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TEXTUREMANAGER_H__F26661F1_5CBF_43D8_AD3B_4333CD88A491__INCLUDED_)
#define AFX_TEXTUREMANAGER_H__F26661F1_5CBF_43D8_AD3B_4333CD88A491__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <d3d9.h>
#include <vector>

class Image_A8R8G8B8;

class TextureManager  
{
protected:
	struct Texture
	{
		LPDIRECT3DTEXTURE9	texture;
		char*				filename;
		char*				name;
		int					tx,ty;
		int					sx,sy;
	};
	typedef std::vector<Texture*>	Textures;
public:
	TextureManager(LPDIRECT3DDEVICE9 device);
	virtual ~TextureManager();

	void				clear(const char* name);
	void				clear_all();
	long				load(const char *name,const char *filename);
	long				load(const char *name,Image_A8R8G8B8* img);
	long				add(const char *name,LPDIRECT3DTEXTURE9 texture);
	long				find_by_name(const char *name);
	int					get_size_x(long id);
	int					get_size_y(long id);
	int					get_texture_size_x(long id);
	int					get_texture_size_y(long id);
	std::string			get_texture_name(long id);
	LPDIRECT3DTEXTURE9	get_texture(long id);

protected:
	LPDIRECT3DDEVICE9	_device;
	Textures			_textures;
};

extern TextureManager* TEXTURES;

#endif // !defined(AFX_TEXTUREMANAGER_H__F26661F1_5CBF_43D8_AD3B_4333CD88A491__INCLUDED_)
