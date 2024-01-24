#ifndef __DEFS_H
#define __DEFS_H

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <mmsystem.h>
#include <string>
#include <vector>
#include <map>
#include <list>

#define	DIRECTINPUT_VERSION         0x0800
#include <dinput.h>
#include <d3d9.h>
#include <d3dx9.h>

#include "MathLib.h"

extern LPDIRECTINPUT8	LPDI;

typedef void (*MouseEventCallback)(unsigned long msg,unsigned long wparam,unsigned long lparam);

void set_mouse_event_callback(MouseEventCallback callback);
MouseEventCallback get_mouse_event_callback();

#endif