#include "Material.h"
#include "TextureManager.h"
#include "Utils.h"

Material::Material(void)
{
	_cull_mode=D3DCULL_CW;
	_fx=FX_SOLID;
	for (int i=0; i<MAX_TEX; i++) _textures[i]=-1;
	_name="";
	_color_vertex=false;
	_color_vertex_e=false;
	_alpha_enable=false;
	_src_blend=D3DBLEND_SRCALPHA;
	_dest_blend=D3DBLEND_INVSRCALPHA;
}

Material::~Material(void)
{
}

void Material::set()
{
	GFX->get_device()->SetMaterial(&_material);
	GFX->set_renderstate(D3DRS_CULLMODE,_cull_mode);
	if (_color_vertex)
	{
		GFX->set_renderstate(D3DRS_AMBIENTMATERIALSOURCE,D3DMCS_COLOR1);
		GFX->set_renderstate(D3DRS_DIFFUSEMATERIALSOURCE,D3DMCS_COLOR1);
		GFX->set_renderstate(D3DRS_SPECULARMATERIALSOURCE,D3DMCS_MATERIAL);
		if (_color_vertex_e) GFX->set_renderstate(D3DRS_EMISSIVEMATERIALSOURCE,D3DMCS_COLOR2);
		else GFX->set_renderstate(D3DRS_EMISSIVEMATERIALSOURCE,D3DMCS_MATERIAL);
		GFX->set_renderstate(D3DRS_COLORVERTEX,TRUE);
	}
	else
	{
		GFX->set_renderstate(D3DRS_AMBIENTMATERIALSOURCE,D3DMCS_MATERIAL);
		GFX->set_renderstate(D3DRS_DIFFUSEMATERIALSOURCE,D3DMCS_MATERIAL);
		GFX->set_renderstate(D3DRS_SPECULARMATERIALSOURCE,D3DMCS_MATERIAL);
		GFX->set_renderstate(D3DRS_EMISSIVEMATERIALSOURCE,D3DMCS_MATERIAL);
		GFX->set_renderstate(D3DRS_COLORVERTEX,FALSE);
	}
	if (_alpha_enable)
	{
		GFX->set_renderstate(D3DRS_ALPHABLENDENABLE,1);
		GFX->set_renderstate(D3DRS_SRCBLEND,_src_blend);
		GFX->set_renderstate(D3DRS_DESTBLEND,_dest_blend);
	}
	else
	{
		GFX->set_renderstate(D3DRS_ALPHABLENDENABLE,0);
	}

	if (_fx==FX_SOLID)
	{
		for (int i=0; i<MAX_TEX; i++)
		{
			GFX->set_texture(i,NULL);
		}
	}
	if ((_fx==FX_TEXTURED) || (_fx==FX_TEXTURED_POINTSAMP))
	{
		long stage=0;
		for (int i=0; i<MAX_TEX; i++)
		{
			if (_textures[i]!=-1)
			{
				LPDIRECT3DTEXTURE9 tex=TEXTURES->get_texture(_textures[i]);
				GFX->set_texture(stage,tex);

				if (_fx==FX_TEXTURED)
				{
					GFX->set_sampler_state(stage,D3DSAMP_MINFILTER,D3DTEXF_LINEAR);
					GFX->set_sampler_state(stage,D3DSAMP_MAGFILTER,D3DTEXF_LINEAR);
					GFX->set_sampler_state(stage,D3DSAMP_MIPFILTER,D3DTEXF_LINEAR);
				}
				else
				{
					GFX->set_sampler_state(stage,D3DSAMP_MINFILTER,D3DTEXF_POINT);
					GFX->set_sampler_state(stage,D3DSAMP_MAGFILTER,D3DTEXF_POINT);
					GFX->set_sampler_state(stage,D3DSAMP_MIPFILTER,D3DTEXF_LINEAR);
				}

				stage++;
			}
		}
		GFX->set_texture_stage_state(0,D3DTSS_COLOROP,D3DTOP_MODULATE);
		GFX->set_texture_stage_state(0,D3DTSS_COLORARG1,D3DTA_TEXTURE);
		GFX->set_texture_stage_state(0,D3DTSS_COLORARG2,D3DTA_DIFFUSE);
		GFX->set_texture_stage_state(0,D3DTSS_ALPHAOP,D3DTOP_MODULATE);
		GFX->set_texture_stage_state(0,D3DTSS_ALPHAARG1,D3DTA_TEXTURE);
		GFX->set_texture_stage_state(0,D3DTSS_ALPHAARG2,D3DTA_DIFFUSE);
		GFX->set_texture_stage_state(0,D3DTSS_TEXCOORDINDEX,0);

	}
}

void Material::set_material_color(float r,float g,float b,float a)
{
	_material.Ambient.r=r; _material.Ambient.g=g; _material.Ambient.b=b; _material.Ambient.a=a; 
	_material.Diffuse.r=r; _material.Diffuse.g=g; _material.Diffuse.b=b; _material.Diffuse.a=a; 
	_material.Specular.r=0.0f; _material.Specular.g=0.0f; _material.Specular.b=0.0f; _material.Specular.a=0.0f; 
	_material.Emissive.r=0.0f; _material.Emissive.g=0.0f; _material.Emissive.b=0.0f; _material.Emissive.a=0.0f; 
	_material.Power=0.0f;
}

void Material::set_emissive_color(float r,float g,float b,float a)
{
	_material.Emissive.r=r; _material.Emissive.g=g; _material.Emissive.b=b; _material.Emissive.a=a; 
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////
// Material managment
////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////

std::vector<Material*>	g_materials;
Material*				g_current_material_3ds=NULL;

Material* get_material(const std::string& name)
{
	for (size_t i=0; i<g_materials.size(); i++)
	{
		if (name==g_materials[i]->get_name()) return g_materials[i];
	}
	return NULL;
}

void load_material_3ds(FILE* file)
{
	unsigned short	chunk_id;
	unsigned long	chunk_len;
	unsigned long	start_pos=ftell(file);

	fread (&chunk_id,sizeof(unsigned short),1,file); //Read the chunk header
	if (feof(file)) return;
	fread (&chunk_len,sizeof(unsigned long),1,file); //Read the length of the chunk 
	if (feof(file)) return;

	switch (chunk_id)
	{
		// Ignore chunks
		case 0xAFFF:		// Material chunk (parent)
		case 0xA200:		// Texture Map 1 chunk (parent)
		case 0xA020:		// Diffuse Color chunk
			break;
		// Interesting chunks
		case 0xA000:		// Material Name chunk
			{
				g_current_material_3ds=new Material;
				g_current_material_3ds->set_material_color(1.0f,1.0f,1.0f,1.0f);
				g_current_material_3ds->set_fx(FX_SOLID);
				g_current_material_3ds->set_cull_mode(D3DCULL_CW);
				g_materials.push_back(g_current_material_3ds);

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
				g_current_material_3ds->set_name(buffer);
			}
			break;
		case 0xA300:		// Texture Name chunk
			{
				char	buffer[512];
				int		i=0;
				char	c;
				do 
				{
					fread(&c,sizeof(unsigned char),1,file);
					buffer[i]=c;
					i++;
				} while ((c!='\0') && (i<20));
				buffer[i+1]=0;		
				// Find texture
				std::string s=buffer;
				if (!file_exists(s.c_str()))
				{
					s="textures/"+s;
				}
				long tex_id=TEXTURES->load(buffer,s.c_str());
				if (tex_id!=-1)
				{
					g_current_material_3ds->set_fx(FX_TEXTURED);
					g_current_material_3ds->set_texture(0,tex_id);
				}
			}
			break;
		case 0x0011:		// RGB chunk
			{
				unsigned char r, g, b;
				fread(&r,sizeof(unsigned char),1,file);
				fread(&g,sizeof(unsigned char),1,file);
				fread(&b,sizeof(unsigned char),1,file);
				g_current_material_3ds->set_material_color((float)r/255.0f,(float)g/255.0f,(float)b/255.0f,1.0f);
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
		load_material_3ds(file);
		current_pos=ftell(file);
	}
}