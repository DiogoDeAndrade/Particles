#pragma once

#include "Screen.h"

class MouseCursor;
class ParticleCloud;
class Explosions;
class Sprite;

class GameScreen :
	public Screen
{
protected:
	typedef enum { ELEMENT_UNKNOWN, ELEMENT_BLOCK, ELEMENT_ATTRACTOR, ELEMENT_FLOOR } ElementType;
	struct Element
	{
		ElementType		_type;
		Sprite*			_sprite;
		Vec2f			_start_pos;
		float			_start_time;
		float			_interval;
		Vec2f			_delta;
		float			_speed;
		unsigned char	_block_color;
		float			_strength;
		int				_attractor_id;
		float			_range;
		float			_duration;
		bool			_random;
	};
	typedef std::map<char,Element>	Elements;
	struct Tutorial
	{
		Vec3f			_pos;
		std::string		_text;
	};
	typedef std::vector<Tutorial>	Tutorials;
public:
	GameScreen(void);
	virtual ~GameScreen(void);

			bool	init(const int level,const int tries,const int pts,const int highscore);
			void	close();
	virtual void	render();
	virtual void	anim(float t);
	virtual	void	mouse_callback(unsigned long msg,unsigned long wparam,unsigned long lparam);
	
			int		get_color(const float x,const float y,const bool dynamics);
			void	new_explosion(const Vec3f& pos,const unsigned long color1,const unsigned long color2);
	inline	int		get_level_score() { return _level_score; }
	inline	bool	full_success() { return _full_success; }
protected:
			void	load_level(const int level);
			bool	is_solid(const int tx,const int ty,const unsigned char block_color);
			void	anim_block(const float t,Element* elem);
			void	new_dialog(const char* text,const float x,const float y,const int dialog_opt,const float trigger_time);
			void	next_tutorial();
public:
	static	GameScreen*	OBJ;
protected:
	MouseCursor*		_cursor;
	float				_color_change;
	ParticleCloud*		_particles;
	Explosions*			_explosions;
	int					_attractor_id;
	int					_sx,_sy,_start_x,_start_y,_end_x,_end_y;
	unsigned char*		_map;
	float				_color_cycle;
	Elements			_elements;
	bool				_spawn_color[3];
	float				_dialog_trigger_time;
	std::string			_dialog_text;
	Vec3f				_dialog_pos;
	int					_dialog_opt;
	bool				_pause;
	int					_tries;
	int					_pts;
	int					_level_score;
	int					_highscore;
	bool				_full_success;
	std::string			_last_clock;
	Tutorials			_tutorials;
	size_t				_tutorial_index;
	unsigned long		_time_since_last_key;
	bool				_potato;
};
