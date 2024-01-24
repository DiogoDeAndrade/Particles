#include "Defs.h"

LPDIRECTINPUT8		LPDI=NULL;
MouseEventCallback	g_mouse_event_callback=NULL;

void set_mouse_event_callback(MouseEventCallback callback)
{
	g_mouse_event_callback=callback;
}

MouseEventCallback	get_mouse_event_callback()
{
	return g_mouse_event_callback;
}
