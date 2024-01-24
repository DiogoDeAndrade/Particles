#include "GameScreen.h"
#include "Gfx.h"
#include "Keyboard.h"
#include "Camera.h"
#include "Light.h"
#include "BlitManager.h"
#include "BlitManager3d.h"
#include "TextureManager.h"
#include "ParticleSystem3d.h"
#include "Mouse.h"
#include "Sound.h"
#include "MouseCursor.h"
#include "XML.h"
#include "Utils.h"

#include "ParticleCloud.h"
#include "Explosions.h"
#include "Options.h"

GameScreen*	GameScreen::OBJ=NULL;

GameScreen::GameScreen(void)
{
	OBJ=this;
}

GameScreen::~GameScreen(void)
{
	OBJ=NULL;
}

bool GameScreen::init(const int level,const int tries,const int pts,const int highscore)
{
	_dialog_text="";
	_pause=false;
	_dialog_trigger_time=0.0f;
	_tries=tries;
	_pts=pts;
	_highscore=highscore;
	_full_success=false;
	_last_clock="";
	_potato=false;
	_time_since_last_key=timeGetTime();

	create_font();

	// Load level
	load_level(level);

	_cursor=new MouseCursor("mouse_cursor");
	_cursor->set_hotspot(16.0f,16.0f);
	_cursor->set_scale(1.0f);

	_cursor->set_color(0.9f,0.9f,0.9f,1.0f);
	_color_change=0.0f;

	if ((_start_x==-1) || (_start_y==-1)) _particles=new ParticleCloud(1000);
	else _particles=new ParticleCloud(200,_start_x,_start_y,30,_spawn_color);
	_attractor_id=_particles->new_attractor(5,16.0f,1.0f);
	_particles->anim(0.0f);

	_color_change=3000.0f;

	_explosions=new Explosions;

	auto it=_elements.begin();
	while (it!=_elements.end())
	{
		if (it->second._type==ELEMENT_ATTRACTOR)
		{
			it->second._attractor_id=_particles->new_attractor(it->second._block_color,20.0f,it->second._strength);
			_particles->set_attractor_pos(it->second._attractor_id,it->second._sprite->_pos+Vec3f(16.0f,16.0f,0.0f));
			_particles->set_attractor_range(it->second._attractor_id,it->second._range);
		}

		++it;
	}

	next_tutorial();
	
	return true;
}

void GameScreen::next_tutorial()
{
	if ((_tutorial_index<_tutorials.size()) && (g_tutorial))
	{
		Vec3f p=_tutorials[_tutorial_index]._pos;
		new_dialog(_tutorials[_tutorial_index]._text.c_str(),p.x,p.y,3,0.0f);
		_tutorial_index++;
	}
	else new_dialog("Click to start!",512.0f,384.0f,1,0);

}

void GameScreen::render()
{
	GFX->begin_scene();
	///////////////////////////////////////////
	// 2d part
	Vec2f res=GFX->get_resolution();

	GFX->clear(D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER,1.0f,1.0f,1.0f,1.0f,1.0f,0);
	GFX->set_2d_render(1.0f);
	GFX->set_camera(res.x*0.5f,res.y*0.5f,0.0f);

	unsigned char*	tmp=_map;
	unsigned char	cc;
	Color			c;
	int				img=TEXTURES->find_by_name("potato");
	for (int y=0; y<_sy; y++)
	{
		for (int x=0; x<_sx; x++)
		{
			cc=*tmp;
			switch (cc)
			{
				case 1:
				case 3:
				case 4:
				case 5:
					c=get_wall_color(cc);
					if (_potato)
					{
						if (cc==1) c.set(1.0f,1.0f,1.0f,1.0f);
						BLIT->add(img,x*32.0f,y*32.0f,0.9f,32.0f,32.0f,c.r,c.g,c.b,c.a,0.0f,0.0f,1.0f,1.0f);
					}
					else 
					if ((cc==1) && (_potato)) c.set(1.0f,1.0f,1.0f,1.0f);
					BLIT->add(-1,x*32.0f,y*32.0f,0.9f,32.0f,32.0f,c.r,c.g,c.b,c.a,0.0f,0.0f,1.0f,1.0f);
					break;
			}
			
			tmp++;
		}
	}

	if ((_end_x!=-1) && (_end_y!=-1))
	{
		Color c[]= { Color(1.0f,0.0f,0.0f,1.0f),Color(1.0f,1.0f,0.0f,1.0f),Color(0.0f,1.0f,0.0f,1.0f),Color(0.0f,1.0f,1.0f,1.0f),Color(0.0f,0.0f,1.0f,1.0f),Color(1.0f,0.0f,1.0f,1.0f) };

		_color_cycle=_color_cycle-0.1f;
		if (_color_cycle<0.0f) _color_cycle=6.0f;
		int idx=(int)(floorf(_color_cycle));
		BLIT->add(_end_x-16.0f,_end_y-16.0f,0.82f,32.0f,32.0f,Color(0.0f,0.0f,0.0f,1.0f));
		BLIT->add(_end_x-14.0f,_end_y-14.0f,0.80f,28.0f,28.0f,c[(idx%6)]);
		BLIT->add(_end_x-12.0f,_end_y-12.0f,0.78f,24.0f,24.0f,c[(idx+1)%6]);
		BLIT->add(_end_x-10.0f,_end_y-10.0f,0.76f,20.0f,20.0f,c[(idx+2)%6]);
		BLIT->add(_end_x- 8.0f,_end_y- 8.0f,0.74f,16.0f,16.0f,c[(idx+3)%6]);
		BLIT->add(_end_x- 6.0f,_end_y- 6.0f,0.72f,12.0f,12.0f,c[(idx+4)%6]);
		BLIT->add(_end_x- 4.0f,_end_y- 4.0f,0.70f, 8.0f, 8.0f,c[(idx+5)%6]);
	}

	auto	it=_elements.begin();
	while (it!=_elements.end())
	{
		if (it->second._sprite)
		{
			it->second._sprite->draw();
		}

		++it;
	}

	_particles->draw();
	_cursor->draw();
	_explosions->draw();

	float dialog_y;
	if ((_dialog_text!="") && (_dialog_trigger_time==0.0f))
	{
		Rect text_rect=get_text_rect(_dialog_pos.x,_dialog_pos.y,_dialog_text.c_str(),TEXT_CENTER);
		text_rect.x1-=20; text_rect.x2+=20;
		text_rect.y1-=20; text_rect.y2+=20;

		// Do a centering of the dialog box
		float height=text_rect.y2-text_rect.y1;
		text_rect.y1=_dialog_pos.y-height*0.5f;
		text_rect.y2=_dialog_pos.y+height*0.5f;

		BLIT->add(text_rect.x1,text_rect.y1,_dialog_pos.z,text_rect.x2-text_rect.x1,text_rect.y2-text_rect.y1,Color(0.2f,0.2f,0.2f,1.0f));
		BLIT->add(text_rect.x1-1.0f,text_rect.y1-1.0f,_dialog_pos.z+0.02f,text_rect.x2-text_rect.x1+2.0f,text_rect.y2-text_rect.y1+2.0f,Color(0.0f,0.0f,0.0f,1.0f));

		dialog_y=text_rect.y1+20.0f;
	}

/*	BLIT->add(924.0f,660.0f,0.2f,100.0f,100.0f,Color(0.0f,0.0f,0.0f,0.2f));

	BLIT->add(970.0f,670.0f,0.15f,40.0f,20.0f,Color(1.0f,0.0f,0.0f,1.0f));
	BLIT->add(970.0f,695.0f,0.15f,40.0f,20.0f,Color(0.0f,1.0f,0.0f,1.0f));
	BLIT->add(970.0f,720.0f,0.15f,40.0f,20.0f,Color(0.0f,1.0f,1.0f,1.0f));
	BLIT->add(970.0f,745.0f,0.15f,40.0f,20.0f,Color(1.0f,1.0f,1.0f,1.0f));*/

	BLIT->render();
	BLIT->clear();

	if ((_dialog_text!="") && (_dialog_trigger_time==0.0f))
	{
		write_text(_dialog_pos.x,dialog_y,1.0f,1.0f,1.0f,1.0f,_dialog_text.c_str(),TEXT_CENTER|TEXT_SHADOW);
	}

	char	buffer[512];
	int		p1=(int)(_particles->get_alive_particle_percentage()*100.0f);
	int		p2=(int)(_particles->get_saved_particles_percentage()*100.0f);
	sprintf_s((char*)&buffer,512,"Particles: %i%%",p1);
	write_text(0.0f,res.y-30.0f,1.0f,1.0f,1.0f,1.0f,buffer);
	sprintf_s((char*)&buffer,512,"Saved: %i%%",p2);
	write_text(400.0f,res.y-30.0f,1.0f,1.0f,1.0f,1.0f,buffer);

/*	if (_dialog_text!="")
	{
		sprintf_s((char*)&buffer,512,"Dialog timer: %f",_dialog_trigger_time);
		write_text(800.0f,res.y-30.0f,1.0f,1.0f,1.0f,1.0f,buffer);
	}*/

/*	write_text(940.0f,667.0f,1.0f,1.0f,1.0f,1.0f,"1:");
	write_text(940.0f,692.0f,1.0f,1.0f,1.0f,1.0f,"2:");
	write_text(940.0f,717.0f,1.0f,1.0f,1.0f,1.0f,"3:");
	write_text(940.0f,742.0f,1.0f,1.0f,1.0f,1.0f,"4:");*/

	if (!_pause)
	{
		if (_color_change>0.0f)
		{
			Color c=_cursor->get_color();

			sprintf_s((char*)&buffer,512,"%is",(int)(ceilf((_color_change/1000.0f))));
			std::string tmp=buffer;
			if (_last_clock!=tmp) play_sound("tick",0.5f);
			_last_clock=buffer;
			write_text(_cursor->_pos.x,_cursor->_pos.y-40.0f,c.r,c.g,c.b,1.0f,buffer,TEXT_CENTER|TEXT_SHADOW);
		}
	}
	
	GFX->end_scene();
}

void GameScreen::anim(float t)
{
	_cursor->anim(t);

	if ((timeGetTime()-_time_since_last_key)>500)
	{
		if (KEY->is_pressed(DIK_R))
		{
			set_exit(true,0);
			_time_since_last_key=timeGetTime();
		}
		if (KEY->is_pressed(DIK_M))
		{
			set_music_enable(!is_music_enabled());
			_time_since_last_key=timeGetTime();
		}
		if (KEY->is_pressed(DIK_S))
		{
			g_sound=!g_sound;
			_time_since_last_key=timeGetTime();
		}
	}

	if (_dialog_trigger_time>0.0f)
	{
		_dialog_trigger_time-=t;
		if (_dialog_trigger_time<=0.0f) 
		{
			_dialog_trigger_time=0.0f;
			_pause=true;
		}
	}

	if (_pause) return;

#ifdef _DEBUG
	if (KEY->is_pressed(DIK_L)) set_exit(true,1);
	if (KEY->is_pressed(DIK_1)) _particles->set_attractor_type(_attractor_id,1);
	if (KEY->is_pressed(DIK_2)) _particles->set_attractor_type(_attractor_id,2);
	if (KEY->is_pressed(DIK_3)) _particles->set_attractor_type(_attractor_id,3);
	if (KEY->is_pressed(DIK_4)) _particles->set_attractor_type(_attractor_id,4);
	if (KEY->is_pressed(DIK_E)) new_explosion(Vec3f(100.0f,100.0f,0.5f),0xFFFFFF00,0xFFFF0000); 
#endif
	if (KEY->is_pressed(DIK_ESCAPE)) set_exit(true,-2);
	if (KEY->is_pressed(DIK_P))
	{
		new_dialog("Paused!\nClick to resume...",512.0f,384.0f,1,0.0f);
		return;
	}

	if (_color_change>0.0f)
	{
		_color_change-=t;
		if (_color_change<=0.0f)
		{
			_particles->set_attractor_type(_attractor_id,4);
			_color_change=0.0f;
		}
	}

	_explosions->anim(t);

	auto	it=_elements.begin();
	while (it!=_elements.end())
	{
		if (it->second._speed!=0.0f) anim_block(t,&(it->second));
		
		if (it->second._type==ELEMENT_FLOOR)
		{
			// Floor element, check if cursor is touching it
			Rect r;
			r.x1=it->second._sprite->_pos.x;
			r.y1=it->second._sprite->_pos.y;
			r.x2=r.x1+32.0f;
			r.y2=r.y1+32.0f;

			if (r.inside(Vec2f(_cursor->_pos.x,_cursor->_pos.y))) 
			{
				// Cursor change color for the duration
				_color_change=it->second._duration;
				_particles->set_attractor_type(_attractor_id,it->second._block_color);
			}
		}

		++it;
	}

	unsigned char	c=_particles->get_attractor_type(_attractor_id);
	Color			cursor_color=get_cursor_color(c);
	_cursor->set_color(cursor_color.r,cursor_color.g,cursor_color.b,cursor_color.a);

	_particles->set_attractor_pos(_attractor_id,_cursor->_pos);
	_particles->anim(t);

	// Check success
	if (_dialog_trigger_time==0.0f)
	{
		if (_particles->get_alive_particles()==0)
		{
			float saved=_particles->get_saved_particles_percentage();
			if (saved>0.5f)
			{
				// Success
				char		buffer[512];
				std::string	tmp="";
				int			pts=(int)((saved-0.5f)*1000.0f);
				sprintf_s((char*)&buffer,512,"Particles saved: %i%%\n",((int)(saved*100.0f)));
				tmp=tmp+buffer;
				sprintf_s((char*)&buffer,512,"Points: %i pts\n",pts);
				tmp=tmp+buffer;
				if (saved==1.0f)
				{
					pts+=500;
					sprintf_s((char*)&buffer,512,"Perfect bonus: 500 pts\n");
					tmp=tmp+buffer;

					_full_success=true;
				}
				sprintf_s((char*)&buffer,512,"Tries: %i\n",_tries);
				tmp=tmp+buffer;
				if (_tries>0)
				{
					float penalty=(1.0f/((float)_tries+1.0f));
					sprintf_s((char*)&buffer,512,"Tries Penalty: %i%%\n",((int)((1.0f-penalty)*100.0f)));
					tmp=tmp+buffer;
					pts=(int)(pts*penalty);
				}
				sprintf_s((char*)&buffer,512,"Total Points: %i pts\n",pts);
				tmp=tmp+buffer;
				sprintf_s((char*)&buffer,512,"Current Score: %i pts\n",_pts+pts);
				tmp=tmp+buffer;
				if ((_pts+pts)>_highscore)
				{
					sprintf_s((char*)&buffer,512,"This is a new highscore!\n");
					tmp=tmp+buffer;
				}
				else
				{
					sprintf_s((char*)&buffer,512,"Highest score: %i pts!\n",_highscore);
					tmp=tmp+buffer;
				}
				tmp=tmp+"\nPress 'R' to retry, or click for next level!";

				_level_score=pts;

				new_dialog(tmp.c_str(),512.0f,384.0f,2,1000.0f);
			}
			else
			{
				// Failure
				new_dialog("You failed to save enough particles!\nPress 'R' to retry!",512.0f,384.0f,0,1000.0f);
			}
		}
	}
}

void GameScreen::anim_block(const float t,Element* elem)
{
	float tt=t;

	if (elem->_start_time>0.0f)
	{
		elem->_start_time-=t;
		tt=-elem->_start_time;
	}
	if (tt<=0.0f) return;

	// If block is moving...
	if ((elem->_delta.x!=0.0f) || (elem->_delta.y!=0.0f))
	{
		// Move block
		Vec3f new_pos=elem->_sprite->_pos+Vec3f(elem->_delta.x*elem->_speed*t,elem->_delta.y*elem->_speed*t,0.0f);
		Vec3f offset_pos=new_pos+Vec3f(16.0f+elem->_delta.x*16.0f,16.0f+elem->_delta.y*16.0f,0.0f);

		int		color=get_color(offset_pos.x,offset_pos.y,false);
		if (((color!=-1) && (color!=elem->_block_color)) || (color==0))
		{
			// Hit something, clamp pos
			offset_pos=new_pos+Vec3f(16.0f,16.0f,0.0f);
			elem->_sprite->_pos.x=32.0f*floorf(offset_pos.x/32.0f);
			elem->_sprite->_pos.y=32.0f*floorf(offset_pos.y/32.0f);
			elem->_delta.set(0.0f,0.0f);
			elem->_start_time=elem->_interval;
		}
		else
		{
			elem->_sprite->_pos=new_pos;
		}
	}
	else
	{
		// Decide to move
		int tx=(int)floorf(elem->_sprite->_pos.x/32.0f);
		int ty=(int)floorf(elem->_sprite->_pos.y/32.0f);
		int dx=0,dy=0;

		int r=0;
		if (elem->_random) r=rand()%4+1;

		if ((tx>0) && ((r==0) || (r==1)))
		{ 
			if (!is_solid(tx-1,ty,elem->_block_color)) 
			{ 
				dx=-1; dy=0; 
			} 
		}
		if ((ty>0) && ((r==0) || (r==2))) 
		{ 
			if (!is_solid(tx,ty-1,elem->_block_color)) 
			{ 
				dx=0; dy=-1; 
			} 
		}
		if ((tx<(_sx-1)) && ((r==0) || (r==3)))
		{ 
			if (!is_solid(tx+1,ty,elem->_block_color)) 
			{ 
				dx=1; dy=0; 
			} 
		}
		if ((ty<(_sy-1)) && ((r==0) || (r==4)))
		{ 
			if (!is_solid(tx,ty+1,elem->_block_color)) 
			{ 
				dx=0; dy=1; 
			} 
		}
		
		elem->_delta.set((float)dx,(float)dy);
	}

	if (elem->_type==ELEMENT_ATTRACTOR)
	{
		_particles->set_attractor_pos(elem->_attractor_id,elem->_sprite->_pos);
	}
}

void GameScreen::mouse_callback(unsigned long msg,unsigned long wparam,unsigned long lparam)
{
	Screen::mouse_callback(msg,wparam,lparam);

	if (msg==WM_LBUTTONUP)
	{
		if (_dialog_text!="")
		{
			if (_dialog_opt!=0)
			{
				_dialog_text="";
				_pause=false;
			}
			if (_dialog_opt==2)
			{
				set_exit(true,1);
			}
			if (_dialog_opt==3)
			{
				next_tutorial();
			}
		}
#ifdef _DEBUG
		else
		{
			unsigned char c=_particles->get_attractor_type(_attractor_id);
			c=(c+1)%4; if (c==0) c=1;
			_particles->set_attractor_type(_attractor_id,c);
		}
#endif
	}
	else if (msg==WM_ACTIVATE)
	{
		if (LOWORD(wparam)==WA_INACTIVE)
		{
			if (_dialog_text=="") new_dialog("Paused!\nClick to resume...",512.0f,384.0f,1,0.0f);
		}
	}
}

void GameScreen::load_level(const int level)
{
	char buffer[512];

#ifdef _DEBUG
	int l=level;
	
	FILE* file=NULL;
	do
	{
		sprintf_s((char*)&buffer,512,"levels/level%02i.xml",l);
		fopen_s(&file,buffer,"rt");
		if (!file) l++;
	}
	while (!file);
	fclose(file);
#else
	sprintf_s((char*)&buffer,512,"levels/level%02i.xml",level);
#endif
	XMLNode root=XMLNode::openFileHelper(buffer);
	XMLNode level_node=root.getChildNode("level");
	std::string tmp=level_node.getStringAttribute("theme");
	if (tmp=="potato")
	{
		_potato=true;
	}
	XMLNode map_node=level_node.getChildNode("map");
	_sx=map_node.getIntAttribute("sx");
	_sy=map_node.getIntAttribute("sy");
	_map=(unsigned char*)malloc(_sx*_sy);
	memset(_map,0,_sx*_sy);

	_start_x=_start_y=-1;
	_end_x=_end_y=-1;

	for (int i=0; i<map_node.nChildNode("line"); i++)
	{
		XMLNode		line=map_node.getChildNode("line",i);
		std::string	tmp=line.getSafeText();
		for (int x=0; x<_sx; x++)
		{
			char c=tmp[x];
			if (c=='#') _map[i*_sx+x]=1;			
			else if (c=='S')
			{
				_start_x=(int)(x*32.0f+16.0f);
				_start_y=(int)(i*32.0f+16.0f);
			}
			else if (c=='X')
			{
				_end_x=(int)(x*32.0f+16.0f);
				_end_y=(int)(i*32.0f+16.0f);

				_map[i*_sx+x]=2;
			}
			else if (c=='.')
			{
			}
			else if ((c>='1') && (c<='3'))
			{
				_map[i*_sx+x]=3+(c-'1');
			}
			else
			{
				auto it=_elements.find(c);
				if (it!=_elements.end())
				{
					char buffer[512];
					sprintf_s((char*)&buffer,512,"Element '%c' already defined on level %i!",c,level);
					MessageBox(NULL,buffer,"Error",MB_OK|MB_ICONERROR);
				}
				else
				{
					Element elem;
					elem._start_pos.set(x*32.0f,i*32.0f);
					elem._sprite=NULL;
					elem._type=ELEMENT_UNKNOWN;

					_elements.insert(std::make_pair(c,elem));
				}
			}
		}
	}

	for (int i=0; i<level_node.nChildNode("element"); i++)
	{
		XMLNode element_node=level_node.getChildNode("element",i);
		char	c=element_node.getStringAttribute("id")[0];
		auto	it=_elements.find(c);
		if (it==_elements.end())
		{
			char buffer[512];
			sprintf_s((char*)&buffer,512,"Element '%c' position is not defined on level %i!",c,level);
			MessageBox(NULL,buffer,"Error",MB_OK|MB_ICONERROR);
		}
		else
		{
			it->second._start_time=element_node.getFloatAttribute("start_time");
			it->second._interval=element_node.getFloatAttribute("interval");
			it->second._speed=element_node.getFloatAttribute("speed")/1000.0f;
			it->second._random=element_node.getBoolAttribute("random");
			it->second._block_color=element_node.getIntAttribute("color");
			it->second._delta.set(0.0f,0.0f);

			std::string t=element_node.getStringAttribute("type");
			if (t=="block")
			{
				it->second._type=ELEMENT_BLOCK;
				it->second._sprite=new Sprite(it->second._start_pos.x,it->second._start_pos.y,0.6f);
				if (_potato) it->second._sprite->set_image(TEXTURES->find_by_name("potato"));
				else it->second._sprite->set_image(-1);
				it->second._sprite->set_size(Vec2f(32.0f,32.0f));

				Color c=get_block_color(it->second._block_color);
				it->second._sprite->set_color(c.r,c.g,c.b,c.a);
			}
			else if (t=="attractor")
			{
				it->second._strength=element_node.getFloatAttribute("strength");

				it->second._type=ELEMENT_ATTRACTOR;
				it->second._sprite=new Sprite(it->second._start_pos.x,it->second._start_pos.y,0.6f);

				if (it->second._strength<0.0f) it->second._sprite->set_image(TEXTURES->find_by_name("attractor_neg"));
				else it->second._sprite->set_image(TEXTURES->find_by_name("attractor_pos"));
				
				it->second._range=element_node.getFloatAttribute("range");

				Color c=get_cursor_color(it->second._block_color);
				it->second._sprite->set_color(c.r,c.g,c.b,c.a);
			}
			else if (t=="floor")
			{
				it->second._strength=element_node.getFloatAttribute("strength");

				it->second._type=ELEMENT_FLOOR;
				it->second._sprite=new Sprite(it->second._start_pos.x,it->second._start_pos.y,0.6f);
				it->second._sprite->set_image(TEXTURES->find_by_name("floor_field"));
				
				it->second._duration=element_node.getFloatAttribute("duration");

				Color c=get_cursor_color(it->second._block_color);
				it->second._sprite->set_color(c.r,c.g,c.b,c.a);
			}
			else
			{
				char buffer[512];
				sprintf_s((char*)&buffer,512,"Element '%c' type is unknown (%s) on level %i!",c,t.c_str(),level);
				MessageBox(NULL,buffer,"Error",MB_OK|MB_ICONERROR);
			}
		}
	}
	XMLNode spawn_node=level_node.getChildNode("spawn");
	if (spawn_node.isEmpty())
	{
		_spawn_color[0]=_spawn_color[1]=_spawn_color[2]=true;
	}
	else
	{
		_spawn_color[0]=_spawn_color[1]=_spawn_color[2]=false;

		for (int i=0; i<spawn_node.nChildNode("color"); i++)
		{
			XMLNode color_node=spawn_node.getChildNode("color",i);
			int		c=atol(color_node.getSafeText().c_str())-1;
			_spawn_color[c]=true;
		}
	}

	for (int i=0; i<level_node.nChildNode("tutorial"); i++)
	{
		XMLNode tutorial_node=level_node.getChildNode("tutorial",i);

		Tutorial t;
		t._pos.x=tutorial_node.getFloatAttribute("x");
		t._pos.y=tutorial_node.getFloatAttribute("y");
		t._pos.z=0.5f;
		t._text=string_find_replace(tutorial_node.getSafeText(),"\\n","\n");

		_tutorials.push_back(t);
	}

	_tutorial_index=0;

	_color_cycle=0.0f;
}

// Color: -1=no object, 0=black, 1=red, 2=green, 3=blue, 0xA0=exit
int GameScreen::get_color(const float x,const float y,const bool dynamics)
{
	int dx=(int)floorf(x/32.0f);
	int dy=(int)floorf(y/32.0f);

	// 0 if no collision
	// 1 if collision
	// 2 if end zone

	unsigned char ret=_map[dy*_sx+dx];
	
	if (ret==0)
	{
		if (dynamics)
		{
			auto it=_elements.begin();
			while (it!=_elements.end())
			{
				if ((it->second._type==ELEMENT_BLOCK))
				{
					Rect r;
					r.x1=it->second._sprite->_pos.x;
					r.y1=it->second._sprite->_pos.y;
					r.x2=r.x1+32.0f;
					r.y2=r.y1+32.0f;

					if (r.inside(Vec2f(x,y))) 
					{
						return it->second._block_color;
					}
				}

				++it;
			}
		}
	}
	else if (ret==1)
	{
		return 0;
	}
	else if (ret==2)
	{
		return 0xA0;
	}
	else if ((ret>=3) && (ret<=5))
	{
		// Colored block
		return ret-2;
	}
		
	return -1;
}

void GameScreen::close()
{
	if (_cursor) { delete _cursor; _cursor=NULL; }
	if (_map) { free(_map); _map=NULL; }
	if (_particles) { delete _particles; _particles=NULL; }
	if (_explosions) { delete _explosions; _explosions=NULL; }
	auto it=_elements.begin();
	while (it!=_elements.end())
	{
		if (it->second._sprite) delete it->second._sprite;
		++it;
	}
	_elements.clear();
	
	delete_font();
}

bool GameScreen::is_solid(const int tx,const int ty,const unsigned char block_color)
{
	unsigned char c=_map[tx+ty*_sx];
	if (c==1) return true;
	else if ((c>=3) && (c<=5))
	{
		if ((c-2)==block_color) return false;

		return true;
	}

	return false;
}

void GameScreen::new_explosion(const Vec3f& pos,const unsigned long color1,const unsigned long color2)
{
	_explosions->new_explosion(pos,color1,color2);
}

void GameScreen::new_dialog(const char* text,const float x,const float y,const int dialog_opt,const float trigger_time)
{
	_dialog_text=text;
	_dialog_pos.set(x,y,0.2f);
	_dialog_opt=dialog_opt;
	if (trigger_time>0.0f)
	{
		_dialog_trigger_time=trigger_time;
	}
	else 
	{
		_pause=true;
	}
}
