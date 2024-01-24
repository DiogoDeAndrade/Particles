#pragma once

#include "Defs.h"
#include "lua/LUAMain.h"

class BaseMesh;

class Object3d : public LUAReference
{
public:
	Object3d(void);
	virtual ~Object3d(void);

	virtual void	draw();

	inline	void		set_mesh(BaseMesh* mesh) { _mesh=mesh; }
	inline	std::string	get_name() { return _name; }
	inline	void		set_name(const std::string& name) { _name=name; }

	inline	void		set_draw(bool b) { _draw=b; }
	inline	bool		get_draw() { return _draw; }

	inline	void		link_to(Object3d* obj) { _link=obj; }

public:
	std::string	_name;
	Vec3f		_pos;
	Quaternion	_ori;
	BaseMesh*	_mesh;
	bool		_draw;
	Object3d*	_link;
};
