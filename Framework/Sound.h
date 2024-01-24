// Sound.h: interface for the Sound class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SOUND_H__E4F8DC2B_D19C_4F8D_AE16_94E81A81BB5F__INCLUDED_)
#define AFX_SOUND_H__E4F8DC2B_D19C_4F8D_AE16_94E81A81BB5F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#pragma comment(lib, "fmodvc.lib")
#include "fmod.h"
#include <vector>

class Sound
{
protected:
	struct Sample
	{
		char			*name;
		bool			is_stream;
		FSOUND_SAMPLE	*sample;
		FSOUND_STREAM	*stream;
	};
	typedef std::vector<Sample*> Samples;
public:
	Sound();
	virtual ~Sound();

			bool	init();
			void	shutdown();
			long	find_by_name(const char *name);
			long	load(const char *name,const char *filaname,bool stream,bool loop);
			int		play(long id,float volume);
			void	stop(long chan);
			void	stop_all();
			void	clear_all();
			inline	size_t	get_sound_count() { return _sounds.size(); }

protected:
	bool	_enable;
	Samples	_sounds;
};

extern Sound* SOUND;

#endif // !defined(AFX_SOUND_H__E4F8DC2B_D19C_4F8D_AE16_94E81A81BB5F__INCLUDED_)
