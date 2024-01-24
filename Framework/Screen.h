// Screen.h: interface for the Screen class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SCREEN_H__676B95DB_390F_4F6B_9405_CB63985E87C3__INCLUDED_)
#define AFX_SCREEN_H__676B95DB_390F_4F6B_9405_CB63985E87C3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Drawable.h"

#define TEXT_NORMAL	0
#define TEXT_CENTER	1
#define TEXT_BOTTOM 2
#define TEXT_SHADOW 4

class PrioritySort
{
public:
	bool operator()(Drawable* obj1,Drawable* obj2)
	{
		if (!obj1) return false;
		if (!obj2) return true;
		if (obj1->get_priority()>obj2->get_priority())
		{
			return true;
		}
		return false;
	}
};

class Screen  
{
protected:
	struct RemoveElem
	{
		Drawable*	drawable;
		bool		should_delete;
	};
	typedef std::vector<RemoveElem>	Removes;
	typedef std::vector<Drawable*>	DrawableVector;
public:
	Screen();
	virtual ~Screen();

	virtual	long	run();
			void	run(float t);
	virtual void	render() = 0;
	virtual void	anim(float t) = 0;
			
	inline	void	set_exit(bool b,long exit_code) { _exit=b; _exit_code=exit_code; }

	inline	void	add_drawable(Drawable* drawable) { _drawables.push_back(drawable); }
	inline	void	remove_drawable(Drawable* drawable,const bool should_delete=false) 
					{ 
						RemoveElem elem;
						elem.drawable=drawable;
						elem.should_delete=should_delete;
						_removes.push_back(elem);
					}
			void	remove_drawables();

			bool	create_font();
			void	delete_font();
			void	write_text(float x,float y,float r,float g,float b,float a,const char *string,const unsigned long flags=TEXT_NORMAL);
			Rect	get_text_rect(float x,float y,const char *string,const unsigned long flags=TEXT_NORMAL);

	virtual	void	mouse_callback(unsigned long msg,unsigned long wparam,unsigned long lparam);

public:
	static	void	load_assets(std::string filename);

protected:
	bool			_exit;
	DrawableVector	_drawables;
	Removes			_removes;
	long			_exit_code;
	LPD3DXFONT		_font;
	UINT			_font_size;
	LPD3DXSPRITE	_sprite;
	D3DXVECTOR2		_desktop_res;
};

extern Screen*	SCREEN;

#endif // !defined(AFX_SCREEN_H__676B95DB_390F_4F6B_9405_CB63985E87C3__INCLUDED_)
