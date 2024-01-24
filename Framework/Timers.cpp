// Timers.cpp: implementation of the Timers class.
//
//////////////////////////////////////////////////////////////////////

#include "Timers.h"
#include "Defs.h"

Timers* TIMERS=NULL;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Timers::Timers()
{
	if (!TIMERS) TIMERS=this;
}

Timers::~Timers()
{
	if (TIMERS==this) TIMERS=NULL;
	_timers.clear();
}

void Timers::poll()
{
	TimerList::iterator i;
	for (i=_timers.begin(); i!=_timers.end(); ++i)
	{
		if (timeGetTime()-(*i).base>(*i).elapse)
		{
			((*i).callback)((*i).param);
			i=_timers.erase(i);
		}
	}
}

void Timers::add_timer(float elapse,TimerCallback callback,void *param) 
{ 
	Timer timer; 
	
	timer.base=(float)(timeGetTime());
	timer.elapse=elapse; 
	timer.callback=callback; 
	timer.param=NULL; 
	_timers.push_back(timer); 
}
