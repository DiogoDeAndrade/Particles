
#include "Gfx.h"
#include "TextureManager.h"
#include "BlitManager.h"
#include "BlitManager3d.h"
#include "Keyboard.h"
#include "Sound.h"
#include "Timers.h"
#include "Utils.h"
#include "Logger.h"
#include "XML.h"

#include "GameScreen.h"
#include "TitleScreen.h"
#include "EndGameScreen.h"
#include "Options.h"

bool init_gfx(long res_x,long res_y,bool windowed,HINSTANCE hInst)
{
	new Gfx;

	GFX->set_window_mode(windowed);
	GFX->set_resolution(res_x,res_y);
	if (!GFX->init(hInst)) return false;

	new TextureManager(GFX->get_device());
	new BlitManager(GFX->get_device());
	new BlitManager3d(GFX->get_device());

	return true;
}

bool init_input(HINSTANCE hInst)
{
	new Keyboard;

	if (!KEY->init(hInst)) return false;

	return true;
}

bool init_sound()
{
	new Sound;

	if (!SOUND->init()) 
	{
		ERROR_MSG("Couldn't start sound... Swarm will be silent...");
	}

	return true;
}

bool init(long res_x,long res_y,bool windowed,HINSTANCE hInst)
{
	if (!init_gfx(res_x,res_y,windowed,hInst)) return false;
	if (!init_input(hInst)) return false;
	if (!init_sound()) return false;

	new Timers;

	return true;
}

void shutdown()
{
	delete SOUND;
	delete TIMERS;
	delete KEY;
	delete BLIT;
	delete TEXTURES;
	delete GFX;
}

INT WINAPI WinMain(HINSTANCE hInst,HINSTANCE,LPSTR,INT)
{
	long res_x=1280;
	long res_y=1024;
	bool windowed=false;

/*	char buffer[512];
	FILE* file=NULL;
	fopen_s(&file,"res.txt","rt");
	if (file)
	{
		fgets((char*)&buffer,512,file); res_x=atol(buffer);
		fgets((char*)&buffer,512,file); res_y=atol(buffer);
		fgets((char*)&buffer,512,file); windowed=(atol(buffer)==0);
		fclose(file);
	}
	else
	{*/
		res_x=1024;
		res_y=768;
		windowed=true;
//	}

	if (!init(res_x,res_y,windowed,hInst)) return -1;

	Screen::load_assets("assets.dat");

	int					ret=0;

	g_level=1;	
	load_options();
	
	while (ret!=-1)
	{
		if (is_music_enabled())
		{
			set_music_enable(true);
		}

		TitleScreen* title=new TitleScreen;
		title->init(g_level);
		title->create_font();
		ret=title->run();
		if (ret==0) g_level=1;
		delete title;

		save_options();
		
		int pts=0;

		if (ret!=-1)
		{
			while (1)
			{
				GameScreen* game=new GameScreen;
				game->init(g_level,g_retries,pts,g_highscore);
				game->create_font();
				ret=game->run();
				int level_score=game->get_level_score();
				if (game->full_success())
				{
					if (g_gold_levels.count(g_level)==0)
					{
						g_gold_levels.insert(std::make_pair(g_level,true));
						save_options();
					}
				}
				game->close();
				delete game;
				
				if (ret==0)
				{
					g_retries++;
				}
				else if (ret==1)
				{
					pts+=level_score;
					if (g_highscore<pts) g_highscore=pts;
					g_retries=0;
					g_level++;

					// Check if there is another level
					char buffer[512];
					sprintf_s((char*)&buffer,512,"levels/level%02i.xml",g_level);
					FILE* file=NULL;
					fopen_s(&file,buffer,"rt");
					if (!file)
					{
						// No more levels, check if elligeable for potato level
						if (g_level==100)
						{
							EndGameScreen* end_game=new EndGameScreen;
							end_game->init(99);
							end_game->create_font();
							ret=end_game->run();
							delete end_game;
							g_level=1;
							ret=0;
							break;
						}
						else
						{
							bool eligeable=true;
							for (int i=1; i<g_level; i++)
							{
								if (g_gold_levels.find(i)==g_gold_levels.end())
								{
									eligeable=false;
									break;
								}
							}
							if (eligeable)
							{
								g_level=99;
							}
							else
							{
								EndGameScreen* end_game=new EndGameScreen;
								end_game->init(g_level);
								end_game->create_font();
								ret=end_game->run();
								delete end_game;
								g_level=1;
								ret=0;
								break;
							}
						}
					}
				}
				else if (ret==-1)
				{
					break;
				}
				else if (ret==-2)
				{
					ret=0;
					break;
				}

				save_options();
			}
		}
	}
	
	shutdown();

	return 0;
}

