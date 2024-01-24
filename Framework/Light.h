#pragma once
#include "Object3d.h"

class Light :
	public Object3d
{
public:
	Light(void);
	virtual ~Light(void);

	void	set(UINT idx);

public:
	D3DLIGHT9	_light;
};
