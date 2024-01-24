#pragma once
#include "Object3d.h"

class Camera :
	public Object3d
{
public:
	Camera(void);
	virtual ~Camera(void);

			void	set();
			void	set_lookat(const Vec3f& from,const Vec3f& to);

public:
	float	_fov;
	float	_aspect;
	float	_near_plane;
	float	_far_plane;
	// For 3d blits
	Vec3f	_right_axis;
	Vec3f	_normal_axis;
};

extern Camera*	CAMERA3D;
