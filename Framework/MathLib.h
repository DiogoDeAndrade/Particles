#ifndef __MATHLIB_H
#define __MATHLIB_H

#pragma once

#include <math.h>
#include <d3dx9.h>

const float PI = 3.1415926535f;
const float PI_OVER_180 = PI / 180.0f;
const float PI_UNDER_180 = 180.0f / PI;
const float PI_OVER_2 = PI / 2.0f;
const float PI_OVER_TWO = PI_OVER_2;
const float PI_TIMES_2 = PI * 2.0f;
const float PI_TIMES_TWO = PI_TIMES_2;

const float VERY_SMALL_FLOAT = 1e-8f;
const float SMALLEST_FLOAT = -99999999.0f;
const float BIGGEST_FLOAT = 99999999.0f;
const float EPSILON_FLOAT = 1e-5f;
const float EPSILON_DOUBLE = 1e-15f;

const float SQRT2=1.414214f;

#define Sqrt(X) (float)(sqrtf(X))
#define Tan(X) (float)(tanf(X))
#define Floor(X) (float)(floorf(X))
#define Ceil(X) (float)(ceilf(X))
#define Atof(x) (float)(atof((x)))
#define Abs(x) (float)(fabs((x)))
#define Exp(x) (float)(exp((x)))
#define Sqr(x) ((x)*(x))
#define Sign(x) (((x)>0.0f)?(1.0f):((((x)<0.0f)?(-1.0f):(0.0f))))

inline float Gauss(float x,float mean,float variance)
{
	float c=(float)(1.0f/(variance*Sqrt(PI_TIMES_2)));
	float p=-(Sqr(x-mean)/(2*Sqr(variance)));
	float e=Exp(p);
	return 2.5f*c*e;
}

inline float deg_to_rad(const float ang) { return ang*PI_OVER_180; }
inline float rad_to_deg(const float ang) { return ang*PI_UNDER_180; }

#define Min(x,y) (((x)<(y))?(x):(y))
#define Max(x,y) (((x)>(y))?(x):(y))

#define Clamp(X,MIN,MAX) (((X)<(MIN))?(MIN):(((X)>(MAX))?(MAX):(X)))
/*
template<typename T>
inline	void swap(T& v1,T& v2)
{
	T tmp=v1;
	v1=v2;
	v2=tmp;
}
*/
////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////

class Vec2f
{
public:
	float x;
	float y;
public:
	Vec2f();									
	Vec2f(const float a, const float b);			

	float			normalize();							
	float			norm() const;							
	float			norm_squared() const;					
	void			set(const float a,const float b);

	Vec2f&			operator*=(const float s);

	float&			operator[](const int i);
	float&			operator()(const int i);
	float			operator[](const int i) const;
	float			operator()(const int i) const;

	float			dot(const Vec2f v) const;
};


inline bool operator<(const Vec2f& o1,const Vec2f& o2)
{
	if (o1.x<o2.x) return true;
	if (o1.x==o2.x)
	{
		return o1.y<o2.y;
	}
	return false;
}


class Vec3f
{
public:
	float x;
	float y;
	float z;
public:
	Vec3f();									
	Vec3f(const Vec3f& v);			
	Vec3f(const Vec3f* v);			
	Vec3f(const float v[3]);								
	Vec3f(const float a, const float b, const float c);	

	float			normalize();						
	float			norm() const;							
	float			norm_squared() const;						
	void			set(const float a,const float b,const float c);
	bool			is_null() const;

	float&			operator[](const int i);
	float&			operator()(const int i);
	float			operator[](const int i) const;
	float			operator()(const int i) const;

	Vec3f&	operator*=(float s);

	float			dot(const Vec3f v) const;
	Vec3f			cross(const Vec3f v) const;
};

inline bool operator<(const Vec3f& o1,const Vec3f& o2)
{
	if (o1.x<o2.x) return true;
	if (o1.x==o2.x)
	{
		if (o1.y<o2.y) return true;
		if (o1.y==o2.y)
		{
			return o1.z<o2.z;
		}
	}
	return false;
}

class Vec4f
{
public:
	union {
		float x;
		float r;
		float x1;
	};
	union {
		float y;
		float g;
		float y1;
	};
	union {
		float z;
		float b;
		float x2;
	};
	union {
		float w;
		float a;
		float y2;
	};
public:
	Vec4f();									
	Vec4f(const float a, const float b, const float c, const float d);	
	float			normalize();							
	float			norm() const;								
	float			norm_squared() const;						
	void			set(const float a,const float b,const float c,const float d);
	bool			inside(const Vec2f& v) const;
	void			harmonize_rect();

	// For Rect only
	void			offset(const float x,const float y);

	Vec4f&			operator*=(float s);
	float&			operator[](const int i);
	float			operator[](const int i) const;
};

inline bool operator<(const Vec4f& o1,const Vec4f& o2)
{
	if (o1.x<o2.x) return true;
	if (o1.x==o2.x)
	{
		if (o1.y<o2.y) return true;
		if (o1.y==o2.y)
		{
			if (o1.z<o2.z) return true;
			if (o1.z==o2.z)
			{
				return o1.w<o2.w;
			}
		}
	}
	return false;
}

/*********************************************************************************************
*                                        >>> Vec2 <<<                                       *
*********************************************************************************************/


inline Vec2f::Vec2f()
{
}


inline Vec2f::Vec2f(const float a, const float b)
{
	x = a;
	y = b;
}


inline void Vec2f::set(const float a,const float b)
{
	x=a; y=b;;
}


inline float Vec2f::normalize()
{
	register float norm = (float)sqrt(x*x + y*y);
	x = x/norm;
	y = y/norm;

	return norm;
}


inline float Vec2f::norm() const
{
	return (float)sqrt(x*x + y*y);
}


inline float Vec2f::norm_squared() const
{
	return x*x + y*y;
}

/**
*  Adds two vectors..
*/

inline Vec2f operator+(const Vec2f& vec1, const Vec2f& vec2)
{
	Vec2f retVec;

	retVec.x = vec1.x + vec2.x;
	retVec.y = vec1.y + vec2.y;

	return retVec;
}


inline Vec2f operator/(const Vec2f& vec1, const Vec2f& vec2)
{
	Vec2f retVec;

	retVec.x = vec1.x/vec2.x;
	retVec.y = vec1.y/vec2.y;

	return retVec;
}


inline Vec2f operator/(const Vec2f& vec, const float s)
{
	Vec2f retVec;

	retVec.x = vec.x / s;
	retVec.y = vec.y / s;

	return retVec;
}


inline Vec2f operator-(const Vec2f& vec1, const Vec2f& vec2)
{
	Vec2f retVec;

	retVec.x = vec1.x - vec2.x;
	retVec.y = vec1.y - vec2.y;

	return retVec;
}


// Produto com um escalar

inline Vec2f operator*(const Vec2f& vec, float s)
{
	Vec2f retVec;

	retVec.x = vec.x * s;
	retVec.y = vec.y * s;

	return retVec;
}

// Produto com um escalar

inline Vec2f operator*(const float s, const Vec2f& vec)
{
	Vec2f retVec;

	retVec.x = vec.x * s;
	retVec.y = vec.y * s;

	return retVec;
}

// Negar o vector (produto com -1)

inline Vec2f operator-(const Vec2f& vec)
{
	Vec2f retVec;

	retVec.x = -vec.x;
	retVec.y = -vec.y;

	return retVec;
}


inline Vec2f& Vec2f::operator*=(const float s)
{
	// Scale coefficients
	x*=s; y*=s;

	return *this;
}


inline bool operator==(const Vec2f& vec1, const Vec2f& vec2)
{
	return ((vec1.x == vec2.x) &&
			(vec1.y == vec2.y));
}


inline bool is_equal(const Vec2f& vec1, const Vec2f& vec2,float epsilon)
{
	return ((fabs(vec1.x-vec2.x)<epsilon) &&
			(fabs(vec1.y-vec2.y)<epsilon));
}


inline bool operator!=(const Vec2f& vec1, const Vec2f& vec2)
{
	return ((vec1.x != vec2.x) ||
		(vec1.y != vec2.y)    );
}


inline float dot_product(const Vec2f& vec1, const Vec2f& vec2)
{
	return vec1.x * vec2.x + vec1.y * vec2.y;
}


inline float& Vec2f::operator [](const int i)
{
	if (i==0) return x;
	return y;
}


inline float& Vec2f::operator ()(const int i)
{
	if (i==0) return x;
	return y;
}


inline float Vec2f::operator [](const int i) const
{
	if (i==0) return x;
	return y;
}


inline float Vec2f::operator ()(const int i) const 
{
	if (i==0) return x;
	return y;
}


inline	float Vec2f::dot(const Vec2f v) const 
{
	return x*v.x+y*v.y;
}

/*********************************************************************************************
*                                        >>> Vec3f <<<                                       *
*********************************************************************************************/

inline Vec3f::Vec3f()
{
}


inline Vec3f::Vec3f(const Vec3f& v)
{
	x = (float)v.x;
	y = (float)v.y;
	z = (float)v.z;
}


inline Vec3f::Vec3f(const Vec3f* v)
{
	x = (float)v->x;
	y = (float)v->y;
	z = (float)v->z;
}

inline Vec3f::Vec3f(const float v[3])
{
	x=v[0];
	y=v[1];
	z=v[2];
}


inline Vec3f::Vec3f(const float a, const float b, const float c)
{
	x = a;
	y = b;
	z = c;
}


inline void Vec3f::set(const float a,const float b,const float c)
{
	x=a; y=b; z=c;
}


inline float Vec3f::normalize()
{
	register float norm = (float)sqrt(x*x + y*y + z*z);
	x = x/norm;
	y = y/norm;
	z = z/norm;

	return norm;
}


inline float Vec3f::norm() const
{
	return (float)sqrt(x*x + y*y + z*z);
}


inline float Vec3f::norm_squared() const
{
	return x*x + y*y + z*z;
}


inline bool Vec3f::is_null() const
{
	return ((fabs(x)<EPSILON_FLOAT) &&
		(fabs(y)<EPSILON_FLOAT) &&
		(fabs(z)<EPSILON_FLOAT));
}


inline float& Vec3f::operator [](const int i)
{
	if (i==0) return x;
	else if (i==1) return y;
	else return z;
}


inline float& Vec3f::operator ()(const int i)
{
	if (i==0) return x;
	else if (i==1) return y;
	else return z;
}


inline float Vec3f::operator [](const int i) const
{
	if (i==0) return x;
	else if (i==1) return y;
	else return z;
}


inline float Vec3f::operator ()(const int i) const 
{
	if (i==0) return x;
	else if (i==1) return y;
	else return z;
}


inline Vec3f& Vec3f::operator*=(const float s)
{
	// Scale coefficients
	x*=s; y*=s; z*=s;

	return *this;
}


inline Vec3f operator+(const Vec3f& vec1, const Vec3f& vec2)
{
	Vec3f retVec;

	retVec.x = vec1.x + vec2.x;
	retVec.y = vec1.y + vec2.y;
	retVec.z = vec1.z + vec2.z;

	return retVec;
}


inline Vec3f operator-(const Vec3f& vec1, const Vec3f& vec2)
{
	Vec3f retVec;

	retVec.x = vec1.x - vec2.x;
	retVec.y = vec1.y - vec2.y;
	retVec.z = vec1.z - vec2.z;

	return retVec;
}

// Produto com um escalar

inline Vec3f operator*(const Vec3f& vec, float s)
{
	Vec3f retVec;

	retVec.x = vec.x * s;
	retVec.y = vec.y * s;
	retVec.z = vec.z * s;

	return retVec;
}

// Produto com um escalar

inline Vec3f operator*(const float s, const Vec3f& vec)
{
	Vec3f retVec;

	retVec.x = vec.x * s;
	retVec.y = vec.y * s;
	retVec.z = vec.z * s;

	return retVec;
}

// Divisiao com um escalar

inline Vec3f operator/(const Vec3f& vec, const float s)
{
	Vec3f retVec;

	retVec.x = vec.x / s;
	retVec.y = vec.y / s;
	retVec.z = vec.z / s;

	return retVec;
}

// Divisao com um escalar

inline Vec3f operator/(const float s, const Vec3f& vec)
{
	Vec3f retVec;

	retVec.x = vec.x / s;
	retVec.y = vec.y / s;
	retVec.z = vec.z / s;

	return retVec;
}

// Negar o vector (produto com -1)

inline Vec3f operator-(const Vec3f& vec)
{
	Vec3f retVec;

	retVec.x = -vec.x;
	retVec.y = -vec.y;
	retVec.z = -vec.z;

	return retVec;
}


inline bool operator==(const Vec3f& vec1, const Vec3f& vec2)
{
	return ((fabs(vec1.x-vec2.x)<EPSILON_FLOAT) &&
		(fabs(vec1.y-vec2.y)<EPSILON_FLOAT) &&
		(fabs(vec1.z-vec2.z)<EPSILON_FLOAT));
}


inline bool is_equal(const Vec3f& vec1, const Vec3f& vec2,float epsilon)
{
	return ((fabs(vec1.x-vec2.x)<epsilon) &&
		(fabs(vec1.y-vec2.y)<epsilon) &&
		(fabs(vec1.z-vec2.z)<epsilon));
}


inline bool operator!=(const Vec3f& vec1, const Vec3f& vec2)
{
	return !((fabs(vec1.x-vec2.x)<EPSILON_FLOAT) &&
		(fabs(vec1.y-vec2.y)<EPSILON_FLOAT) &&
		(fabs(vec1.z-vec2.z)<EPSILON_FLOAT));
}


inline float dot_product(const Vec3f& vec1, const Vec3f& vec2)
{
	return vec1.x * vec2.x + vec1.y * vec2.y + vec1.z * vec2.z;
}


inline float dot_product(const Vec3f *vec1,const Vec3f *vec2)
{
	return vec1->x * vec2->x + vec1->y * vec2->y + vec1->z * vec2->z;
}


inline Vec3f cross_product(const Vec3f& u, const Vec3f& v)
{
	Vec3f retVec;

	retVec.x = u.y * v.z - u.z * v.y;
	retVec.y = u.z * v.x - u.x * v.z;
	retVec.z = u.x * v.y - u.y * v.x;

	return retVec;
}


inline float Vec3f::dot(const Vec3f v) const 
{
	return x*v.x+y*v.y+z*v.z;
}


inline Vec3f Vec3f::cross(const Vec3f v) const 
{
	Vec3f retVec;

	retVec.x = y * v.z - z * v.y;
	retVec.y = z * v.x - x * v.z;
	retVec.z = x * v.y - y * v.x;

	return retVec;
}

/*********************************************************************************************
*                                        >>> Vec4 <<<                                       *
*********************************************************************************************/


inline Vec4f::Vec4f()
{
}


inline Vec4f::Vec4f(const float a, const float b, const float c, const float d)
{
	x = a;
	y = b;
	z = c;
	w = d;
}


inline float Vec4f::normalize()
{
	register float norm = (float)sqrt(x*x + y*y + z*z + w*w);
	x = x/norm;
	y = y/norm;
	z = z/norm;
	w = w/norm;

	return norm;
}


inline float Vec4f::norm() const
{
	return (float)sqrt(x*x + y*y + z*z + w*w);
}


inline float Vec4f::norm_squared() const
{
	return x*x + y*y + z*z + w*w;
}


inline void Vec4f::set(const float a,const float b,const float c,const float d)
{
	x=a; y=b; z=c; w=d;
}


inline void Vec4f::offset(const float a,const float b)
{
	x=x+a;
	y=y+b;
	z=z+a;
	w=w+b;
}


inline Vec4f& Vec4f::operator*=(const float s)
{
	// Scale coefficients
	x*=s; y*=s; z*=s; w*=s;

	return *this;
}


inline Vec4f operator+(const Vec4f& vec1, const Vec4f& vec2)
{
	Vec4f retVec;

	retVec.x = vec1.x + vec2.x;
	retVec.y = vec1.y + vec2.y;
	retVec.z = vec1.z + vec2.z;
	retVec.w = vec1.w + vec2.w;

	return retVec;
}


inline Vec4f operator-(const Vec4f& vec1, const Vec4f& vec2)
{
	Vec4f retVec;

	retVec.x = vec1.x - vec2.x;
	retVec.y = vec1.y - vec2.y;
	retVec.z = vec1.z - vec2.z;
	retVec.w = vec1.w - vec2.w;

	return retVec;
}

// Produto com um escalar

inline Vec4f operator*(const Vec4f& vec, const float s)
{
	Vec4f retVec;

	retVec.x = vec.x * s;
	retVec.y = vec.y * s;
	retVec.z = vec.z * s;
	retVec.w = vec.w * s;

	return retVec;
}

// Produto com um escalar

inline Vec4f operator*(const float s, const Vec4f& vec)
{
	Vec4f retVec;

	retVec.x = vec.x * s;
	retVec.y = vec.y * s;
	retVec.z = vec.z * s;
	retVec.w = vec.w * s;

	return retVec;
}

// Divisiao com um escalar

inline Vec4f operator/(const Vec4f& vec, const float s)
{
	Vec4f retVec;

	retVec.x = vec.x / s;
	retVec.y = vec.y / s;
	retVec.z = vec.z / s;
	retVec.w = vec.w / s;

	return retVec;
}

// Divisao com um escalar

inline Vec4f operator/(const float s, const Vec4f& vec)
{
	Vec4f retVec;

	retVec.x = vec.x / s;
	retVec.y = vec.y / s;
	retVec.z = vec.z / s;
	retVec.w = vec.w / s;

	return retVec;
}

// Multiplicacao por membro

inline Vec4f operator/(const Vec4f& vec1, const Vec4f& vec2)
{
	Vec4f retVec;

	retVec.x = vec1.x * vec2.x;
	retVec.y = vec1.y * vec2.y;
	retVec.z = vec1.z * vec2.z;
	retVec.w = vec1.w * vec2.w;

	return retVec;
}

// Negar o vector (produto com -1)

inline Vec4f operator-(const Vec4f& vec)
{
	Vec4f retVec;

	retVec.x = -vec.x;
	retVec.y = -vec.y;
	retVec.z = -vec.z;
	retVec.w = -vec.w;

	return retVec;
}


inline bool operator==(const Vec4f& vec1, const Vec4f& vec2)
{
	return ((vec1.x == vec2.x) &&
		(vec1.y == vec2.y) &&
		(vec1.z == vec2.z) &&
		(vec1.w == vec2.w)    );
}


inline bool is_equal(const Vec4f& vec1, const Vec4f& vec2,float epsilon)
{
	return ((fabs(vec1.x-vec2.x)<epsilon) &&
		(fabs(vec1.y-vec2.y)<epsilon) &&
		(fabs(vec1.z-vec2.z)<epsilon) &&
		(fabs(vec1.w-vec2.w)<epsilon));
}


inline bool operator!=(const Vec4f& vec1, const Vec4f& vec2)
{
	return ((vec1.x != vec2.x) ||
		(vec1.y != vec2.y) ||
		(vec1.z != vec2.z) ||
		(vec1.w != vec2.w)    );
}


inline float dot_product(const Vec4f& vec1, const Vec4f& vec2)
{
	return vec1.x * vec2.x + vec1.y * vec2.y + vec1.z * vec2.z + vec1.w * vec2.w;
}

// Produto com um escalar

inline Vec4f operator*(const Vec4f& vec1, const Vec4f& vec2)
{
	Vec4f retVec;

	retVec.x = vec1.x * vec2.x;
	retVec.y = vec1.y * vec2.y;
	retVec.z = vec1.z * vec2.z;
	retVec.w = vec1.w * vec2.w;

	return retVec;
}


inline float& Vec4f::operator [](const int i)
{
	if (i==0) return x;
	else if (i==1) return y;
	else if (i==2) return z;
	else return w;
}


inline float Vec4f::operator [](const int i) const
{
	if (i==0) return x;
	else if (i==1) return y;
	else if (i==2) return z;
	else return w;
}


inline bool Vec4f::inside(const Vec2f& v) const
{
	if ((v.x>=x1) && (v.x<=x2) &&
		(v.y>=y1) && (v.y<=y2)) return true;
	return false;
}


inline	void Vec4f::harmonize_rect()
{
	float tmp;
	if (x1>x2) { tmp=x1; x1=x2; x2=tmp; }
	if (y1>y2) { tmp=y1; y1=y2; y2=tmp; }
}

inline	Vec2f	rotate_around_point(Vec2f source,Vec2f origin,float ang)
{
	Vec2f disp=source-origin;

	Vec2f res;
	res.x=disp.x*cos(ang)-disp.y*sin(ang);
	res.y=disp.x*sin(ang)+disp.y*cos(ang);

	res=res+origin;

	return res;
}

inline	float	interpolate(const float t,const float v1,const float v2)
{
	return v1+t*(v2-v1);
}

inline	Vec2f	interpolate_v2(const float t,const Vec2f& v1,const Vec2f& v2)
{
	Vec2f	tmp(v1.x,v1.y);
	tmp.x+=t*(v2.x-v1.x);
	tmp.y+=t*(v2.y-v1.y);
	return tmp;
}

inline	Vec2f	interpolate_v2(const float t,const Vec2f* v1,const Vec2f* v2)
{
	Vec2f	tmp(v1->x,v1->y);
	tmp.x+=t*(v2->x-v1->x);
	tmp.y+=t*(v2->y-v1->y);
	return tmp;
}

inline	Vec3f	interpolate_v3(const float t,const Vec3f& v1,const Vec3f& v2)
{
	Vec3f	tmp(v1.x,v1.y,v1.z);
	tmp.x+=t*(v2.x-v1.x);
	tmp.y+=t*(v2.y-v1.y);
	tmp.z+=t*(v2.z-v1.z);
	return tmp;
}

inline	Vec3f	interpolate_v3(const float t,const Vec3f* v1,const Vec3f* v2)
{
	Vec3f	tmp(v1->x,v1->y,v1->z);
	tmp.x+=t*(v2->x-v1->x);
	tmp.y+=t*(v2->y-v1->y);
	tmp.z+=t*(v2->z-v1->z);
	return tmp;
}

inline	Vec4f	interpolate_v4(const float t,const Vec4f& v1,const Vec4f& v2)
{
	Vec4f	tmp(v1.x,v1.y,v1.z,v1.w);
	tmp.x+=t*(v2.x-v1.x);
	tmp.y+=t*(v2.y-v1.y);
	tmp.z+=t*(v2.z-v1.z);
	tmp.w+=t*(v2.w-v1.w);
	return tmp;
}

inline	Vec4f	interpolate_v4(const float t,const Vec4f* v1,const Vec4f* v2)
{
	Vec4f	tmp(v1->x,v1->y,v1->z,v1->w);
	tmp.x+=t*(v2->x-v1->x);
	tmp.y+=t*(v2->y-v1->y);
	tmp.z+=t*(v2->z-v1->z);
	tmp.w+=t*(v2->w-v1->w);
	return tmp;
}

typedef Vec4f	Color;
typedef Vec4f	Rect;

// Converts 4 floats representing a color to a colorref (DWORD)
inline unsigned long float_to_colorref(const float r,const float g,const float b,const float a) 
{ 
	return (int(a*255.0f) << 24)+(int(r*255.0f) << 16)+(int(g*255.0f) << 8)+(int(b*255.0f)); 
}
// Converts a 4d vector representing a color to a colorref (DWORD)
inline unsigned long float_to_colorref(const Color *color) 
{ 
	return ((int(color->a*255.0f)&0xFF) << 24)+((int(color->r*255.0f)&0xFF) << 16)+((int(color->g*255.0f)&0xFF) << 8)+((int(color->b*255.0f)&0xFF)); 
}

inline unsigned long float_to_colorref(const Color &color) 
{ 
	return ((int(color.a*255.0f)&0xFF) << 24)+((int(color.r*255.0f)&0xFF) << 16)+((int(color.g*255.0f)&0xFF) << 8)+((int(color.b*255.0f)&0xFF)); 
}

inline unsigned long float_to_colorref(const Vec3f &color) 
{ 
	return ((255 << 24)+
		((int(color.x*255.0f)&0xFF) << 16)+
		((int(color.y*255.0f)&0xFF) << 8)+
		((int(color.z*255.0f)&0xFF))); 
}

// Converts a colorref to a vector representating the color
inline Color colorref_to_float(const unsigned long color) 
{ 
	Color tmp;

	tmp.a=float((color & 0x00FF000000) >> 24)/255.0f;
	tmp.r=float((color & 0x0000FF0000) >> 16)/255.0f;
	tmp.g=float((color & 0x000000FF00) >> 8)/255.0f;
	tmp.b=float((color & 0x00000000FF))/255.0f;

	return tmp;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


class Quaternion
{
public:
	Quaternion();											
	Quaternion(const float _x, const float _y, const float _z, const float _w);		
	Quaternion(const Quaternion& o);		
	Quaternion(const D3DXQUATERNION& o);		

	void operator+=(const Quaternion& q);					///< Adds Quaternion q to this Quaternion
	void operator-=(const Quaternion& q);					///< Subtracts Quaternion q from this Quaternion

	void add(const Quaternion& q, const Quaternion& r);		///< This Quaternion will be set to the sum q + r
	void sub(const Quaternion& q, const Quaternion& r);		///< This Quaternion will be set to the subtraction q - r
	void mult(const Quaternion& q, const Quaternion& r);	///< This Quaternion will be set to the product qr

	void conj();											///< This Quaternion will be set to its conjugate
	void conj(const Quaternion& q);							///< This Quaternion will be set to the conjugate of q

	void invert();											///< This Quaternion will be inverted
	void invert(const Quaternion& q);						///< This Quaternion will be set to the inverse of q
	void invert(const Quaternion* q);
	void unit_invert();										///< This is assumed to be a unit Quaternion and it will be inverted (deprecated - same as invert)
	void unit_invert(const Quaternion& q);					///< This Quaternion will be set to the inverse of unit Quaternion q (deprecated - same as invert)

	float norm() const;											///< Returns the norm (there is an bug in this function)
	float normalize();										///< Normalizes the Quaternion

	void identity_quaternion();								///< This Quaternion will be set to the identity Quaternion
	bool is_identity() const;

	void rotation_quaternion(const float _x, const float _y, const float _z, const float ang);	///< This Quaternion will be set to a Quaternion representing a rotation of ang degrees (*not radians*) aroung vector (x,y,z)
	void rotation_quaternion(const Vec3f& vec, const float ang);				///< This Quaternion will be set to a Quaternion representing a rotation of ang degrees (*not radians*) aroung vector vec

	//	void euler_quaternion(const float yaw, const float pitch, const float roll);
	void to_euler(float& rotX,float& rotY,float& rotZ);

	void rotate_point(Vec3f& dst, const Vec3f& org) const;				///< The rotation encoded in the Quaternion will be applied to org and written to dst
	void slerp(const Quaternion& to, float t, Quaternion& res);		///< Interpolates betwwen this Quaternion and Quaternion to. t shoud be beeetween 0 and 1.

	void set(const float _x,const float _y,const float _z,const float _w);

public:
	float x, y, z, w;
};


inline Quaternion::Quaternion()
{
}


inline Quaternion::Quaternion(const float _x, const float _y, const float _z, const float _w)
{
	x = _x;
	y = _y;
	z = _z;
	w = _w;
}

inline Quaternion::Quaternion(const Quaternion& o)
{
	x=o.x;
	y=o.y;
	z=o.z;
	w=o.w;
}

inline Quaternion::Quaternion(const D3DXQUATERNION& o)
{
	x=o.x;
	y=o.y;
	z=o.z;
	w=o.w;
}

inline void Quaternion::identity_quaternion()
{
	x = y = z = 0.0f;
	w = 1.0f;
}


inline bool Quaternion::is_identity() const
{
	if ((x==0.0f) && (y==0.0f) && (z==0.0f) && (w==1.0f)) return true;
	return false;
}



inline void Quaternion::operator+=(const Quaternion& q)
{
	x += q.x;
	y += q.y;
	z += q.z;
	w += q.w;
}


inline void Quaternion::operator-=(const Quaternion& q)
{
	x -= q.x;
	y -= q.y;
	z -= q.z;
	w -= q.w;
}


inline bool operator!=(const Quaternion& q1, const Quaternion& q2)
{
	return ((fabs(q1.x-q2.x) > EPSILON_FLOAT) ||
		(fabs(q1.y-q2.y) > EPSILON_FLOAT) ||
		(fabs(q1.z-q2.z) > EPSILON_FLOAT) ||
		(fabs(q1.w-q2.w) > EPSILON_FLOAT));
}


inline void Quaternion::add(const Quaternion& q, const Quaternion& r)
{
	x = q.x + r.x;
	y = q.y + r.y;
	z = q.z + r.z;
	w = q.w + r.w;
}


inline void Quaternion::sub(const Quaternion& q, const Quaternion& r)
{
	x = q.x - r.x;
	y = q.y - r.y;
	z = q.z - r.z;
	w = q.w - r.w;
}


inline void Quaternion::mult(const Quaternion& q, const Quaternion& r)
{
	float A, B, C, D, E, F, G, H;

	A = (q.w + q.x)*(r.w + r.x);
	B = (q.z - q.y)*(r.y - r.z);
	C = (q.w - q.x)*(r.y + r.z); 
	D = (q.y + q.z)*(r.w - r.x);
	E = (q.x + q.z)*(r.x + r.y);
	F = (q.x - q.z)*(r.x - r.y);
	G = (q.w + q.y)*(r.w - r.z);
	H = (q.w - q.y)*(r.w + r.z);

	w = B + (-E - F + G + H) /2;
	x = A - (E + F + G + H)/2; 
	y = C + (E - F + G - H)/2; 
	z = D + (E - F - G + H)/2;
}



inline void Quaternion::conj()
{
	x = -x;
	y = -y;
	z = -z;
}


inline void Quaternion::conj(const Quaternion& q)
{
	x = -q.x;
	y = -q.y;
	z = -q.z;
	w = q.w;
}


inline float Quaternion::norm() const
{
	return x * x + y * y + z * z + w * w;	// Norma mal calculada
}


inline float Quaternion::normalize()
{
	float n=sqrt(x * x + y * y + z * z + w * w);

	x/=n;
	y/=n;
	z/=n;
	w/=n;

	return n;
}


inline void Quaternion::invert(const Quaternion& q)
{
	float inv_norm = 1.0f / q.norm();

	x = -q.x * inv_norm;
	y = -q.y * inv_norm;
	z = -q.z * inv_norm;

	w = q.w * inv_norm;
}


inline void Quaternion::invert(const Quaternion* q)
{
	float inv_norm = 1.0f / q->norm();

	x = -q->x * inv_norm;
	y = -q->y * inv_norm;
	z = -q->z * inv_norm;
	w =  q->w * inv_norm;
}


inline void Quaternion::invert()
{
	float inv_norm = 1.0f / norm();

	x *= -inv_norm;
	y *= -inv_norm;
	z *= -inv_norm;

	w *= inv_norm;
}



inline void Quaternion::unit_invert()
{
	conj();
}


inline void Quaternion::unit_invert(const Quaternion& q)
{
	conj(q);
	w = q.w;
}


inline void Quaternion::rotation_quaternion(const float _x, const float _y, const float _z, const float ang)
{
	float ang_2 = ang * 0.5f * PI_OVER_180;
	float sin_ang_2 = sin(ang_2);

	x = _x * sin_ang_2;
	y = _y * sin_ang_2;
	z = _z * sin_ang_2;
	w = cos(ang_2);
}


inline void Quaternion::rotation_quaternion(const Vec3f& vec, const float a)
{
	float ang = -a;		
	float ang_2 = ang * 0.5f * PI_OVER_180;
	float sin_ang_2 = sin(ang_2);
	Vec3f v = vec;
	v.normalize();

	x = v.x * sin_ang_2;
	y = v.y * sin_ang_2;
	z = v.z * sin_ang_2;
	w = cos(ang_2);
}

// Assumes a unit Quaternion

inline void Quaternion::rotate_point(Vec3f& dst, const Vec3f& org) const
{
	Quaternion p(org.x, org.y, org.z,0);		// NOTA: Optimizar isto 'a mao ...
	Quaternion r,r1,c;

	c.conj(*this);
	r1.mult(*this,p);
	r.mult(r1,c);

	dst.x = r.x;
	dst.y = r.y;
	dst.z = r.z;	
}


inline void Quaternion::set(const float _x,const float _y,const float _z,const float _w)
{
	x=_x;
	y=_y;
	z=_z;
	w=_w;
}


inline void Quaternion::slerp(const Quaternion& to, float t, Quaternion& res)
{
	float	to1[4];
	float	omega, cosom, sinom, scale0, scale1;
	const float DELTA = 0.0001f;


	// calc cosine
	cosom = x * to.x + y * to.y + z * to.z + w * to.w;

	// adjust signs (if necessary)

	if ( cosom <0.0 ){ cosom = -cosom; to1[0] = - to.x;
	to1[1] = - to.y;
	to1[2] = - to.z;
	to1[3] = - to.w;
	} else  {
		to1[0] = to.x;
		to1[1] = to.y;
		to1[2] = to.z;
		to1[3] = to.w;
	}

	// calculate coefficients
	if ( (1.0f - cosom) > DELTA ) {
		// standard case (slerp)
		omega = acos(cosom);
		sinom = sin(omega);
		scale0 = sin((1.0f - t) * omega) / sinom;
		scale1 = sin(t * omega) / sinom;

	} else {        
		// "from" and "to" Quaternions are very close 
		//  ... so we can do a linear scint16erpolation
		scale0 = 1.0f - t;
		scale1 = t;
	}

	// calculate final values
	res.x = float(scale0 * x + scale1 * to1[0]);
	res.y = float(scale0 * y + scale1 * to1[1]);
	res.z = float(scale0 * z + scale1 * to1[2]);
	res.w = float(scale0 * w + scale1 * to1[3]);
}


inline Quaternion vector_rotation(const Vec3f& f,const Vec3f& t) 
{
	Vec3f from=f;
	from.normalize();
	Vec3f to=t;
	to.normalize();

	if (from==to)
	{
		return Quaternion(0,0,0,1);
	}

	Vec3f rot;
	if (from==-to)
	{
		// Existem infinitas escolhas para o vector de rota�o, ERRO ...
		rot=Vec3f(0.0f,1.0f,0.0f);
	}
	else
	{
		rot=cross_product(from,to);
	}

	float cos_2ang=dot_product(from,to);
	rot.normalize();
	float sin_ang=sqrt((1 - cos_2ang) / 2);
	float cos_ang=sqrt(1 - sin_ang * sin_ang);
	return Quaternion(rot.x*sin_ang,rot.y*sin_ang,rot.z*sin_ang,cos_ang);
}


inline Quaternion vector_to_quaternion(Vec3f& vec) 
{
	Vec3f tmp(0.0f,0.0f,1.0f);

	return vector_rotation(tmp,vec);
}


inline	Quaternion	helper_get_ori2d(const Vec3f& vec)
{
	// Look at
	// Rot around y axis
	// Get "base" view vector
	Vec3f	delta=vec; delta.y=0.0f; delta.normalize();
	// Get angle around Y axis
	float	ang_y=delta.z;
	if (ang_y<-1.0f) ang_y=-1.0f;
	else if (ang_y>1.0f) ang_y=1.0f;
	ang_y=(float)rad_to_deg((float)acos(ang_y));
	// Rotate around Y axis
	Quaternion ori; ori.identity_quaternion(); 
	if (delta.x<0.0f) ori.rotation_quaternion(0.0f,1.0f,0.0f,-ang_y);
	else ori.rotation_quaternion(0.0f,1.0f,0.0f,ang_y);
	return ori;
}

// ********************************************************************/
// Menos eficientes - use with care !!!


inline Quaternion operator+(const Quaternion& q, const Quaternion& r)
{
	Quaternion ret;

	ret.x = q.x + r.x;
	ret.y = q.y + r.y;
	ret.z = q.z + r.z;
	ret.w = q.w + r.w;

	return ret;
}


inline Quaternion operator-(const Quaternion& q, const Quaternion& r)
{
	Quaternion ret;

	ret.x = q.x - r.x;
	ret.y = q.y - r.y;
	ret.z = q.z - r.z;
	ret.w = q.w - r.w;

	return ret;
}


inline Quaternion operator*(const Quaternion& q, const Quaternion& r)
{
	Quaternion ret;

	float A, B, C, D, E, F, G, H;

	A = (q.w + q.x)*(r.w + r.x);
	B = (q.z - q.y)*(r.y - r.z);
	C = (q.w - q.x)*(r.y + r.z); 
	D = (q.y + q.z)*(r.w - r.x);
	E = (q.x + q.z)*(r.x + r.y);
	F = (q.x - q.z)*(r.x - r.y);
	G = (q.w + q.y)*(r.w - r.z);
	H = (q.w - q.y)*(r.w + r.z);

	ret.w = B + (-E - F + G + H) /2;
	ret.x = A - (E + F + G + H)/2; 
	ret.y = C + (E - F + G - H)/2; 
	ret.z = D + (E - F - G + H)/2;

	return ret;
}


inline void Quaternion::to_euler(float& rotX,float& rotY,float& rotZ)
{
	rotX=(float)rad_to_deg((float)(atan((2.0f*(x*w+y*z))/(1.0f-2.0f*(Sqr(z)+Sqr(w))))));
	rotY=(float)rad_to_deg((float)(asin((2.0f*(x*z-y*w)))));
	rotZ=-(float)rad_to_deg((float)(atan((2.0f*(x*y+z*w))/(1.0f-2.0f*(Sqr(y)+Sqr(z))))));
}


inline Quaternion operator-(const Quaternion& q)
{
	Quaternion ret;

	float inv_norm=1.0f /q.norm();

	ret.x=-inv_norm*q.x;
	ret.y=-inv_norm*q.y;
	ret.z=-inv_norm*q.z;

	ret.w=inv_norm*q.w;

	return ret;
}

#endif