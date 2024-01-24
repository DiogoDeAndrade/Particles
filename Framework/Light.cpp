#include "Light.h"
#include "Gfx.h"
#include "Utils.h"

Light::Light(void)
{
	Vec3f d=Vec3f(-0.5f,-0.5f,0.5f);
	d.normalize();

	_light.Type=D3DLIGHT_DIRECTIONAL;
	_light.Ambient.r=_light.Ambient.g=_light.Ambient.b=_light.Ambient.a=0.0f;
	_light.Diffuse.r=_light.Diffuse.g=_light.Diffuse.b=_light.Diffuse.a=1.0f;
	_light.Specular.r=_light.Specular.g=_light.Specular.b=_light.Specular.a=1.0f;
	_light.Direction.x=d.x; _light.Direction.y=d.y; _light.Direction.z=d.z; 
	_light.Range=sqrt(FLT_MAX);
	_light.Falloff=1.0f;
	_light.Attenuation0=1.0f;
   	_light.Attenuation1=0.0f;
   	_light.Attenuation2=0.0f;
	_light.Theta=30.0f;
	_light.Phi=45.0f;
}

Light::~Light(void)
{
}

void Light::set(UINT idx)
{
	Vec3f dir;
	_ori.rotate_point(dir,Vec3f(0.0f,0.0f,1.0f));

	_light.Position=D3DXVECTOR3(_pos.x,_pos.y,_pos.z);
	_light.Direction=D3DXVECTOR3(dir.x,dir.y,dir.z);
	GFX->set_light(idx,&_light);
}
