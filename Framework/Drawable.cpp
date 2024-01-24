// Drawable.cpp: implementation of the Drawable class.
//
//////////////////////////////////////////////////////////////////////

#include "Drawable.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Drawable::Drawable()
{
	_last_frame_time=(float)timeGetTime();
	_pos.x=0.0f;
	_pos.y=0.0f;
	_pos.z=0.0f;
	_current_image=0;
	_priority=0.0f;
	_draw=true;
	_blend_mode=BLEND_ALPHA;
	_scale_x=_scale_y=1.0f;
	set_hotspot(0.0f,0.0f);
	set_color(1.0f,1.0f,1.0f,1.0f);
}

Drawable::~Drawable()
{

}

