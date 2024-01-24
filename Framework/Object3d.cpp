#include "Object3d.h"
#include "Gfx.h"
#include "Mesh.h"

Object3d::Object3d(void)
{
	_mesh=NULL;
	_name="";
	_ori.identity_quaternion();
	_pos.set(0,0,0);
	_draw=true;
	_link=NULL;
}

Object3d::~Object3d(void)
{
}

void Object3d::draw()
{
	if (!_mesh) return;

	if (_link)
	{
		_pos=_link->_pos;
		_ori=_link->_ori;
	}

	GFX->set_world_transformation(0,_pos.x,_pos.y,_pos.z,_ori);

	_mesh->draw();
}
