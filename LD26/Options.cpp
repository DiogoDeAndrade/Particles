#include "Options.h"
#include "XML.h"
#include "Sound.h"

int					g_highscore=0;
std::map<int,bool>	g_gold_levels;
int					g_level=0;
int					g_retries=0;
bool				g_colorblind=false;
bool				g_tutorial=true;
bool				g_music=true;
bool				g_sound=true;

int					g_music_id=-1;

void load_options()
{
	FILE* file=NULL;
	fopen_s(&file,"options.xml","rt");
	if (!file) return;
	fclose(file);

	XMLNode root_node=XMLNode::openFileHelper("options.xml");
	if (root_node.isEmpty()) return;

	root_node=root_node.getChildNode("options");
	if (root_node.isEmpty()) return;

	XMLNode progress_node=root_node.getChildNode("progress");
	if (progress_node.isEmpty()) return;

	g_level=progress_node.getIntAttribute("current_level");
	g_highscore=progress_node.getIntAttribute("highscore");
	g_retries=progress_node.getIntAttribute("retries");

	g_gold_levels.clear();
	for (int i=0; i<progress_node.nChildNode("gold_level"); i++)
	{
		XMLNode gold_node=progress_node.getChildNode("gold_level",i);
		int     l=gold_node.getIntAttribute("level");
		g_gold_levels.insert(std::make_pair(l,true));
	}

	g_colorblind=root_node.getBoolAttribute("colorblind");
	g_tutorial=root_node.getBoolAttribute("tutorial");
	g_music=root_node.getBoolAttribute("music");
	g_sound=root_node.getBoolAttribute("sound");
}

void save_options()
{
	scXMLWriterNode* root_node=new scXMLWriterNode(NULL,"UTF-8");
	scXMLWriterNode* options_node=new scXMLWriterNode(root_node,"options");
	options_node->add_property("colorblind",g_colorblind);
	options_node->add_property("tutorial",g_tutorial);
	options_node->add_property("music",g_music);
	options_node->add_property("sound",g_sound);
	scXMLWriterNode* progress_node=new scXMLWriterNode(options_node,"progress");
	progress_node->add_property("current_level",(__int64)g_level);
	progress_node->add_property("highscore",(__int64)g_highscore);
	progress_node->add_property("retries",(__int64)g_retries);
	auto it=g_gold_levels.begin();
	while (it!=g_gold_levels.end())
	{
		scXMLWriterNode* gold_level=new scXMLWriterNode(progress_node,"gold_level");
		gold_level->add_property("level",(__int64)it->first);
		++it;
	}

	root_node->output("options.xml");
}

unsigned long g_particle_colors[4]=
{
	0xFF000000,
	0xFFFF0000,
	0xFF00CC00,
	0xFF00CCFF,
};

Color g_cursor_colors[6]=
{
	Color(0.0f,0.0f,0.0f,1.0f),
	Color(1.0f,0.0f,0.0f,1.0f),
	Color(0.0f,0.8f,0.0f,1.0f),
	Color(0.0f,0.8f,0.8f,1.0f),
	Color(1.0f,1.0f,1.0f,1.0f),
	Color(0.9f,0.9f,0.9f,0.9f),
};

Color g_block_colors[4]=
{
	Color(0.4f,0.4f,0.4f,1.0f),
	Color(0.3f,0.0f,0.0f,1.0f),
	Color(0.0f,0.3f,0.0f,1.0f),
	Color(0.0f,0.3f,0.3f,1.0f),
};

Color g_wall_colors[6]=
{
	Color(1.0f,1.0f,1.0f,1.0f),
	Color(0.0f,0.0f,0.0f,1.0f),
	Color(1.0f,1.0f,1.0f,1.0f),
	Color(0.5f,0.0f,0.0f,1.0f),
	Color(0.0f,0.5f,0.0f,1.0f),
	Color(0.0f,0.5f,0.5f,1.0f),
};

unsigned long g_particle_colorblind_colors[4]=
{
	0xFF000000,
	0xFFF7941D,
	0xFF00B9F2,
	0xFFDA6FAB,
};

Color g_cursor_colorblind_colors[6]=
{
	Color(0.0f,0.0f,0.0f,1.0f),
	Color(0.9f,0.6f,0.0f,1.0f),
	Color(0.35f,0.7f,0.9f,1.0f),
	Color(0.8f,0.6f,0.7f,1.0f),
	Color(1.0f,1.0f,1.0f,1.0f),
	Color(0.9f,0.9f,0.9f,0.9f),
};

Color g_block_colorblind_colors[4]=
{
	Color(0.4f,0.4f,0.4f,1.0f),
	Color(0.27f,0.18f,0.0f,1.0f),
	Color(0.105f,0.21f,0.27f,1.0f),
	Color(0.24f,0.18f,0.21f,1.0f),
};

Color g_wall_colorblind_colors[6]=
{
	Color(1.0f,1.0f,1.0f,1.0f),
	Color(0.0f,0.0f,0.0f,1.0f),
	Color(1.0f,1.0f,1.0f,1.0f),
	Color(0.45f,0.3f,0.0f,1.0f),
	Color(0.175f,0.35f,0.45f,1.0f),
	Color(0.4f,0.3f,0.35f,1.0f),
};

unsigned long get_particle_color(const int type)
{
	if (g_colorblind) return g_particle_colorblind_colors[type];
	
	return g_particle_colors[type];
}

Color get_cursor_color(const int type)
{
	if (g_colorblind) return g_cursor_colorblind_colors[type];

	return g_cursor_colors[type];	
}

Color get_block_color(const int type)
{
	if (g_colorblind) return g_block_colorblind_colors[type];

	return g_block_colors[type];	
}

Color get_wall_color(const int type)
{
	if (g_colorblind) return g_wall_colorblind_colors[type];

	return g_wall_colors[type];	
}

bool is_music_enabled()
{
	return g_music;
}

void set_music_enable(const bool b)
{
	if (g_music==b) 
	{
		if (g_music)
		{
			if (g_music_id!=-1) return;
		}
		else return;
	}
	g_music=b;
	if (g_music)
	{
		g_music_id=SOUND->play(SOUND->find_by_name("music"),0.1f);
	}
	else
	{
		if (g_music_id)
		{
			SOUND->stop(g_music_id);
			g_music_id=-1;
		}
	}
}

std::map<const char*,unsigned long>	g_sound_times;

void play_sound(const char* sound_name,const float vol,const unsigned long min_time)
{
	if (!g_sound) return;

	auto it=g_sound_times.find(sound_name);
	if (it==g_sound_times.end())
	{
		SOUND->play(SOUND->find_by_name(sound_name),vol);
		g_sound_times.insert(std::make_pair(sound_name,timeGetTime()));
	}
	else
	{
		if ((timeGetTime()-it->second)>min_time)
		{
			SOUND->play(SOUND->find_by_name(sound_name),vol);
			it->second=timeGetTime();
		}
	}
}
