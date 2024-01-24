// Timers.h: interface for the Timers class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TIMERS_H__A0DBA524_C378_43EC_8B48_F40B16B3E097__INCLUDED_)
#define AFX_TIMERS_H__A0DBA524_C378_43EC_8B48_F40B16B3E097__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <list>

typedef void (*TimerCallback)(void* param);

class Timers  
{
protected:
	struct Timer
	{
		float			base;
		float			elapse;
		TimerCallback	callback;
		void*			param;
	};
	typedef std::list<Timer>	TimerList;
public:
	Timers();
	virtual ~Timers();

			void	poll();
			void	add_timer(float elapse,TimerCallback callback,void *param=NULL);

protected:
	TimerList	_timers;
	float		_last_call;
};

extern Timers* TIMERS;

#endif // !defined(AFX_TIMERS_H__A0DBA524_C378_43EC_8B48_F40B16B3E097__INCLUDED_)
