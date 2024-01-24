// Gfx.cpp: implementation of the Gfx class.
//
//////////////////////////////////////////////////////////////////////

#include "Gfx.h"
#include "Defs.h"
#include "Screen.h"
#include <stdlib.h>
#include "Utils.h"
#include "Logger.h"

Gfx*		GFX=NULL;

LRESULT CALLBACK WndProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	switch (uMsg)
	{
		case WM_ACTIVATE:
		case WM_ACTIVATEAPP:
			{
				MouseEventCallback	callback=get_mouse_event_callback();
				if (callback)
				{
					callback(uMsg,(unsigned long)wParam,(unsigned long)lParam);
				}
			}
			return 0L;
		case WM_CLOSE:
		case WM_DESTROY:
			// Clean up and close the app
			if (SCREEN)
			{
				SCREEN->set_exit(true,-1);
			}
			return 0L;
		case WM_LBUTTONUP:
		case WM_LBUTTONDOWN:
		case WM_RBUTTONUP:
		case WM_RBUTTONDOWN:
			{
				MouseEventCallback	callback=get_mouse_event_callback();
				if (callback)
				{
					callback(uMsg,(unsigned long)wParam,(unsigned long)lParam);
				}
			}
		case WM_ENTERSIZEMOVE:
			// Halt frame movement while the app is sizing or moving
			return 0;
			break;
		case WM_EXITSIZEMOVE:
			return 0;
			break;
		case WM_SETCURSOR:
			SetCursor(NULL);
			return TRUE;
	}

	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Gfx::Gfx()
{
	if (!GFX) GFX=this;
	set_window_mode(false);
	set_resolution(1024,768);
	set_window(NULL);
	set_color_modulation(1.0f,1.0f,1.0f); 
	set_app_name("Game");
	for (int i=0; i<256; i++)
	{
		_renderstate[i]=0xFFFFFFFF;
	}
	_renderstate[D3DRS_AMBIENT]=0;
	for (int j=0; j<MAX_TEX; j++)
	{
		for (int i=0; i<256; i++)
		{
			_rss[j][i]=0xFFFFFFFF;
			_sampler[j][i]=0xFFFFFFFF;
		}
	}
	for (int i=0; i<MAX_TEX; i++) _texture[i]=NULL;
	_vertex_shader=(LPDIRECT3DVERTEXSHADER9)0x123456;
	_pixel_shader=(LPDIRECT3DPIXELSHADER9)0x123456;
	_vdecl=(LPDIRECT3DVERTEXDECLARATION9)0x123456;
	_fvf=0;
	_in_scene=false;
}

Gfx::~Gfx()
{
	if (GFX==this) GFX=NULL;
	shutdown();
}

bool Gfx::init(HINSTANCE instance)
{
    HRESULT hr;

    // Create the Direct3D object
    _d3d9=Direct3DCreate9(D3D_SDK_VERSION);
    if (_d3d9==NULL)
	{
        MessageBox(NULL,"Error creating D3D9 device!",_app_name.c_str(),MB_ICONERROR|MB_OK);
		return false;
	}

    // Build a list of Direct3D adapters, modes and devices. The
    // ConfirmDevice() callback is used to confirm that only devices that
    // meet the app's requirements are considered.
	_d3d_enumeration.AppUsesDepthBuffer=TRUE;
    _d3d_enumeration.SetD3D(_d3d9);
    _d3d_enumeration.ConfirmDeviceCallback=confirm_device_helper;
    if (FAILED(hr=_d3d_enumeration.Enumerate()))
    {
        MessageBox(NULL,"Can't enumerate device!",_app_name.c_str(),MB_ICONERROR|MB_OK);
        SAFE_RELEASE(_d3d9);
        return false;
    }

    // Register the windows class
    WNDCLASS wndClass = { 0, WndProc, 0, 0, instance,
                          LoadIcon( instance, MAKEINTRESOURCE(IDI_ASTERISK)),
                          LoadCursor( instance, MAKEINTRESOURCE(IDC_ARROW) ),
                          (HBRUSH)GetStockObject(WHITE_BRUSH),
                          NULL, _app_name.c_str() };
    RegisterClass(&wndClass);

    // Set the window's initial style
	DWORD dwWindowStyle;
	if (_window_mode)
	{
		dwWindowStyle=WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_THICKFRAME | 
                     WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_VISIBLE;
	}
	else
	{
		dwWindowStyle=WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_POPUPWINDOW | WS_VISIBLE;
	}
//    DWORD dwWindowStyle=WS_POPUP|WS_VISIBLE;

    // Set the window's initial width
    RECT rc;
    SetRect(&rc,0,0,int(_resolution.x),int(_resolution.y));
    AdjustWindowRect(&rc,dwWindowStyle,false);

    // Create the render window
    _window= CreateWindow(_app_name.c_str(),_app_name.c_str(),dwWindowStyle,
						  CW_USEDEFAULT,CW_USEDEFAULT,
						  (rc.right-rc.left),(rc.bottom-rc.top),0,
                          NULL,instance,0);

    // Save window properties
    _window_style=GetWindowLong(_window,GWL_STYLE);
    GetWindowRect(_window,&_window_bounds);
    GetClientRect(_window,&_window_client);

	ShowWindow(_window,SW_SHOW);
	SetFocus(_window);
	ClipCursor(NULL);
	SetCursor(NULL);

    if (FAILED(hr=choose_initial_settings()))
    {
        MessageBox(NULL,"Can't choose initial D3D settings!",_app_name.c_str(),MB_ICONERROR|MB_OK);
        SAFE_RELEASE(_d3d9);
        return false;
    }

    // Initialize the 3D environment for the app
    if (FAILED(hr=init_d3d_environment()))
    {
        SAFE_RELEASE(_d3d9);
        MessageBox(NULL,"Can't initialize D3D environment!",_app_name.c_str(),MB_ICONERROR|MB_OK);
        return false;
    }

	set_renderstate(D3DRS_ALPHABLENDENABLE,TRUE);
	set_renderstate(D3DRS_LIGHTING,1);

    return true;
}

void Gfx::shutdown()
{
	if (_vertex_shader) _device->SetVertexShader(NULL);
	if (_pixel_shader) _device->SetPixelShader(NULL);
	if (_vdecl) _device->SetVertexDeclaration(NULL);
	for (int i=0; i<MAX_TEX; i++)
	{
		_device->SetTexture(i,NULL);
	}
	SAFE_RELEASE(_device);
	SAFE_RELEASE(_d3d9);
}

bool Gfx::confirm_device_helper(D3DCAPS9* pCaps,VertexProcessingType vertexProcessingType, 
							   D3DFORMAT backBufferFormat,D3DFORMAT format)
{
    DWORD dwBehavior;

    if (vertexProcessingType == SOFTWARE_VP)
        dwBehavior = D3DCREATE_SOFTWARE_VERTEXPROCESSING;
    else if (vertexProcessingType == MIXED_VP)
        dwBehavior = D3DCREATE_MIXED_VERTEXPROCESSING;
    else if (vertexProcessingType == HARDWARE_VP)
        dwBehavior = D3DCREATE_HARDWARE_VERTEXPROCESSING;
    else if (vertexProcessingType == PURE_HARDWARE_VP)
        dwBehavior = D3DCREATE_HARDWARE_VERTEXPROCESSING | D3DCREATE_PUREDEVICE;
    else
        dwBehavior = 0; // TODO: throw exception
    
    return SUCCEEDED(GFX->confirm_device(pCaps,dwBehavior,backBufferFormat));
}

void Gfx::set_cursor_position(long x,long y)
{
	_device->SetCursorPosition(x,y,0);
}

HRESULT Gfx::choose_initial_settings()
{
    bool bFoundFullscreen=find_best_fullscreen_mode(false,false);
    bool bFoundWindowed=find_best_windowed_mode(false,false);

    if (!_window_mode && bFoundFullscreen) _d3d_settings.IsWindowed=false;
    if (!bFoundWindowed && bFoundFullscreen) _d3d_settings.IsWindowed=false;
    if (!bFoundFullscreen && !bFoundWindowed) return E_FAIL;

    return S_OK;
}

HRESULT Gfx::init_d3d_environment()
{
    HRESULT hr;

    D3DAdapterInfo* pAdapterInfo = _d3d_settings.PAdapterInfo();
    D3DDeviceInfo* pDeviceInfo = _d3d_settings.PDeviceInfo();

    _window_mode=_d3d_settings.IsWindowed;

    // Set up the presentation parameters
	build_present_params_from_settings();

    if( pDeviceInfo->Caps.PrimitiveMiscCaps & D3DPMISCCAPS_NULLREFERENCE )
    {
        // Warn user about null ref device that can't render anything
		MessageBox(NULL,"No device found!",_app_name.c_str(),MB_ICONERROR|MB_OK);
    }

    DWORD behaviorFlags;
    if (_d3d_settings.GetVertexProcessingType() == SOFTWARE_VP)
        behaviorFlags = D3DCREATE_SOFTWARE_VERTEXPROCESSING;
    else if (_d3d_settings.GetVertexProcessingType() == MIXED_VP)
        behaviorFlags = D3DCREATE_MIXED_VERTEXPROCESSING;
    else if (_d3d_settings.GetVertexProcessingType() == HARDWARE_VP)
        behaviorFlags = D3DCREATE_HARDWARE_VERTEXPROCESSING;
    else if (_d3d_settings.GetVertexProcessingType() == PURE_HARDWARE_VP)
        behaviorFlags = D3DCREATE_HARDWARE_VERTEXPROCESSING | D3DCREATE_PUREDEVICE;
    else
        behaviorFlags = 0; // TODO: throw exception

    // Create the device
	_d3dpp.PresentationInterval=D3DPRESENT_INTERVAL_ONE;
//	_d3dpp.PresentationInterval=D3DPRESENT_INTERVAL_IMMEDIATE;
//	_d3dpp.Flags=0;
//	_d3dpp.AutoDepthStencilFormat=D3DFMT_D24S8;
    hr = _d3d9->CreateDevice( _d3d_settings.AdapterOrdinal(), pDeviceInfo->DevType,
                               _window, behaviorFlags, &_d3dpp,
                               &_device );

    if( SUCCEEDED(hr) )
    {
        // When moving from fullscreen to windowed mode, it is important to
        // adjust the window size after recreating the device rather than
        // beforehand to ensure that you get the window size you want.  For
        // example, when switching from 640x480 fullscreen to windowed with
        // a 1000x600 window on a 1024x768 desktop, it is impossible to set
        // the window size to 1000x600 until after the display mode has
        // changed to 1024x768, because windows cannot be larger than the
        // desktop.

	    if( _window_mode )
        {
            SetWindowPos( _window, HWND_NOTOPMOST,
                          _window_bounds.left, _window_bounds.top,
                          ( _window_bounds.right - _window_bounds.left ),
                          ( _window_bounds.bottom - _window_bounds.top ),
                          SWP_SHOWWINDOW );
        }

		if (!_window_mode)
		{
			MSG msg;
			unsigned long t0=timeGetTime();
			while ((timeGetTime()-t0)<1000)
			{
				while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
				{
					TranslateMessage(&msg);
					DispatchMessage(&msg);
				}
			}

			_device->Reset(&_d3dpp);
		}

/*		_device->BeginScene();
		_device->Clear(0,NULL,D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER,0xFFFF0000,1.0f,0);
		_device->EndScene();
		swap_buffers();
		_device->BeginScene();
		_device->Clear(0,NULL,D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER,0xFF00FF00,1.0f,0);
		_device->EndScene();
		swap_buffers();//*/

		// Store device Caps
        _device->GetDeviceCaps(&_caps);
        _create_flags=behaviorFlags;

        // Store render target surface desc
        LPDIRECT3DSURFACE9 pBackBuffer = NULL;
        _device->GetBackBuffer( 0, 0, D3DBACKBUFFER_TYPE_MONO, &pBackBuffer );
        pBackBuffer->GetDesc(&_back_buffer_desc);
        pBackBuffer->Release();

		return S_OK;
    }

    // If that failed, fall back to the reference rasterizer
    if (pDeviceInfo->DevType==D3DDEVTYPE_HAL)
    {
        if (find_best_windowed_mode(false,true))
        {
            _window_mode = true;
            // Make sure main window isn't topmost, so error message is visible
            SetWindowPos( _window, HWND_NOTOPMOST,
                          _window_bounds.left, _window_bounds.top,
                          ( _window_bounds.right - _window_bounds.left ),
                          ( _window_bounds.bottom - _window_bounds.top ),
                          SWP_SHOWWINDOW );

            // Let the user know we are switching from HAL to the reference rasterizer
            MessageBox(NULL,"No HAL, switching to REF!",_app_name.c_str(),MB_ICONERROR|MB_OK);

            hr=init_d3d_environment();
        }
    }
    return hr;
}

HRESULT Gfx::confirm_device(D3DCAPS9* pCaps,DWORD dwBehavior,D3DFORMAT Format)
{
    return S_OK;
}

bool Gfx::find_best_windowed_mode(bool bRequireHAL,bool bRequireREF)
{
    // Get display mode of primary adapter (which is assumed to be where the window 
    // will appear)
    D3DDISPLAYMODE primaryDesktopDisplayMode;
    _d3d9->GetAdapterDisplayMode(0, &primaryDesktopDisplayMode);

    D3DAdapterInfo* pBestAdapterInfo = NULL;
    D3DDeviceInfo* pBestDeviceInfo = NULL;
    D3DDeviceCombo* pBestDeviceCombo = NULL;

    for( UINT iai = 0; iai < _d3d_enumeration.m_pAdapterInfoList->Count(); iai++ )
    {
        D3DAdapterInfo* pAdapterInfo = (D3DAdapterInfo*)_d3d_enumeration.m_pAdapterInfoList->GetPtr(iai);
        for( UINT idi = 0; idi < pAdapterInfo->pDeviceInfoList->Count(); idi++ )
        {
            D3DDeviceInfo* pDeviceInfo = (D3DDeviceInfo*)pAdapterInfo->pDeviceInfoList->GetPtr(idi);
            if (bRequireHAL && pDeviceInfo->DevType != D3DDEVTYPE_HAL)
                continue;
            if (bRequireREF && pDeviceInfo->DevType != D3DDEVTYPE_REF)
                continue;
            for( UINT idc = 0; idc < pDeviceInfo->pDeviceComboList->Count(); idc++ )
            {
                D3DDeviceCombo* pDeviceCombo = (D3DDeviceCombo*)pDeviceInfo->pDeviceComboList->GetPtr(idc);
                bool bAdapterMatchesBB = (pDeviceCombo->BackBufferFormat == pDeviceCombo->AdapterFormat);
                if (!pDeviceCombo->IsWindowed)
                    continue;
                if (pDeviceCombo->AdapterFormat != primaryDesktopDisplayMode.Format)
                    continue;
                // If we haven't found a compatible DeviceCombo yet, or if this set
                // is better (because it's a HAL, and/or because formats match better),
                // save it
                if( pBestDeviceCombo == NULL || 
                    pBestDeviceCombo->DevType != D3DDEVTYPE_HAL && pDeviceCombo->DevType == D3DDEVTYPE_HAL ||
                    pDeviceCombo->DevType == D3DDEVTYPE_HAL && bAdapterMatchesBB )
                {
                    pBestAdapterInfo = pAdapterInfo;
                    pBestDeviceInfo = pDeviceInfo;
                    pBestDeviceCombo = pDeviceCombo;
                    if( pDeviceCombo->DevType == D3DDEVTYPE_HAL && bAdapterMatchesBB )
                    {
                        // This windowed device combo looks great -- take it
                        goto EndWindowedDeviceComboSearch;
                    }
                    // Otherwise keep looking for a better windowed device combo
                }
            }
        }
    }
EndWindowedDeviceComboSearch:
    if (pBestDeviceCombo == NULL )
        return false;

    _d3d_settings.pWindowed_AdapterInfo = pBestAdapterInfo;
    _d3d_settings.pWindowed_DeviceInfo = pBestDeviceInfo;
    _d3d_settings.pWindowed_DeviceCombo = pBestDeviceCombo;
    _d3d_settings.IsWindowed = true;
    _d3d_settings.Windowed_DisplayMode = primaryDesktopDisplayMode;
    _d3d_settings.Windowed_Width = _window_client.right - _window_client.left;
    _d3d_settings.Windowed_Height = _window_client.bottom - _window_client.top;
    if (_d3d_enumeration.AppUsesDepthBuffer)
        _d3d_settings.Windowed_DepthStencilBufferFormat = *(D3DFORMAT*)pBestDeviceCombo->pDepthStencilFormatList->GetPtr(0);
    _d3d_settings.Windowed_MultisampleType = *(D3DMULTISAMPLE_TYPE*)pBestDeviceCombo->pMultiSampleTypeList->GetPtr(0);
    _d3d_settings.Windowed_MultisampleQuality = 0;
    _d3d_settings.Windowed_VertexProcessingType = *(VertexProcessingType*)pBestDeviceCombo->pVertexProcessingTypeList->GetPtr(0);
    _d3d_settings.Windowed_PresentInterval = *(UINT*)pBestDeviceCombo->pPresentIntervalList->GetPtr(0);
    return true;
}

bool Gfx::find_best_fullscreen_mode(bool bRequireHAL,bool bRequireREF)
{
    // For fullscreen, default to first HAL DeviceCombo that supports the current desktop 
    // display mode, or any display mode if HAL is not compatible with the desktop mode, or 
    // non-HAL if no HAL is available
    D3DDISPLAYMODE adapterDesktopDisplayMode;
    D3DDISPLAYMODE bestAdapterDesktopDisplayMode;
    D3DDISPLAYMODE bestDisplayMode;
    bestAdapterDesktopDisplayMode.Width = 0;
    bestAdapterDesktopDisplayMode.Height = 0;
    bestAdapterDesktopDisplayMode.Format = D3DFMT_UNKNOWN;
    bestAdapterDesktopDisplayMode.RefreshRate = 0;

    D3DAdapterInfo* pBestAdapterInfo = NULL;
    D3DDeviceInfo* pBestDeviceInfo = NULL;
    D3DDeviceCombo* pBestDeviceCombo = NULL;

    for( UINT iai = 0; iai < _d3d_enumeration.m_pAdapterInfoList->Count(); iai++ )
    {
        D3DAdapterInfo* pAdapterInfo = (D3DAdapterInfo*)_d3d_enumeration.m_pAdapterInfoList->GetPtr(iai);
        _d3d9->GetAdapterDisplayMode( pAdapterInfo->AdapterOrdinal, &adapterDesktopDisplayMode );
        for( UINT idi = 0; idi < pAdapterInfo->pDeviceInfoList->Count(); idi++ )
        {
            D3DDeviceInfo* pDeviceInfo = (D3DDeviceInfo*)pAdapterInfo->pDeviceInfoList->GetPtr(idi);
            if (bRequireHAL && pDeviceInfo->DevType != D3DDEVTYPE_HAL)
                continue;
            if (bRequireREF && pDeviceInfo->DevType != D3DDEVTYPE_REF)
                continue;
            for( UINT idc = 0; idc < pDeviceInfo->pDeviceComboList->Count(); idc++ )
            {
                D3DDeviceCombo* pDeviceCombo = (D3DDeviceCombo*)pDeviceInfo->pDeviceComboList->GetPtr(idc);
                bool bAdapterMatchesBB = (pDeviceCombo->BackBufferFormat == pDeviceCombo->AdapterFormat);
                bool bAdapterMatchesDesktop = (pDeviceCombo->AdapterFormat == adapterDesktopDisplayMode.Format);
                if (pDeviceCombo->IsWindowed)
                    continue;
                // If we haven't found a compatible set yet, or if this set
                // is better (because it's a HAL, and/or because formats match better),
                // save it
                if (pBestDeviceCombo == NULL ||
                    pBestDeviceCombo->DevType != D3DDEVTYPE_HAL && pDeviceInfo->DevType == D3DDEVTYPE_HAL ||
                    pDeviceCombo->DevType == D3DDEVTYPE_HAL && pBestDeviceCombo->AdapterFormat != adapterDesktopDisplayMode.Format && bAdapterMatchesDesktop ||
                    pDeviceCombo->DevType == D3DDEVTYPE_HAL && bAdapterMatchesDesktop && bAdapterMatchesBB )
                {
                    bestAdapterDesktopDisplayMode = adapterDesktopDisplayMode;
                    pBestAdapterInfo = pAdapterInfo;
                    pBestDeviceInfo = pDeviceInfo;
                    pBestDeviceCombo = pDeviceCombo;
                    if (pDeviceInfo->DevType == D3DDEVTYPE_HAL && bAdapterMatchesDesktop && bAdapterMatchesBB)
                    {
                        // This fullscreen device combo looks great -- take it
                        goto EndFullscreenDeviceComboSearch;
                    }
                    // Otherwise keep looking for a better fullscreen device combo
                }
            }
        }
    }
EndFullscreenDeviceComboSearch:
    if (pBestDeviceCombo == NULL)
        return false;

    // Need to find a display mode on the best adapter that uses pBestDeviceCombo->AdapterFormat
    // and is as close to bestAdapterDesktopDisplayMode's res as possible
    bestDisplayMode.Width = 0;
    bestDisplayMode.Height = 0;
    bestDisplayMode.Format = D3DFMT_UNKNOWN;
    bestDisplayMode.RefreshRate = 0;
    for( UINT idm = 0; idm < pBestAdapterInfo->pDisplayModeList->Count(); idm++ )
    {
        D3DDISPLAYMODE* pdm = (D3DDISPLAYMODE*)pBestAdapterInfo->pDisplayModeList->GetPtr(idm);
        if( pdm->Format != pBestDeviceCombo->AdapterFormat )
            continue;
        if( pdm->Width == bestAdapterDesktopDisplayMode.Width &&
            pdm->Height == bestAdapterDesktopDisplayMode.Height && 
            pdm->RefreshRate == bestAdapterDesktopDisplayMode.RefreshRate )
        {
            // found a perfect match, so stop
            bestDisplayMode = *pdm;
            break;
        }
        else if( pdm->Width == bestAdapterDesktopDisplayMode.Width &&
                 pdm->Height == bestAdapterDesktopDisplayMode.Height && 
                 pdm->RefreshRate > bestDisplayMode.RefreshRate )
        {
            // refresh rate doesn't match, but width/height match, so keep this
            // and keep looking
            bestDisplayMode = *pdm;
        }
        else if( pdm->Width == bestAdapterDesktopDisplayMode.Width )
        {
            // width matches, so keep this and keep looking
            bestDisplayMode = *pdm;
        }
        else if( bestDisplayMode.Width == 0 )
        {
            // we don't have anything better yet, so keep this and keep looking
            bestDisplayMode = *pdm;
        }
    }

    _d3d_settings.pFullscreen_AdapterInfo = pBestAdapterInfo;
    _d3d_settings.pFullscreen_DeviceInfo = pBestDeviceInfo;
    _d3d_settings.pFullscreen_DeviceCombo = pBestDeviceCombo;
    _d3d_settings.IsWindowed = false;
    _d3d_settings.Fullscreen_DisplayMode = bestDisplayMode;
    if (_d3d_enumeration.AppUsesDepthBuffer)
        _d3d_settings.Fullscreen_DepthStencilBufferFormat = *(D3DFORMAT*)pBestDeviceCombo->pDepthStencilFormatList->GetPtr(0);
    _d3d_settings.Fullscreen_MultisampleType = *(D3DMULTISAMPLE_TYPE*)pBestDeviceCombo->pMultiSampleTypeList->GetPtr(0);
    _d3d_settings.Fullscreen_MultisampleQuality = 0;
    _d3d_settings.Fullscreen_VertexProcessingType = *(VertexProcessingType*)pBestDeviceCombo->pVertexProcessingTypeList->GetPtr(0);
    _d3d_settings.Fullscreen_PresentInterval = D3DPRESENT_INTERVAL_DEFAULT;
    return true;
}

void Gfx::build_present_params_from_settings()
{
    _d3dpp.Windowed               = _d3d_settings.IsWindowed;
    _d3dpp.BackBufferCount        = 1;
    _d3dpp.MultiSampleType        = _d3d_settings.MultisampleType();
    _d3dpp.MultiSampleQuality     = _d3d_settings.MultisampleQuality();
    _d3dpp.SwapEffect             = D3DSWAPEFFECT_DISCARD;
    _d3dpp.EnableAutoDepthStencil = _d3d_enumeration.AppUsesDepthBuffer;
    _d3dpp.hDeviceWindow          = _window;
    if( _d3d_enumeration.AppUsesDepthBuffer )
    {
        _d3dpp.Flags              = D3DPRESENTFLAG_DISCARD_DEPTHSTENCIL;
        _d3dpp.AutoDepthStencilFormat = _d3d_settings.DepthStencilBufferFormat();
    }
    else
    {
        _d3dpp.Flags              = 0;
    }

    if( _window_mode )
    {
        _d3dpp.BackBufferWidth=_window_client.right - _window_client.left;
        _d3dpp.BackBufferHeight=_window_client.bottom - _window_client.top;
        _d3dpp.BackBufferFormat=_d3d_settings.PDeviceCombo()->BackBufferFormat;
        _d3dpp.FullScreen_RefreshRateInHz=0;
        _d3dpp.PresentationInterval=_d3d_settings.PresentInterval();
    }
    else
    {
        _d3dpp.BackBufferWidth=(UINT)_resolution.x;
        _d3dpp.BackBufferHeight=(UINT)_resolution.y;
        _d3dpp.BackBufferFormat=_d3d_settings.PDeviceCombo()->BackBufferFormat;
        _d3dpp.FullScreen_RefreshRateInHz=_d3d_settings.Fullscreen_DisplayMode.RefreshRate;
        _d3dpp.PresentationInterval=_d3d_settings.PresentInterval();
    }
}

void Gfx::swap_buffers()
{
	HRESULT hRet=_device->Present(NULL,NULL,NULL,NULL);
	if (hRet!=D3D_OK)
	{
		switch (hRet)
		{
			case D3DERR_INVALIDCALL: 
				log("Couldn't swap buffers - Invalid call!");
				break;
			case D3DERR_DEVICELOST: 
				log("Couldn't swap buffers - Device lost!");
				break;
		}
	}
}

void Gfx::set_2d_render(float zoom)
{
	D3DXMATRIX	persp( 2.0f*zoom, 0.0f, 0.0f, 0.0f,
					   0.0f,-2.0f*zoom, 0.0f, 0.0f,
					   0.0f, 0.0f, 1.0f, 0.0f,
					  -1.0f*zoom, 1.0f*zoom, 0.0f, 1.0f);

	_device->SetTransform(D3DTS_PROJECTION,&persp);	

	D3DXMATRIX	identity;
	
	D3DXMatrixIdentity(&identity);
	_device->SetTransform(D3DTS_WORLDMATRIX(0),&identity);
}

void Gfx::clear(DWORD clear_buffers,float r,float g,float b,float a,float zVal,unsigned long sVal)
{
	D3DVIEWPORT9 viewport;
	viewport.X=0; viewport.Y=0;
	viewport.Width=(DWORD)_resolution.x;
	viewport.Height=(DWORD)_resolution.y;
	viewport.MinZ=0.0f;
	viewport.MaxZ=1.0f;

	_device->SetViewport(&viewport);

	HRESULT		hRet=_device->Clear(0,NULL,clear_buffers,D3DCOLOR_COLORVALUE(r,g,b,a),zVal,sVal);

	if (hRet!=D3D_OK)
	{
		ERROR_MSG("Can't clear buffers!");
	}
}

void Gfx::begin_scene()
{
	if (_in_scene) return;
	HRESULT		hRet=_device->BeginScene();
	_in_scene=true;
}

void Gfx::end_scene()
{
	if (!_in_scene) return;
	HRESULT		hRet=_device->EndScene();
	_in_scene=false;
}

void Gfx::set_camera(float x,float y,float z)
{
	D3DXMATRIX	view;
	
	D3DXMatrixTranslation(&view,-x/_resolution.x+0.5f,-y/_resolution.y+0.5f,-z);
	_device->SetTransform(D3DTS_VIEW,&view);
}

void Gfx::set_camera(float x,float y,float z,Quaternion rot)
{	
	D3DXMATRIX		mat;
	D3DXQUATERNION	quat=D3DXQUATERNION(rot.x,rot.y,rot.z,rot.w);
	D3DXQUATERNION	tmp;
	D3DXVECTOR3		vec(-x,-y,-z);

	D3DXQuaternionInverse(&tmp,&quat);
	D3DXMatrixAffineTransformation(&mat,1.0f,&(-vec),&tmp,&vec);
	_device->SetTransform(D3DTS_VIEW,&mat);
}

void Gfx::set_perspective(float fov,float aspect,float near_plane,float far_plane)
{
	D3DXMATRIX	persp;

	D3DXMatrixPerspectiveFovLH(&persp,deg_to_rad(fov/aspect),aspect,near_plane,far_plane);
	_device->SetTransform(D3DTS_PROJECTION,&persp);

}

Vec3f Gfx::transform_vertex(const float fov,const float aspect,const float near_plane,const float far_plane,
							const float cam_x,const float cam_y,const float cam_z,const Quaternion& rot,
							const Vec3f& v)
{
	D3DXMATRIX	persp;
	D3DXVECTOR3	vert(v.x,v.y,v.z);
	D3DXVECTOR3	output(v.x,v.y,v.z);

	D3DXMatrixPerspectiveFovLH(&persp,deg_to_rad(fov/aspect),aspect,near_plane,far_plane);
	
	D3DXMATRIX		mat;
	D3DXQUATERNION	quat=D3DXQUATERNION(rot.x,rot.y,rot.z,rot.w);
	D3DXQUATERNION	tmp;
	D3DXVECTOR3		vec(-cam_x,-cam_y,-cam_z);

	D3DXQuaternionInverse(&tmp,&quat);
	D3DXMatrixAffineTransformation(&mat,1.0f,&(-vec),&tmp,&vec);

	D3DVIEWPORT9 viewport;
	viewport.X=0; viewport.Y=0;
	viewport.Width=(DWORD)_resolution.x; viewport.Height=(DWORD)_resolution.y;
	viewport.MinZ=0; viewport.MaxZ=1;

	D3DXVec3Project(&output,&vert,&viewport,&persp,&mat,NULL);

	Vec3f ret(output.x,output.y,output.z);

	return ret;
}


void Gfx::set_world_transformation(int idx,float x,float y,float z,Quaternion r)
{
	D3DXMATRIX		w;
	D3DXVECTOR3		vec=D3DXVECTOR3(x,y,z);
	D3DXQUATERNION	rot=D3DXQUATERNION(r.x,r.y,r.z,r.w);

	D3DXMatrixAffineTransformation(&w,1.0f,NULL,&rot,&vec);
	_device->SetTransform(D3DTS_WORLD,&w);
}

void Gfx::set_light(UINT idx,D3DLIGHT9* l)
{
	if (l)
	{
		_device->SetLight(idx,l);
		_device->LightEnable(idx,TRUE);
	}
	else
	{
		_device->LightEnable(idx,FALSE);
	}
}

void Gfx::clear_lights()
{
	for (int i=0; i<8; i++) _device->LightEnable(i,FALSE);
}
