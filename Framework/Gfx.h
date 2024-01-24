// Gfx.h: interface for the Gfx class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GFX_H__65AF6A21_DD51_4626_9C30_4141D1E621D0__INCLUDED_)
#define AFX_GFX_H__65AF6A21_DD51_4626_9C30_4141D1E621D0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <string>
#include "Defs.h"
#include "dx/dxutil.h"
#include "dx/d3denumeration.h"
#include "dx/d3dsettings.h"

#define MAX_TEX	8

class Gfx  
{
public:
	Gfx();
	virtual ~Gfx();

	// Init stuff and config stuff
			bool				init(HINSTANCE instance);
			void				shutdown();
	inline	void				set_window_mode(bool b) { _window_mode=b; }
	inline	bool				is_windowed() { return _window_mode; }
	inline	void				set_resolution(long rx,long ry) { _resolution.set(float(rx),float(ry)); }
	inline	Vec2f				get_resolution() { return _resolution; }
	inline	void				set_app_name(const std::string& str) { _app_name=str; }
	inline	void				set_window(HWND window) { _window=window; }
	inline	HWND				get_window() { return _window; }
	inline	LPDIRECT3DDEVICE9	get_device() { return _device; }
	inline	unsigned long		get_window_style() { return _window_style; }

	// Real gfx stuff
			void		set_perspective(float fov,float aspect,float near_plane,float far_plane);
			void		set_2d_render(float zoom=1.0f);
			void		set_camera(float x,float y,float z);
			void		set_camera(float x,float y,float z,Quaternion rot);
			void		set_world_transformation(int idx,float x,float y,float z,Quaternion rot);
			void		clear(DWORD clear_buffers,float r=0.0f,float g=0.0f,float b=0.0f,float a=1.0f,float zVal=1.0f,unsigned long sVal=0);
			void		begin_scene();
			void		end_scene();
			void		swap_buffers();
	inline	void		set_color_modulation(float r,float g,float b) { _modulate.x=r; _modulate.y=g; _modulate.z=b; }
			Vec3f		get_color_modulation() { return _modulate; }
			void		set_light(UINT idx,D3DLIGHT9* l);
			void		clear_lights();
	inline	void		set_renderstate(D3DRENDERSTATETYPE rs,DWORD val)
	{
		if (_renderstate[rs]==val) return;
		_renderstate[rs]=val;
		_device->SetRenderState(rs,val);
	}
	inline	void		set_vertex_shader(LPDIRECT3DVERTEXSHADER9 shader)
	{
		if (shader==_vertex_shader) return;
		_vertex_shader=shader;
		_device->SetVertexShader(shader);
	}
	inline	void		set_pixel_shader(LPDIRECT3DPIXELSHADER9 shader)
	{
		if (shader==_pixel_shader) return;
		_pixel_shader=shader;
		_device->SetPixelShader(shader);
	}
	inline	void		set_fvf(DWORD fvf)
	{
		if (fvf==_fvf) return;
		_fvf=fvf;
		_device->SetFVF(fvf);
	}
	inline	void		set_vertex_declaration(LPDIRECT3DVERTEXDECLARATION9 decl)
	{
		if (decl==_vdecl) return;
		_vdecl=decl;
		_device->SetVertexDeclaration(decl);
	}
	inline	void		set_stream_source(UINT n,LPDIRECT3DVERTEXBUFFER9 buffer,UINT offset,UINT stride)
	{
		_device->SetStreamSource(n,buffer,offset,stride);
	}
	inline	void		set_indices(LPDIRECT3DINDEXBUFFER9 buffer)
	{
		_device->SetIndices(buffer);
	}
	inline	void		set_texture(UINT stage,LPDIRECT3DTEXTURE9 tex)
	{
		if (_texture[stage]==tex) return;
		_texture[stage]=tex;
		_device->SetTexture(stage,tex);
	}
	inline	void		set_texture_stage_state(UINT stage,D3DTEXTURESTAGESTATETYPE r,DWORD val)
	{
		if (_rss[stage][r]==val) return;
		_rss[stage][r]=val;
		_device->SetTextureStageState(stage,r,val);
	}
	inline	void		set_sampler_state(UINT slot,D3DSAMPLERSTATETYPE state,DWORD val)
	{
		if (_sampler[slot][state]==val) return;
		_sampler[slot][state]=val;
		_device->SetSamplerState(slot,state,val);
	}

			Vec3f		transform_vertex(const float fov,const float aspect,const float near_plane,const float far_plane,
										 const float cam_x,const float cam_y,const float cam_z,const Quaternion& rot,
										 const Vec3f& v);

public:
			void		set_cursor_position(long x,long y);

protected:
    static	bool		confirm_device_helper(D3DCAPS9* pCaps,VertexProcessingType vertexProcessingType,D3DFORMAT backBufferFormat,D3DFORMAT format);
			HRESULT		choose_initial_settings();
			HRESULT		init_d3d_environment();
			HRESULT		confirm_device(D3DCAPS9* pCaps,DWORD dwBehavior,D3DFORMAT Format);
			bool		find_best_fullscreen_mode(bool bRequireHAL,bool bRequireREF);
			bool		find_best_windowed_mode(bool bRequireHAL,bool bRequireREF);
			void		build_present_params_from_settings();

protected:
	bool							_window_mode;
	Vec2f							_resolution;
	Vec3f							_modulate;
protected:
    LPDIRECT3D9						_d3d9;          // The main D3D object
    LPDIRECT3DDEVICE9				_device;        // The D3D rendering device
    CD3DEnumeration					_d3d_enumeration;
	HWND							_window;	
	CD3DSettings					_d3d_settings;
	D3DPRESENT_PARAMETERS			_d3dpp;			   // Parameters for CreateDevice/Reset
    DWORD							_window_style;     // Saved window style for mode switches
	RECT							_window_bounds;    // Saved window bounds for mode switches
    RECT							_window_client;    // Saved client area size for mode switches
    DWORD							_create_flags;     // Indicate sw or hw vertex processing
    D3DCAPS9						_caps;			   // Caps for the device
    D3DSURFACE_DESC					_back_buffer_desc;   // Surface desc of the backbuffer
	std::string						_app_name;
	DWORD							_renderstate[256];
	LPDIRECT3DVERTEXSHADER9			_vertex_shader;
	LPDIRECT3DPIXELSHADER9			_pixel_shader;
	DWORD							_fvf;
	LPDIRECT3DVERTEXDECLARATION9	_vdecl;
	LPDIRECT3DTEXTURE9				_texture[MAX_TEX];
	DWORD							_rss[MAX_TEX][256];
	DWORD							_sampler[MAX_TEX][256];
	bool							_in_scene;
};

extern Gfx*		GFX;

#endif // !defined(AFX_GFX_H__65AF6A21_DD51_4626_9C30_4141D1E621D0__INCLUDED_)
