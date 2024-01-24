#pragma once

#include <map>
#include "MathLib.h"

extern int					g_highscore;
extern std::map<int,bool>	g_gold_levels;
extern int					g_level;
extern int					g_retries;
extern bool					g_colorblind;
extern bool					g_tutorial;
extern bool					g_sound;

void load_options();
void save_options();

unsigned long	get_particle_color(const int type);
Color			get_cursor_color(const int type);
Color			get_block_color(const int type);
Color			get_wall_color(const int type);
bool			is_music_enabled();
void			set_music_enable(const bool b);
void			play_sound(const char* sound_name,const float vol,const unsigned long min_time=0);
