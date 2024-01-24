#include "LUAMain.h"

// Lua alloc function
 void* sc_lua_alloc(void *ud, void *ptr, size_t osize, size_t nsize) 
{
	(void)ud;     /* not used */
	(void)osize;  /* not used */

	if (nsize==0) 
	{	
		// new size == 0? free memory
		free(ptr);  /* ANSI requires that free(NULL) has no effect */
		return NULL;
	}
	else
	{
		// (re)alloc memory
		/* ANSI requires that realloc(NULL, size) == malloc(size) */
		if (ptr) return realloc(ptr,nsize);
		else return malloc(nsize);
	}
}

LUA* SCLUA=NULL;

LUA::LUA(void)
{
	if (!SCLUA) SCLUA=this;
	_base_env=NULL;
}

LUA::~LUA(void)
{
	if (SCLUA==this) SCLUA=NULL;
}

bool LUA::init()
{
	shutdown();

	_base_env=new LUAEnv;
	
	return true;
}

void LUA::shutdown()
{
	if (_base_env)
	{
		delete(_base_env); _base_env=NULL;
	}
}

LUAEnv*	LUA::get_environment_from_state(lua_State* L)
{
	LUAEnvs::iterator it;
	for(it = _environments.begin(); it != _environments.end(); it++)
	{
		if((*it)->get_env() == L)
			return (*it);
	}

	return NULL;
}

void		LUA::remove_environment(LUAEnv* env)
{
	LUAEnvs::iterator it;
	for(it = _environments.begin(); it != _environments.end(); it++)
	{
		if((*it) == env)
		{
			_environments.erase(it);
			break;
		}
	}
}

void LUA::register_library(const std::string& lib_name,LUA_LIBRARY_FUNCTION func)
{
	_libraries.insert(std::make_pair(lib_name,func));
}

LUA_LIBRARY_FUNCTION LUA::get_library_function(const std::string& lib_name)
{
	if (_libraries.count(lib_name)>0)
	{
		return _libraries[lib_name];
	}
	return NULL;
}
