#pragma once

#include "Defs.h"
#include "Gfx.h"

typedef enum { FX_SOLID=0, FX_TEXTURED=1, FX_TEXTURED_POINTSAMP=2 } MaterialFX;

class Material
{
public:
	Material(void);
	virtual ~Material(void);

	inline	void		set_name(const std::string& name) { _name=name; }
	inline	std::string	get_name() { return _name; }
			void		set();
			void		set_material_color(float r,float g,float b,float a);
			void		set_emissive_color(float r,float g,float b,float a);
	inline	void		set_fx(MaterialFX fx) { _fx=fx; }
	inline	void		set_cull_mode(DWORD cull_mode) { _cull_mode=cull_mode; }
	inline	void		set_texture(long slot,long tex_id) { _textures[slot]=tex_id; }
	inline	void		set_alpha(bool active,D3DBLEND src_blend,D3DBLEND dest_blend)
	{
		_alpha_enable=active;
		_src_blend=src_blend;
		_dest_blend=dest_blend;
	}
	inline	void		set_color_vertex(const bool b,const bool e) { _color_vertex=b; _color_vertex_e=e; }

	// Material properties
	std::string				_name;
	D3DMATERIAL9			_material;
	DWORD					_cull_mode;
	MaterialFX				_fx;
	long					_textures[MAX_TEX];
	bool					_alpha_enable;
	bool					_color_vertex;
	bool					_color_vertex_e;
	D3DBLEND				_src_blend;
	D3DBLEND				_dest_blend;
};

Material*	get_material(const std::string& name);
void		load_material_3ds(FILE* file);