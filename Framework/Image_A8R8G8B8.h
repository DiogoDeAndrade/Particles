
#ifndef __IMAGE_A8R8G8B8_H
#define __IMAGE_A8R8G8B8_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "MathLib.h"
#include <stdio.h>

class Image_A8R8G8B8
{
protected:
	struct sb_DDSPixelFormat
	{
		unsigned long dwSize;
		unsigned long dwFlags;
		unsigned long dwFourCC;
		unsigned long dwRGBBitCount;
		unsigned long dwRBitMask;
		unsigned long dwGBitMask;
		unsigned long dwBBitMask;
		unsigned long dwRGBAlphaBitMask;
	};

	struct sb_DDSCaps
	{
		unsigned long dwCaps1;
		unsigned long dwCaps2;
		unsigned long reserved[2];
	};

	struct sb_DDSSurfaceDesc
	{
		unsigned long dwSize;
		unsigned long dwFlags;
		unsigned long dwHeight;	
		unsigned long dwWidth;
		unsigned long dwPitchSize;
		unsigned long dwDepth;
		unsigned long dwMipMapCount;
		unsigned long dwReserved[11];
		sb_DDSPixelFormat dwPixelFormat;
		sb_DDSCaps ddsCaps;
		unsigned long dwReserved2;
	};
public:
	Image_A8R8G8B8();
	virtual ~Image_A8R8G8B8();

	void		create(const size_t sx,const size_t sy);
	void		clear(const float r,const float g,const float b,const float a=1.0f);
	void		rect(const Rect& rect,const Color& color);

	inline	size_t			get_size_x() const { return _sx; }
	inline	size_t			get_size_y() const { return _sy; }
	
	//Acessors
	inline	void  set(const size_t x,const size_t y,const Color& color)
			{
				size_t n=((y*_sx)+x) << 2;

				_bitmap[n]=(unsigned char)(255.0f*color.b);
				_bitmap[n+1]=(unsigned char)(255.0f*color.g);
				_bitmap[n+2]=(unsigned char)(255.0f*color.r);
				_bitmap[n+3]=(unsigned char)(255.0f*color.a);
			}

	inline	void  set(const size_t x,const size_t y,const int r, const int g, const int b, const int a)
	{
		size_t n=((y*_sx)+x) << 2;

		_bitmap[n]=(unsigned char)(b);
		_bitmap[n+1]=(unsigned char)(g);
		_bitmap[n+2]=(unsigned char)(r);
		_bitmap[n+3]=(unsigned char)(a);
	}
	
	inline Color	get(const size_t x,const size_t y) const
			{
				size_t n=((y*_sx)+x) << 2;

				return Color(_bitmap[n],_bitmap[n+1],_bitmap[n+2],_bitmap[n+3]);
			};

	inline	unsigned char*	get_data() const { return _bitmap; }

	inline  void			set_data(const size_t sx,const size_t sy,unsigned char* data) { _sx=sx; _sy=sy; _bitmap=data;}

			void			bleed_on_alpha();
			void			bleed_on_alpha_protected();
			void			flip_x();
			void			flip_y();

			void			exchange_channels(const int chan1,const int chan2);
			Color			linear_sample(const size_t x,const size_t y,const float sx,const float sy);
			
			void			draw_line(const float px1,const float py1,const float px2,const float py2,const Color& c);
			void			blit(Image_A8R8G8B8* src,const int x,const int y);
			void			alpha_blit(Image_A8R8G8B8* src,const int x,const int y);

			void			premultiply_alpha();

			bool			load_dds(const char* filename);
protected:
			bool			read_dds_format_header(FILE* file,unsigned long* magic_number,sb_DDSSurfaceDesc* surfdesc) const;
protected:
	unsigned char*	_bitmap;
	size_t			_sx;
	size_t			_sy;
};

#endif // !defined(AFX_LDIMAGE2D_H__5974B88A_3029_43AB_8C08_6BAA4BFB9D35__INCLUDED_)
