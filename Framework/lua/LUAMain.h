
#ifndef __LUA_H
#define __LUA_H

#include "LUAEnv.h"
#include "LUAArray.h"
#include "LUAHelpers.h"

#include <map>
#include <string>

#ifdef _DEBUG
#ifdef LUA_DOUBLE
#pragma comment(lib, "luad_d.lib")
#else
#pragma comment(lib, "lua_d.lib")
#endif
#else
#ifdef LUA_DOUBLE
#pragma comment(lib, "luad.lib")
#else
#pragma comment(lib, "lua.lib")
#endif
#endif
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h> 

struct lua_State;
#define LuaGlue int
 
typedef void (*LUA_LIBRARY_FUNCTION)(LUAEnv* env);

class LUAReference
{
public:
	LUAReference() { ; }
	virtual ~LUAReference() { ; }
};

class LUA
{
protected:
	typedef std::vector<LUAEnv*> LUAEnvs;
	typedef std::map< std::string , LUA_LIBRARY_FUNCTION > scLuaLibraries;
public:
	LUA(void);
public:
	virtual ~LUA(void);

	virtual	bool		init();
	virtual	void		shutdown();

	virtual LUAEnv*		get_environment_from_state(lua_State* L);
	virtual void		add_environment(LUAEnv* env) { _environments.push_back(env); }
	virtual void		remove_environment(LUAEnv* env);
	virtual	bool		add_script(LUAScript* script) { return _base_env->add_script(script); }
	virtual	bool		call_function(std::string function_name,LUAArray* in,LUAArray* out) { return _base_env->call_function(function_name,in,out); }
	virtual	LUAEnv*		get_base_env() { return _base_env; }

	// Libraries
			void						register_library(const std::string& lib_name,LUA_LIBRARY_FUNCTION func);
			LUA_LIBRARY_FUNCTION		get_library_function(const std::string& lib_name);

protected:
	LUAEnvs		_environments;
	LUAEnv*		_base_env;
	scLuaLibraries	_libraries;
};

void* sc_lua_alloc(void *ud, void *ptr, size_t osize, size_t nsize);

extern LUA* SCLUA;

#endif