#include "MouseCursor.h"
#include "TextureManager.h"
#include "Gfx.h"

MouseCursor*	CURSOR=NULL;

MouseCursor::MouseCursor(const char* cursor_texture)
{
	_pos.z=0.1f;
	set_image(TEXTURES->find_by_name(cursor_texture));
	set_hotspot(32,32);
	CURSOR=this;

	set_priority(-1000.0f);
}

MouseCursor::~MouseCursor(void)
{
	CURSOR=NULL;
}

void MouseCursor::anim(float t)
{
	Sprite::anim(t);

	POINT pt;
	GetCursorPos(&pt);
	_pos.x=(float)pt.x;
	_pos.y=(float)pt.y;

	RECT	full, client;
	long	clientw, clienth;
	HWND	window=GFX->get_window();

	GetClientRect(window,&client);
	clientw=client.right-client.left;
	clienth=client.bottom-client.top;
	AdjustWindowRect(&client,GFX->get_window_style(),false); 
	GetWindowRect(window,&full);

	RECT	e;
	e.left=full.left-client.left;
	e.top=full.top-client.top;
	e.right=full.left-client.left+clientw;
	e.bottom=full.top-client.top + clienth;

	float	sx=(float)(e.right-e.left);
	float	sy=(float)(e.bottom-e.top);

	_pos.x=(GFX->get_resolution().x)*(pt.x-e.left)/sx;
	_pos.y=(GFX->get_resolution().y)*(pt.y-e.top)/sy;
}
