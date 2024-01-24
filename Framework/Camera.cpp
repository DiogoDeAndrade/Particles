#include "Camera.h"
#include "Gfx.h"

Camera*	CAMERA3D=NULL;

Camera::Camera(void)
{
	_fov=60.0f;
	_aspect=1.333333f;
	_near_plane=1.0f;
	_far_plane=1000.0f;
}

Camera::~Camera(void)
{
}

void Camera::set()
{ 
	GFX->set_perspective(_fov,_aspect,_near_plane,_far_plane);
	GFX->set_camera(_pos.x,_pos.y,_pos.z,_ori);

	_ori.rotate_point(_right_axis,Vec3f(1.0f,0.0f,0.0f));
	_right_axis.y=0.0f;
	_right_axis.normalize();

	_ori.rotate_point(_normal_axis,Vec3f(0.0f,0.0f,-1.0f));
	_normal_axis.y=0.0f;
	_normal_axis.normalize();

	CAMERA3D=this;
}

void Camera::set_lookat(const Vec3f& f,const Vec3f& t)
{
	D3DXMATRIX	mat;
	D3DXVECTOR3	up=D3DXVECTOR3(0.0f,1.0f,0.0f);
	D3DXVECTOR3	from=D3DXVECTOR3(f.x,f.y,f.z);
	D3DXVECTOR3	to=D3DXVECTOR3(t.x,t.y,t.z);
	D3DXMatrixLookAtLH(&mat,&from,&to,&up);

	_pos=f;
	
	D3DXQUATERNION o;
	D3DXQuaternionIdentity(&o);
	D3DXQuaternionRotationMatrix(&o,&mat);
	D3DXQuaternionInverse(&o,&o);

	_ori=o;
}
