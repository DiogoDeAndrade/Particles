#ifndef __LUAENV_H
#define __LUAENV_H

#include <list>
#include <vector>

struct	lua_State;
class	LUAScript;
class	LUAArray;

typedef int (*LUAFunctionType)(struct lua_State* pLuaState);

struct LUAClassDefElem
{
	LUAClassDefElem(const std::string& name,LUAFunctionType func) { _name=name; _func=func; }
	std::string			_name;
	LUAFunctionType	_func;
};
typedef std::vector<LUAClassDefElem> LUAClassDef;

typedef enum { SCLUA_NIL=0, SCLUA_BOOL=1, SCLUA_STRING=2, SCLUA_NUMBER=3, SCLUA_ARRAY=4, SCLUA_FUNCTION=5, SCLUA_REFERENCE=6, SCLUA_TABLE=7, SCLUA_UNKNOWN=0xFF } scLuaType;

class LUAEnv
{
public:
	LUAEnv();
	LUAEnv(lua_State*	env,LUAEnv* parent);
public:
	virtual ~LUAEnv(void);

	virtual	bool		add_script(LUAScript* script);
	virtual	bool		add_script(const std::string& script_file);
	virtual	bool		load_file(const std::string& filename);
	virtual	bool		add_function(const char* pfunction_name,
									 LUAFunctionType pfunction);
	virtual	bool		add_function(const char* scope,
									 const char* pfunction_name,
									 LUAFunctionType pfunction);
	virtual	void		add_class(const std::string& nspace,const std::string& cname,
								  LUAClassDef* def,LUAClassDef* namespace_funcs);
	virtual	bool		call_function(const std::string& function_name,LUAArray* in,LUAArray* out);
	virtual	bool		exists_function(const std::string& function_name);

	virtual	lua_State*	get_env() { return _env; }
	virtual	void		dump_stack();
	virtual	void		dump_stack_to_string(std::string& tmp);

			std::string	get_var(const std::string& name);
			std::string	get_var_string(const std::string& name);
			float		get_var_number(const std::string& name);
			void*		get_var_pointer(const std::string& name);
			bool		get_var_bool(const std::string& name);
			
			void		set_var(const std::string& name,const std::string& val);
			void		set_var(const std::string& name,const float val);
			void		set_var(const std::string& name,void* val);
			void		set_var(const std::string& name,const bool val);

			// Co-rotines
	virtual	LUAEnv*	build_thread();
	virtual	int			resume();
	virtual	int			resume(const std::string& function_name,LUAArray* in);

	inline	void		set_line(const size_t line_no) { _line_no=line_no; }
	inline	size_t		get_line() { return _line_no; }

protected:
	virtual	bool		call(int args, int ret);
	virtual	void		push_string(const char* pstring);
	virtual	const char*	get_string_argument(int num,const char* pdefault=NULL);
	virtual	long		get_long_argument(int num,long pdefault=0);
	virtual	float		get_float_argument(int num,float pdefault=0.0f);
	virtual	bool		get_bool_argument(int num,bool pdefault=false);
			void		get_last_error();
			bool		run_command(const std::string& cmd,const std::string& filename);

protected:
	lua_State*	_env;
	LUAEnv*	_parent;
	int			_reference;
	std::string	_last_error;
	size_t		_line_no;
};

typedef std::list<LUAEnv*>	LUAEnvList;

size_t get_line_no(lua_State* L);

extern LUAEnv*	g_current_lua_env;

#endif