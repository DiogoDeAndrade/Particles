
#ifndef __UTILS_H
#define __UTILS_H

#include "Defs.h"
#include <stdio.h>

#define ERROR_MSG(X) MessageBox(NULL,X,"Game Error",MB_ICONERROR|MB_OK)

inline float get_random_number(float min,float max)
{
	if (min==max) return min;

	float d=float(rand())/float(RAND_MAX);

	return d*(max-min)+min;
}

inline unsigned long get_random_number_ul(unsigned long min,unsigned long max)
{
	if (min==max) return min;

	return (rand()%(max-min))+min;
}

inline float sign(float n)
{
	return ((n<0.0f)?(-1.0f):((n>0.0f)?(1.0f):(0.0f)));
}

#define Sqr(X) ((X)*(X))

inline	unsigned long FtoDW(float f) { return *((unsigned long*)&f); }

#include <d3dx9.h>

inline	Vec3f	rotate_point2d(Vec3f& p,float angle)
{
	float c=cosf(deg_to_rad(angle));
	float s=sinf(deg_to_rad(angle));

	Vec3f ret;
	ret.x=p.x*c-p.y*s;
	ret.y=p.x*s+p.y*c;
	ret.z=p.z;

	return ret;
}

inline	float	get_rotation_angle(D3DXVECTOR2& v)
{
	float angle=rad_to_deg(acos(v.x));
	if (v.y<0.0f) angle=360-angle;

	return angle+90.0f;
}

inline	float	get_rotation_angle(Vec3f& v)
{
	float angle=rad_to_deg(acos(v.x));
	if (v.y<0.0f) angle=360-angle;
	angle+=90.0f;
	while (angle<0.0f) angle+=360.0f;
	while (angle>360.0f) angle-=360.0f;

	return angle;
}

inline bool file_exists(const char* name)
{
	FILE* file=NULL;
	fopen_s(&file,name,"rb");
	if (!file) return false;
	fclose(file);
	return true;

}


inline	bool	is_float(const std::string& str)
{
	if (str.empty()) return false;

	size_t start_index=0;
	if (str[0]=='-') start_index=1;

	bool found_dot=false;
	bool found_e=false;
	bool found_number=false;
	for (size_t i=start_index; i<str.length(); i++)
	{
		if ((str[i]<'0') ||
			(str[i]>'9'))
		{
			if (str[i]=='.')
			{
				if (found_e) return false;
				if (found_dot) return false;
				found_dot=true;
			}
			else if (str[i]=='e')
			{
				if (found_e) return false;
				if (i==str.length()-1) return false;
				if (str[i+1]=='-')
				{
					i++;
				}
				found_e=true;
			}
			else return false;
		}
		else found_number=true;
	}
	if (!found_number) return false;
	return true;
}

inline bool is_float(const char *number)
{
	return is_float(std::string(number));
}

inline bool is_integer(const char *number)
{
	char *tmp=(char*)number;

	while (*tmp!=0)
	{
		char c=*tmp;

		if ((c<'0') || (c>'9'))
		{
			return false;
		}

		tmp++;
	}

	return true;
}

inline bool is_a_number(const char* number)
{
	char *tmp=(char*)number;

	bool find_number=false;
	bool dot=false;
	bool e=false;
	bool neg=false;

	while (*tmp!=0)
	{
		char c=*tmp;

		if ((c<'0') || (c>'9'))
		{
			if (c=='e') 
			{
				if (e) return false;
				if (!find_number) return false;
				e=true;
				neg=false;
			}
			else if (c=='.')
			{
				if (dot) return false;
				if (!find_number) return false;
				dot=true;
			}
			else if (c=='-')
			{
				if (neg) return false;
				neg=true;
			}
			else return false;
		}
		else { find_number=true; neg=true; }
		tmp++;
	}

	return find_number;
}

inline bool is_integer(const std::string& number)
{
	return is_integer(number.c_str());
}

inline bool is_a_number(const std::string& number)
{
	return is_a_number(number.c_str());
}

inline std::string string_find_replace(const std::string& source, const std::string& find, const std::string& replace)
{
	std::string	ret="";
	size_t		idx=0;
	size_t		p;
	do 
	{
		p=source.find(find,idx);
		if (p==std::string::npos)
		{
			ret=ret+source.substr(idx,std::string::npos);
			break;
		}
		else
		{
			ret=ret+source.substr(idx,p-idx)+replace;
			idx=p+find.length();
		}
	} while (1);

	return ret;
}

#endif