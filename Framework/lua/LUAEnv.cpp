#include "LUAEnv.h"
#include "LUA.h"
#include "LUAHelpers.h"
#include "LUAScript.h"
#include "../Logger.h"

typedef std::list<LUAEnv*> LUAEnvs;
LUAEnvs	g_lua_envs;
LUAEnv*	g_current_lua_env;

void add_lua_env(LUAEnv* env)
{
	g_lua_envs.push_back(env);
}

void remove_lua_env(LUAEnv* env)
{
	LUAEnvs::iterator it=g_lua_envs.begin();
	while (it!=g_lua_envs.end())
	{
		if ((*it)==env) it=g_lua_envs.erase(it);
		else ++it;
	}
}

LUAEnv* get_lua_env(lua_State* L)
{
	if (L==NULL) return g_current_lua_env;

	LUAEnvs::iterator it=g_lua_envs.begin();
	while (it!=g_lua_envs.end())
	{
		if ((*it)->get_env()==L) return *it;
		else ++it;
	}
	return NULL;
}

void lua_hook_line(lua_State* L,lua_Debug* ar)
{
	LUAEnv* env=get_lua_env(L);
	if (!env) return;

	env->set_line(ar->currentline);
}

size_t get_line_no(lua_State* L)
{
	LUAEnv* env=get_lua_env(L);
	if (!env) return 0;

	return env->get_line();
}

class LUAEnvAuxCurrent
{
public:
	LUAEnvAuxCurrent(LUAEnv* env) { _last_env=g_current_lua_env; g_current_lua_env=env; }
	virtual ~LUAEnvAuxCurrent() { g_current_lua_env=_last_env; }
protected:
	LUAEnv* _last_env;
};

LuaGlue lua_include(lua_State* L)
{
	std::string str=lua_get_string_argument(L,0,"");
	LUAScript* scr=new LUAScript;
	bool err=scr->load_from_file(str);
	if (!err)
	{
		log("Can't include script \"%s\"!",str.c_str());
		lua_push_boolean(L,false);
		return 1;
	}
	if (g_current_lua_env) 
	{
		lua_push_boolean(L,true);
		g_current_lua_env->add_script(scr);
	}
	else
	{
		log("Can't find current LUA environment!");
		lua_push_boolean(L,false);
	}
	delete scr;

	return 1;
}

LuaGlue lua_uses(lua_State* L)
{
	std::string str=lua_get_string_argument(L,0,"");
	
	LUA_LIBRARY_FUNCTION func=SCLUA->get_library_function(str);
	if (!func)
	{
		log("Library \"%s\" is not registered!",str.c_str());
		lua_push_boolean(L,false);
		return 1;
	}
	if (g_current_lua_env) 
	{
		func(g_current_lua_env);
		lua_push_boolean(L,true);
	}
	else
	{
		log("Can't find current LUA environment!");
		lua_push_boolean(L,false);
	}

	return 1;
}

LuaGlue lua_log(lua_State* L)
{
	std::string	str=lua_get_string_argument(L,0,"");

	log(str.c_str());

	return 0;
}

LUAEnv::LUAEnv()
{
	_last_error="";
	_env=lua_newstate(sc_lua_alloc,NULL);
	luaL_openlibs(_env);
	_parent=NULL;
	_reference=-1;
	if(SCLUA) SCLUA->add_environment(this);
	add_lua_env(this);
	lua_sethook(_env,lua_hook_line,LUA_MASKLINE,0);

	add_function("std","include",lua_include);
	add_function("std","uses",lua_uses);
	add_function("std","log",lua_log);
}

LUAEnv::LUAEnv(lua_State* env,LUAEnv* parent)
{
	_last_error="";
	_env=env;
	_parent=parent;
	if (parent) _reference=luaL_ref(parent->get_env(),LUA_REGISTRYINDEX);
	if(SCLUA) SCLUA->add_environment(this);
	add_lua_env(this);
	lua_sethook(_env,lua_hook_line,LUA_MASKLINE,0);
}

LUAEnv::~LUAEnv(void)
{
	if (_env)
	{
		if(SCLUA) SCLUA->remove_environment(this);
		if (_parent)
		{
			luaL_unref(_parent->get_env(),LUA_REGISTRYINDEX,_reference);
		}
		else
		{
			lua_close(_env); 
			_env=NULL;
		}
	}
	remove_lua_env(this);
}

bool LUAEnv::add_script(LUAScript* script)
{
	LUAEnvAuxCurrent current_lua_env_aux(this);

	const char* s=script->get_script_ptr();
	bool error=run_command(s,script->get_script_id());
	if (!error)
	{
		log("Can't call LUA script (%s)!",script->get_script_id().c_str());
		return false;
	}
	_last_error=error;
	return true;
}

bool LUAEnv::add_script(const std::string& script_file)
{
	LUAScript scr;
	bool err=scr.load_from_file(script_file);
	if (!err) return err;
	
	return add_script(&scr);
}

void LUAEnv::add_class(const std::string& nspace,const std::string& cname,
						LUAClassDef* def,LUAClassDef* namespace_funcs)
{
	std::string	metatable_name=nspace;
	metatable_name+="."; metatable_name+=cname;
	luaL_Reg*	methods=new luaL_Reg[def->size()+1];
	for (size_t i=0; i<def->size(); i++)
	{
		methods[i].name=def->at(i)._name.c_str();
		methods[i].func=def->at(i)._func;
	}
	methods[def->size()].name=NULL;
	methods[def->size()].func=NULL;

	luaL_newmetatable(_env,metatable_name.c_str());
	lua_pushvalue(_env,-1);
	lua_setfield(_env,-2,"__index");
	luaL_register(_env,NULL,methods);
	
	luaL_Reg*	namespace_methods=new luaL_Reg[namespace_funcs->size()+1];
	for (size_t i=0; i<namespace_funcs->size(); i++)
	{
		namespace_methods[i].name=namespace_funcs->at(i)._name.c_str();
		namespace_methods[i].func=namespace_funcs->at(i)._func;
	}
	namespace_methods[namespace_funcs->size()].name=NULL;
	namespace_methods[namespace_funcs->size()].func=NULL;
	luaL_register(_env,cname.c_str(),namespace_methods);

	delete[] methods;
	delete[] namespace_methods;
}

bool LUAEnv::call(int args,int ret)
{
	LUAEnvAuxCurrent current_lua_env_aux(this);

	int lua_err=lua_pcall(_env,args,ret,0);
	if (lua_err!=0)
	{
		get_last_error();
		std::string tmp="Can't call LUA function!\n";
		dump_stack_to_string(_last_error);
		tmp+=_last_error;
		log("%s",tmp.c_str());
		return false;
	}
	_last_error="ok";
	return true;
}

bool LUAEnv::call_function(const std::string& function_name,LUAArray* in,LUAArray* out)
{
	LUAEnvAuxCurrent current_lua_env_aux(this);

	bool exist=exists_function(function_name);
	if (!exist)
	{
		_last_error="error: function does not exist!";
		return false;
	}

	_last_error="ok";

	lua_getglobal(_env,function_name.c_str());

	int size=0;
	if (in)
	{
		in->push(_env,false);
		size=int(in->get_size());
	} 

	bool call_result=call(size,LUA_MULTRET);
	_last_error=call_result;
	if (!call_result)
	{
		log("Function call to %s failed (%s)",function_name.c_str(),_last_error.c_str());
		if (in)
		{
			log("Input params:");
			in->dump();
		}
		if (out)
		{
			log("Output params:");
			out->dump();
		}
	}

	if (!out) return call_result;

	out->clear();

	int num_ret=lua_gettop(_env);

	for (int i=1; i<num_ret+1; i++)
	{
		int t=lua_type(_env,i);
		switch (t)
		{
			case LUA_TSTRING:
				{
					const char* ret_i=get_string_argument(i);
					if (!ret_i)	return false;
					std::string str=ret_i;
					out->push_string(str);
				}
				break;
			case LUA_TBOOLEAN:
				{
					bool ret_i=get_bool_argument(i,false);
					out->push_bool(ret_i);
				}
				break;
			case LUA_TNUMBER:
				{
					float ret_f=get_float_argument(i,0);
					out->push_float(ret_f);
				}
				break;
			case LUA_TTABLE:
				{
					LUAArray* a=new LUAArray;
					a->load(_env,i-1);
					out->push_array(a);
				}
				break;
			default:
				break;
		}
	}
	lua_pop(_env,num_ret);

	return call_result;
}

void LUAEnv::push_string(const char* pstring)
{
	lua_pushstring(_env,pstring);
}

const char*	LUAEnv::get_string_argument(int num,const char* pdefault)
{
	return luaL_optstring(_env,num,pdefault);
}

long LUAEnv::get_long_argument(int num,long pdefault)
{
	return luaL_optlong(_env,num,pdefault);
}

bool LUAEnv::get_bool_argument(int num,bool pdefault)
{
	return (lua_toboolean(_env,num))?(true):(false);
}

float LUAEnv::get_float_argument(int num,float pdefault)
{
	return lua_tonumber(_env,num);
}

bool LUAEnv::add_function(const char* pfunction_name,
							   LUAFunctionType pfunction)
{
	lua_register(_env, pfunction_name, pfunction);	// register function
	return true;
}

bool	LUAEnv::add_function(const char* scope,
							   const char* pfunction_name,
							   LUAFunctionType pfunction)
{
	// Scope already defined?
	bool scope_defined=false;
	lua_getglobal(_env,scope);
	if (!lua_isnil(_env,-1)) 
		scope_defined=true;
	else lua_pop(_env,-1);

	if (!scope_defined)
	{
		lua_newtable(_env);
	}

	lua_pushstring(_env,pfunction_name);
	lua_pushcfunction(_env,pfunction);
	lua_settable(_env,-3);

	if (!scope_defined)
		lua_setglobal(_env,scope);
	else
		lua_pop(_env,-1);

	return true;
}

LUAEnv* LUAEnv::build_thread()
{
	lua_State* state=lua_newthread(_env);

	LUAEnv* env=new LUAEnv(state,this);

	return env;
}

int	LUAEnv::resume()
{
	LUAEnvAuxCurrent current_lua_env_aux(this);

	int tmp=lua_resume(_env,lua_gettop(_env));			// yield function
	if ((tmp!=0) && (tmp!=LUA_YIELD))
	{
		log("Thread can't be resumed at line %li!",get_line_no(_env));
		dump_stack();
	}

	return tmp;
}

int	LUAEnv::resume(const std::string& function_name,LUAArray* in)
{
	LUAEnvAuxCurrent current_lua_env_aux(this);

	lua_getglobal(_env,function_name.c_str());


	int size=0;
	if (in)
	{
		size= static_cast<int>(in->get_size());
		in->push(_env,false);
	}

	int tmp=lua_resume(_env,size);			// yield function
	if ((tmp!=0) && (tmp!=LUA_YIELD))
	{
		dump_stack();
	}

	return tmp;
}

void LUAEnv::dump_stack()
{
	int i;
	int top=lua_gettop(_env);
	log("LUA Stack:");
	for (i=top; i>0; i--)
	{
		int t=lua_type(_env,i);
		switch (t)
		{
		case LUA_TSTRING:
			log("String: \"%s\"",lua_tostring(_env,i));
			break;
		case LUA_TBOOLEAN:
			log("Boolean: %s",(lua_toboolean(_env,i))?("true"):("false"));
			break;
		case LUA_TNUMBER:
			log("Number: %f",lua_tonumber(_env,i));
			break;
		default:
			log("Other: %s",lua_typename(_env,t));
			break;
		}
	}
}

void LUAEnv::dump_stack_to_string(std::string& str)
{
	int i;
	int top=lua_gettop(_env);
	str+="\nLUA Stack:\n";
	for (i=top; i>0; i--)
	{
		char buffer[512];
		int t=lua_type(_env,i);
		switch (t)
		{
			case LUA_TSTRING:
				sprintf_s((char*)&buffer,512,"String: \"%s\"\n",lua_tostring(_env,i));
				break;
			case LUA_TBOOLEAN:
				sprintf_s((char*)&buffer,512,"Boolean: %s\n",(lua_toboolean(_env,i))?("true"):("false"));
				break;
			case LUA_TNUMBER:
				sprintf_s((char*)&buffer,512,"Number: %f\n",lua_tonumber(_env,i));
				break;
			default:
				sprintf_s((char*)&buffer,512,"Other: %s\n",lua_typename(_env,t));
				break;
		}
		str+=buffer;
	}
}

bool LUAEnv::exists_function(const std::string& function_name)
{
	int n_elems=lua_gettop(_env);

	lua_pushnil(_env);
	lua_getglobal(_env,function_name.c_str());

	n_elems=lua_gettop(_env)-n_elems;

	// Check if it is a function
	if (lua_isnil(_env,lua_gettop(_env)))
	{
		lua_pop(_env,n_elems);
		return false;
	}
	if (lua_isfunction(_env,lua_gettop(_env)))
	{
		lua_pop(_env,n_elems);
		return true;
	}
	lua_pop(_env,n_elems);
	return false;
}

void LUAEnv::get_last_error()
{
	int top=lua_gettop(_env);
	if (top>0)
	{
//		dump_stack();
		const char* tmp=lua_tostring(_env,top);
		if (tmp)
		{
			lua_pop(_env,top);
			_last_error=tmp;
		}
		else _last_error="unknown";
	}
	else _last_error="unknown";
}

bool	LUAEnv::run_command(const std::string& cmd,const std::string& filename)
{
	LUAEnvAuxCurrent current_lua_env_aux(this);

	int			ret=luaL_loadbuffer(_env,cmd.c_str(),cmd.length(),filename.c_str());
	if (ret!=0)
	{
		get_last_error();
		if (ret==LUA_ERRSYNTAX) 
		{ 
			log("Can't run LUA script - Syntax error!\nError: %s",_last_error.c_str()); 

			return false;
		}
		else if (ret==LUA_ERRMEM) 
		{ 
			log("Can't run LUA script - Memory allocation error (%s)!",_last_error.c_str()); 

			return false;
		}

		log("Can't run LUA script - Unknown error (%s)!",_last_error.c_str());
		return false;
	}

	bool error=call(0,LUA_MULTRET);			// call function
	return error;
}

bool	LUAEnv::load_file(const std::string& filename)
{
	LUAScript	scr;
	bool err=scr.load_from_file(filename);
	if (!err) return false;
	err=add_script(&scr);
	if (!err) return false;
	return true;
}

std::string LUAEnv::get_var(const std::string& name)
{
	lua_getglobal(_env,name.c_str());
	return lua_tostring(_env,-1);
}

std::string LUAEnv::get_var_string(const std::string& name)
{
	lua_getglobal(_env,name.c_str());
	return lua_tostring(_env,-1);
}

float LUAEnv::get_var_number(const std::string& name)
{
	lua_getglobal(_env,name.c_str());
	return lua_tonumber(_env,-1);
}

void* LUAEnv::get_var_pointer(const std::string& name)
{
	lua_getglobal(_env,name.c_str());
	return (void*)lua_topointer(_env,-1);
}

bool LUAEnv::get_var_bool(const std::string& name)
{
	lua_getglobal(_env,name.c_str());
	return (lua_toboolean(_env,-1))?(true):(false);
}

void LUAEnv::set_var(const std::string& name,const std::string& val)
{
	lua_pushstring(_env,val.c_str());
	lua_setglobal(_env,name.c_str());
}

void LUAEnv::set_var(const std::string& name,const float val)
{
	lua_pushnumber(_env,val);
	lua_setglobal(_env,name.c_str());
}

void LUAEnv::set_var(const std::string& name,void* val)
{
	lua_pushlightuserdata(_env,val);
	lua_setglobal(_env,name.c_str());
}

void LUAEnv::set_var(const std::string& name,const bool val)
{
	lua_pushboolean(_env,(val)?(1):(0));
	lua_setglobal(_env,name.c_str());
}
