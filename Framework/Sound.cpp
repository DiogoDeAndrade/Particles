// Sound.cpp: implementation of the Sound class.
//
//////////////////////////////////////////////////////////////////////

#include "Sound.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Sound* SOUND=NULL;

Sound::Sound()
{
	_enable=false;
	if (!SOUND) SOUND=this;
}

Sound::~Sound()
{
	shutdown();
	if (SOUND==this) SOUND=NULL;
}

bool Sound::init()
{
	if (FSOUND_Init(44100,32,FSOUND_INIT_USEDEFAULTMIDISYNTH)) 
	{
		_enable=true;
		return true;
	}

	return false;
}

void Sound::shutdown()
{
	clear_all();
	if (_enable) FSOUND_Close();
	_enable=false;
}

long Sound::find_by_name(const char *name)
{
	for (unsigned long i=0; i<_sounds.size(); i++)
	{
		if (_sounds[i]) 
		{
			if (!_stricmp(_sounds[i]->name,name))
			{
				return i;
			}
		}
	}
	return -1;
}

long Sound::load(const char *name,const char *filename,bool stream,bool loop)
{
	if (!_enable) return 0;

	Sample *sample=new Sample;
	sample->is_stream=stream;

	if (stream)
	{
		sample->stream=FSOUND_Stream_Open(filename,(loop)?(FSOUND_LOOP_NORMAL):(FSOUND_LOOP_OFF),0,0);
		if (!sample->stream) 
		{
			delete sample;		
			return -1;
		}
	}
	else
	{
		sample->sample=FSOUND_Sample_Load(FSOUND_FREE,filename,(loop)?(FSOUND_LOOP_NORMAL):(FSOUND_LOOP_OFF),0,0);
		if (!sample->sample) 
		{
			delete sample;
			return -1;
		}
	}
	sample->name=_strdup(name);
	
	_sounds.push_back(sample);

	return (long)(_sounds.size()-1);
}

int Sound::play(long id,float volume)
{
	if (!_enable) return -1;
	if (id==-1) return -1;

	Sample* sample=_sounds[id];
	if (!sample) return -1;

	int ret=-1;

	if (sample->is_stream)
	{
		if (sample->stream) ret=FSOUND_Stream_Play(FSOUND_FREE,sample->stream);
	}
	else
	{
		if (sample->sample)
		{
			ret=FSOUND_PlaySound(FSOUND_FREE,sample->sample);
		}
	}
	FSOUND_SetVolume(ret,(int)(volume*255));

	return ret;
}

void Sound::stop(long chan)
{
	FSOUND_StopSound(chan);
}

void Sound::stop_all()
{
	if (!_enable) return;

	FSOUND_StopSound(FSOUND_ALL);
}

void Sound::clear_all()
{
	stop_all();

	for (unsigned long i=0; i<_sounds.size(); i++)
	{
		if (_sounds[i]) 
		{
			if (_sounds[i]->is_stream)
			{
				FSOUND_Stream_Close(_sounds[i]->stream);
			}
			else
			{
				FSOUND_Sample_Free(_sounds[i]->sample);
			}
			free(_sounds[i]->name);
			delete _sounds[i];
			_sounds[i]=NULL;
		}
	}
	_sounds.clear();
}

