
#include "Image_A8R8G8B8.h"
#include "Logger.h"

#include <xutility>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Image_A8R8G8B8::Image_A8R8G8B8()
{
	_bitmap=NULL;
	_sx=_sy=0;
}

Image_A8R8G8B8::~Image_A8R8G8B8()
{
	if (_bitmap) delete[] _bitmap;
}

void Image_A8R8G8B8::create(const size_t sx,const size_t sy)
{
	if (_bitmap) delete[] _bitmap;

	size_t n=(sx*sy) << 2;

	_bitmap=new unsigned char[n];
	_sx=sx;
	_sy=sy;
}

void Image_A8R8G8B8::clear(const float r,const float g,const float b,const float a)
{
	unsigned char bb=(unsigned char)(b*255.0f);
	unsigned char gg=(unsigned char)(g*255.0f);
	unsigned char rr=(unsigned char)(r*255.0f);
	unsigned char aa=(unsigned char)(a*255.0f);
	for (size_t i=0; i<(_sx*_sy) << 2; i+=4)
	{
		_bitmap[i]=bb;
		_bitmap[i+1]=gg;
		_bitmap[i+2]=rr;
		_bitmap[i+3]=aa;
	}
}

void Image_A8R8G8B8::exchange_channels(const int chan1,const int chan2)
{
	size_t n=0;

	for (size_t i=0; i<_sy*_sx; i++)
	{
		unsigned char tmp=_bitmap[n+chan1];
		_bitmap[n+chan1]=_bitmap[n+chan2];
		_bitmap[n+chan2]=tmp;

		n+=4;
	}	
}


void Image_A8R8G8B8::bleed_on_alpha()
{
	unsigned char* new_bitmap=new unsigned char[_sx*_sy*4];
	unsigned char* src=_bitmap;
	unsigned char* dest=new_bitmap;

	for (size_t i=0; i<_sx*_sy*4; i++) { *dest=0; dest++; }
	dest=new_bitmap;
	
	for (size_t yy=1; yy<_sy-1; yy++)
	{
		src=_bitmap+(yy*_sx+1)*4;
		dest=new_bitmap+(yy*_sx+1)*4;
		for (size_t xx=1; xx<_sx-1; xx++)
		{
			if (*(src+3)==0)
			{
				float	count=0.0f;
				float	r=0.0f;
				float	g=0.0f;
				float	b=0.0f;

				for (int dy=-1; dy<=1; dy++)
				{
					unsigned char* ptr=src+(dy*_sx-1)*4;
					for (int dx=-1; dx<=1; dx++)
					{
						if (ptr[3]>0)
						{
							count+=1.0f;
							r+=ptr[0];
							g+=ptr[1];
							b+=ptr[2];
						}
						ptr+=4;
					}
				}
				if (count==0.0f)
				{
					*dest=0; dest++;
					*dest=0; dest++;
					*dest=0; dest++;
					*dest=0; dest++;
				}
				else
				{
					*dest=(unsigned char)(r/count); dest++;
					*dest=(unsigned char)(g/count); dest++;
					*dest=(unsigned char)(b/count); dest++;
					*dest=255; dest++;
				}
				src+=4;
			}
			else 
			{
				*dest=*src; dest++; src++;
				*dest=*src; dest++; src++;
				*dest=*src; dest++; src++;
				*dest=*src; dest++; src++;
			}
		}
	}

	delete[] _bitmap;
	_bitmap=new_bitmap;
}

void Image_A8R8G8B8::bleed_on_alpha_protected()
{
	unsigned char* new_bitmap=new unsigned char[_sx*_sy*4];
	unsigned char* src=_bitmap;
	unsigned char* dest=new_bitmap;

	for (size_t i=0; i<_sx*_sy*4; i++) { *dest=0; dest++; }
	dest=new_bitmap;

	for (int yy=0; yy<(int)_sy; yy++)
	{
		for (int xx=0; xx<(int)_sx; xx++)
		{
			src=_bitmap+(yy*_sx+xx)*4;
			if (*(src+3)==0)
			{
				float	count=0.0f;
				float	r=0.0f;
				float	g=0.0f;
				float	b=0.0f;

				for (int dy=-1; dy<=1; dy++)
				{
					int py=yy+dy;
					if ((py<0) || (py>=(int)_sy)) continue;
					for (int dx=-1; dx<=1; dx++)
					{
						int px=xx+dx;
						if ((px<0) || (px>=(int)_sx)) continue;
						unsigned char* ptr=_bitmap+(py*_sx+px)*4;
						if (ptr[3]>0)
						{
							count+=1.0f;
							r+=ptr[0];
							g+=ptr[1];
							b+=ptr[2];
						}
						ptr+=4;
					}
				}
				if (count==0.0f)
				{
					*dest=0; dest++;
					*dest=0; dest++;
					*dest=0; dest++;
					*dest=0; dest++;
				}
				else
				{
					*dest=(unsigned char)(r/count); dest++;
					*dest=(unsigned char)(g/count); dest++;
					*dest=(unsigned char)(b/count); dest++;
					*dest=255; dest++;
				}
				src+=4;
			}
			else 
			{
				*dest=*src; dest++; src++;
				*dest=*src; dest++; src++;
				*dest=*src; dest++; src++;
				*dest=*src; dest++; src++;
			}
		}
	}

	delete[] _bitmap;
	_bitmap=new_bitmap;
}

void Image_A8R8G8B8::flip_x()
{
	unsigned long* src;
	unsigned long* dest;
	for (size_t yy=0; yy<_sy; yy++)
	{
		src=((unsigned long*)_bitmap)+(_sx*yy);
		dest=src+_sx-1;
		for (size_t i=0; i<_sx/2; i++)
		{
			unsigned long tmp=*dest;
			*dest=*src;
			*src=tmp;
			src++;
			dest--;
		}
	}
}

void Image_A8R8G8B8::flip_y()
{
	unsigned char* temp=(unsigned char*)malloc(_sx*4);
	unsigned char* src=_bitmap;
	unsigned char* dest=_bitmap+((_sy-1)*_sx*4);
	for (size_t i=0; i<_sy/2; i++)
	{
		memcpy(temp,src,_sx*4);
		memcpy(src,dest,_sx*4);
		memcpy(dest,temp,_sx*4);

		src+=_sx*4;
		dest-=_sx*4;
	}
	free(temp);
}

void Image_A8R8G8B8::rect(const Rect& rect,const Color& color)
{
	unsigned char* ptr;
	unsigned char  r=(unsigned char)(color.r*255.0f);
	unsigned char  g=(unsigned char)(color.g*255.0f);
	unsigned char  b=(unsigned char)(color.b*255.0f);
	unsigned char  a=(unsigned char)(color.a*255.0f);
	for (size_t yy=(size_t)rect.y1; yy<=(size_t)rect.y2; yy++)
	{
		ptr=_bitmap+((yy*_sx)+(size_t)rect.x1)*4;
		for (size_t xx=(size_t)rect.x1; xx<=(size_t)rect.x2; xx++)
		{
			*ptr=b; ++ptr;
			*ptr=g; ++ptr;
			*ptr=r; ++ptr;
			*ptr=a; ++ptr;
		}
	}
}

Color Image_A8R8G8B8::linear_sample(const size_t x,const size_t y,const float sx,const float sy)
{
	long x1=(long)floorf((float)x-((float)sx)*0.5f);
	long y1=(long)floorf((float)y-((float)sy)*0.5f);
	long x2=(long)floorf((float)x+((float)sx)*0.5f);
	long y2=(long)floorf((float)y+((float)sy)*0.5f);

	float	div=0.0f;
	Color accum(0.0f,0.0f,0.0f,0.0f);
	long wx,wy;
	for (long xx=x1; xx<=x2; xx++)
	{
		for (long yy=y1; yy<=y2; yy++)
		{
			wx=xx; wy=yy;
			while (wx>=(long)_sx) wx-=(long)_sx;
			while (wy>=(long)_sy) wy-=(long)_sy;
			while (wx<0) wx+=(long)_sx;
			while (wy<0) wy+=(long)_sy;
			Color c=get((size_t)wx,(size_t)wy);
			accum=accum+c;
			div+=1.0f;
		}
	}
	accum=accum/div;
	return accum;
}	

void Image_A8R8G8B8::draw_line(const float px1,const float py1,const float px2,const float py2,const Color& c)
{
	int		x1=(int)px1;
	int		y1=(int)py1;
	int		x2=(int)px2;
	int		y2=(int)py2;
	unsigned long	color=float_to_colorref(c);

	bool steep=abs(y2-y1)>abs(x2-x1);
	if (steep)
	{
		std::swap(x1,y1); std::swap(x2,y2);

		if (x1>x2) { std::swap(x1,x2); std::swap(y1,y2); }

		int			delta_x=x2-x1;
		int			delta_y=abs(y2-y1);
		int			error=delta_x>>1;
		int			ystep=(y1<y2)?(1):(-1);
		int			y=y1;
		unsigned long*	dest=((unsigned long*)_bitmap)+(x1*_sx+y1);

		for (int x=x1; x<x2; x++)
		{
			*dest=color;
			error=error-delta_y;

			dest+=_sx;
			if (error<0)
			{
				dest=dest+ystep;
				error+=delta_x;
			}
		}
	}
	else
	{
		if (x1>x2) { std::swap(x1,x2); std::swap(y1,y2); }

		int			delta_x=x2-x1;
		int			delta_y=abs(y2-y1);
		int			error=delta_x>>1;
		int			ystep=(y1<y2)?(1):(-1);
		int			y=y1;
		unsigned long*	dest=((unsigned long*)_bitmap)+(y1*_sx+x1);

		for (int x=x1; x<x2; x++)
		{
			*dest=color;
			error=error-delta_y;

			dest++;
			if (error<0)
			{
				dest+=_sx*ystep;
				error+=delta_x;
			}
		}
	}
}

void Image_A8R8G8B8::premultiply_alpha()
{
	unsigned char* ptr=_bitmap;
	for (size_t i=0; i<_sx*_sy; i++)
	{
		unsigned int a=*(ptr+3);
		*ptr=((*ptr)*a)>>8; ptr++;
		*ptr=((*ptr)*a)>>8; ptr++;
		*ptr=((*ptr)*a)>>8; ptr++;
		ptr++;
	}
}

bool Image_A8R8G8B8::read_dds_format_header(FILE* file,unsigned long* magic_number,sb_DDSSurfaceDesc* surfdesc) const
{
	if (!file){ log("Invalid file for reading DDS header"); return false; }
	
	fread((unsigned char*)&(magic_number),sizeof(unsigned long),1,file);
	fread((unsigned char*)&(surfdesc->dwSize),sizeof(unsigned long),1,file);
	fread((unsigned char*)&(surfdesc->dwFlags),sizeof(unsigned long),1,file);
	fread((unsigned char*)&(surfdesc->dwHeight),sizeof(unsigned long),1,file);
	fread((unsigned char*)&(surfdesc->dwWidth),sizeof(unsigned long),1,file);
	fread((unsigned char*)&(surfdesc->dwPitchSize),sizeof(unsigned long),1,file);
	fread((unsigned char*)&(surfdesc->dwDepth),sizeof(unsigned long),1,file);
	fread((unsigned char*)&(surfdesc->dwMipMapCount),sizeof(unsigned long),1,file);

	for (size_t i=0; i<11; ++i)
	{
		fread((unsigned char*)&(surfdesc->dwReserved[i]),sizeof(unsigned long),1,file);
	}

	fread((unsigned char*)&(surfdesc->dwPixelFormat.dwSize),sizeof(unsigned long),1,file);
	fread((unsigned char*)&(surfdesc->dwPixelFormat.dwFlags),sizeof(unsigned long),1,file);
	fread((unsigned char*)&(surfdesc->dwPixelFormat.dwFourCC),sizeof(unsigned long),1,file);
	fread((unsigned char*)&(surfdesc->dwPixelFormat.dwRGBBitCount),sizeof(unsigned long),1,file);
	fread((unsigned char*)&(surfdesc->dwPixelFormat.dwRBitMask),sizeof(unsigned long),1,file);
	fread((unsigned char*)&(surfdesc->dwPixelFormat.dwGBitMask),sizeof(unsigned long),1,file);
	fread((unsigned char*)&(surfdesc->dwPixelFormat.dwBBitMask),sizeof(unsigned long),1,file);
	fread((unsigned char*)&(surfdesc->dwPixelFormat.dwRGBAlphaBitMask),sizeof(unsigned long),1,file);

	fread((unsigned char*)&(surfdesc->ddsCaps.dwCaps1),sizeof(unsigned long),1,file);
	fread((unsigned char*)&(surfdesc->ddsCaps.dwCaps2),sizeof(unsigned long),1,file);
	fread((unsigned char*)&(surfdesc->ddsCaps.reserved[0]),sizeof(unsigned long),1,file);
	fread((unsigned char*)&(surfdesc->ddsCaps.reserved[1]),sizeof(unsigned long),1,file);

	fread((unsigned char*)&(surfdesc->dwReserved2),sizeof(unsigned long),1,file);

	return true;
}

bool Image_A8R8G8B8::load_dds(const char* filename)
{
	if (_bitmap) delete[] _bitmap;
	_bitmap=NULL;

	unsigned long		magic_number;
	sb_DDSSurfaceDesc	surfdesc;

	FILE* file=NULL;
	fopen_s(&file,filename,"rb");
	if (!file) return false;

	if (!read_dds_format_header(file,&magic_number,&surfdesc)) return false;

	bool	alpha_present=false;
	int		bytes_per_pixel=4;
	//& SBDDPF_RGB;
	if (surfdesc.dwPixelFormat.dwFlags&0x00000040)
	{
		if (surfdesc.dwPixelFormat.dwRGBBitCount==32) 
		{
			if (surfdesc.dwPixelFormat.dwFlags&0x0000001) alpha_present=true;
			else alpha_present=false;
			if (!(surfdesc.dwFlags&0x00000008)) surfdesc.dwPitchSize=surfdesc.dwWidth*4;
		}
		else if (surfdesc.dwPixelFormat.dwRGBBitCount==24) 
		{
			alpha_present=false;
			bytes_per_pixel=3;
			if (!(surfdesc.dwFlags&0x00000008)) surfdesc.dwPitchSize=surfdesc.dwWidth*3;
		}
		else
		{
			log("DDS image sub-type not supported in file \"%s\"!",filename);
			return false;
		}
	}
	else
	{
		log("DDS compressed images not supported in file \"%s\"!",filename);
		return NULL;
	}

	create(surfdesc.dwWidth,surfdesc.dwHeight);
	unsigned char* buffer=get_data();

	unsigned char* tmp=(unsigned char*)malloc(sizeof(unsigned char)*surfdesc.dwPitchSize);

	for (size_t i=0; i<(size_t)surfdesc.dwHeight; ++i)
	{
		memset(tmp,0,sizeof(unsigned char)*surfdesc.dwPitchSize);
		size_t bytes_read;
		bytes_read=fread(tmp,sizeof(unsigned char),surfdesc.dwPitchSize,file); 

		if (bytes_per_pixel==4)
		{
			if (alpha_present)
			{
				memcpy(buffer,tmp,sizeof(unsigned char)*surfdesc.dwWidth*4);
				buffer=buffer+surfdesc.dwWidth*4;
			}
			else
			{
				unsigned char* tmp2=tmp;
				for (size_t j=0; j<(size_t)surfdesc.dwWidth; ++j)
				{
					*(buffer)=*(tmp2);
					*(buffer+1)=*(tmp2+1);
					*(buffer+2)=*(tmp2+2);
					*(buffer+3)=255;
					tmp2+=4;
					buffer+=4;
				}
			}
		}
		else
		{
			unsigned char* tmp2=tmp;
			for (size_t j=0; j<(size_t)surfdesc.dwWidth; ++j)
			{
				*(buffer)=*(tmp2);
				*(buffer+1)=*(tmp2+1);
				*(buffer+2)=*(tmp2+2);
				*(buffer+3)=255;
				tmp2+=3;
				buffer+=4;
			}
		}
	}

	free(tmp);
	fclose(file);

	return true;
}

void Image_A8R8G8B8::blit(Image_A8R8G8B8* src,const int x,const int y)
{
	unsigned char*	src_data=src->get_data();
	unsigned char*	dest_data=get_data()+((y*_sx)+x)*4;
	size_t			src_pitch=src->get_size_x()*4;

	for (size_t y=0; y<src->get_size_y(); y++)
	{
		memcpy(dest_data,src_data,src_pitch);
		src_data+=src_pitch;
		dest_data+=(_sx*4);
	}
}

void Image_A8R8G8B8::alpha_blit(Image_A8R8G8B8* src,const int x,const int y)
{
	unsigned char*	src_data=src->get_data();
	unsigned char*	dest_data=get_data()+((y*_sx)+x)*4;
	unsigned char*	tmp;
	size_t			src_pitch=src->get_size_x()*4;
	size_t			xx,yy;
	unsigned char	r,g,b,a;
	float			aa;

	for (yy=0; yy<src->get_size_y(); yy++)
	{
		tmp=dest_data;
		for (xx=0; xx<src->get_size_x(); xx++)
		{
			a=src_data[0]; r=src_data[1]; g=src_data[2]; b=src_data[3];
			aa=((float)a)/255.0f;
			tmp[0]=(unsigned char)(tmp[0]*(1.0f-aa)+a*aa);
			tmp[1]=(unsigned char)(tmp[1]*(1.0f-aa)+r*aa);
			tmp[2]=(unsigned char)(tmp[2]*(1.0f-aa)+g*aa);
			tmp[3]=(unsigned char)(tmp[3]*(1.0f-aa)+b*aa);

			tmp+=4;
			src_data+=4;
		}
		dest_data+=(_sx*4);
	}
}
