#include "LUAHelpers.h"
#include "../Utils.h"
#include "../Tokenizer.h"

size_t lua_get_argument_count(lua_State* L)
{
	return lua_gettop(L);
}

bool lua_isarray(lua_State* L,int idx)
{
	if (!lua_istable(L,idx))
	{
		return false;
	}

	lua_pushnil(L);  /* first key */
	if (lua_next(L,idx-1)==0) return false;

	std::string tmp=lua_get_string_argument(L,-3,"");
	lua_pop(L,2);

	if (is_a_number(tmp)) return true;

	return false;
}

const char*	lua_get_string_argument(lua_State* L,
									int num,
									const char* pdefault)
{
	return luaL_optstring(L, num+1, pdefault);
}

// Retrieves argument from stack (as number)
// Returns ddefault/0 if argument isn't a number / doesn't exist
float lua_get_number_argument(lua_State* L,
								int num,
								float ddefault)
{
	return luaL_optnumber(L, num+1, ddefault);
}

// Retrieves argument from stack (as number)
// Returns bdefault/false if argument isn't a boolean / doesn't exist
bool lua_get_boolean_argument(lua_State* L,
							  int num,
							  bool bdefault)
{
	if( !lua_isboolean(L,num+1) )
		return bdefault;
	return ( lua_toboolean(L,num+1) != 0 );
}

void* lua_get_pointer_argument(lua_State* L,
							   int num,
							   void* bdefault)
{
	if (!lua_isuserdata(L,num+1)) return bdefault;
	return lua_touserdata(L,num+1);
}

LUAFunctionType	lua_get_function_argument(lua_State* L,
											  int num,
											  LUAFunctionType func)
{
	if( !lua_isfunction(L,num+1) )
		return func;
	return lua_tocfunction(L,num+1);
}


const char*	lua_get_string_argument_ext(lua_State* L,
										int num,
										const char* field_name,
										const char* pdefault)
{
	lua_push_string(L,field_name);
	lua_gettable(L,num+1);
	if (!lua_isstring(L,-1)) return pdefault;
	const char* ptr=lua_tostring(L,-1);
	lua_pop(L,1);
	return ptr;
}

// Retrieves argument from stack (as number)
// Returns ddefault/0 if argument isn't a number / doesn't exist
float lua_get_number_argument_ext(lua_State* L,
									int num,
									const char* field_name,
									float ddefault)
{
	lua_push_string(L,field_name);
	lua_gettable(L,num+1);
	if (!lua_isnumber(L,-1)) return ddefault;
	float v=lua_tonumber(L,-1);
	lua_pop(L,1);
	return v;
}

// Retrieves argument from stack (as number)
// Returns bdefault/false if argument isn't a boolean / doesn't exist
bool lua_get_boolean_argument_ext(lua_State* L,
								  int num,
								  const char* field_name,
								  bool bdefault)
{
	lua_push_string(L,field_name);
	lua_gettable(L,num+1);
	if (!lua_isboolean(L,-1)) return bdefault;
	bool b=(lua_toboolean(L,-1)!=0)?true:false;
	lua_pop(L,1);
	return b;
}

void* lua_get_pointer_argument_ext(lua_State* L,
								   int num,
								   const char* field_name,
								   void* bdefault)
{
	lua_push_string(L,field_name);
	lua_gettable(L,num+1);
	if (!lua_isuserdata(L,-1)) return bdefault;
	void* ptr=lua_touserdata(L,-1);
	lua_pop(L,1);
	return ptr;
}

// Push string to stack
void lua_push_string(lua_State* L, const char* pstring)
{
	lua_pushstring(L, pstring);
}

// Push number to stack
void lua_push_number(lua_State* L, float value)
{
	lua_pushnumber(L, value);
}

// Push boolean to stack
void lua_push_boolean(lua_State* L, bool value)
{
	lua_pushboolean(L, static_cast<int>(value));
}

void lua_push_function(lua_State* L,LUAFunctionType func)
{
	lua_pushcfunction(L,func);
}

void lua_push_nil(lua_State* L)
{
	lua_pushnil(L);
}

void lua_push_table(lua_State* L)
{
	lua_newtable(L);
}

void lua_push_user(lua_State* L,void* ptr)
{
	lua_pushlightuserdata(L,ptr);
}

void lua_push_reference(lua_State* L,LUAReference* ptr)
{
	lua_pushlightuserdata(L,ptr);
}

void lua_push_string(lua_State* L, const char* field_name,const char* pstring)
{
	int idx=lua_gettop(L);

	lua_pushstring(L,field_name);
	lua_push_string(L,pstring);
	lua_settable(L,idx);
}

void lua_push_number(lua_State* L, const char* field_name,float value)
{
	int idx=lua_gettop(L);

	lua_pushstring(L,field_name);
	lua_push_number(L,value);
	lua_settable(L,idx);
}

void lua_push_boolean(lua_State* L, const char* field_name,bool value)
{
	int idx=lua_gettop(L);

	lua_pushstring(L,field_name);
	lua_push_boolean(L,value);
	lua_settable(L,idx);
}

void lua_push_nil(lua_State* L, const char* field_name)
{
	int idx=lua_gettop(L);

	lua_pushstring(L,field_name);
	lua_push_nil(L);
	lua_settable(L,idx);
}

void lua_push_function(lua_State* L, const char* field_name,LUAFunctionType func)
{
	int idx=lua_gettop(L);

	lua_pushstring(L,field_name);
	lua_push_function(L,func);
	lua_settable(L,idx);
}

Rect lua_get_rect(lua_State* L,const int num)
{
	int idx;
	if (num>=0) idx=num+1;
	else idx=num;

	if (!lua_istable(L,idx))
	{
		return Rect(0.0f,0.0f,0.0f,0.0f);
	}
	Rect rect;
	lua_rawgeti(L,idx,1);
	rect.x1=lua_get_number_argument(L,-2,0.0f);
	lua_pop(L,1);
	lua_rawgeti(L,idx,2);
	rect.y1=lua_get_number_argument(L,-2,0.0f);
	lua_pop(L,1);
	lua_rawgeti(L,idx,3);
	rect.x2=lua_get_number_argument(L,-2,0.0f);
	lua_pop(L,1);
	lua_rawgeti(L,idx,4);
	rect.y2=lua_get_number_argument(L,-2,0.0f);
	lua_pop(L,1);

	return rect;
}

Color lua_get_color(lua_State* L,const int num)
{
	int idx;
	if (num>=0) idx=num+1;
	else idx=num;

	if (!lua_istable(L,idx))
	{
		return Color(0.0f,0.0f,0.0f,0.0f);
	}
	Color c;
	lua_rawgeti(L,idx,1);
	c.r=lua_get_number_argument(L,-2,0.0f);
	lua_pop(L,1);
	lua_rawgeti(L,idx,2);
	c.g=lua_get_number_argument(L,-2,0.0f);
	lua_pop(L,1);
	lua_rawgeti(L,idx,3);
	c.b=lua_get_number_argument(L,-2,0.0f);
	lua_pop(L,1);
	lua_rawgeti(L,idx,4);
	c.a=lua_get_number_argument(L,-2,0.0f);
	lua_pop(L,1);

	return c;
}

Vec2f lua_get_vec2(lua_State* L,const int num)
{
	int idx;
	if (num>=0) idx=num+1;
	else idx=num;

	if (!lua_istable(L,idx))
	{
		return Vec2f(0.0f,0.0f);
	}
	Vec2f v;
	lua_rawgeti(L,idx,1);
	v.x=lua_get_number_argument(L,-2,0.0f);
	lua_pop(L,1);
	lua_rawgeti(L,idx,2);
	v.y=lua_get_number_argument(L,-2,0.0f);
	lua_pop(L,1);

	return v;
}

Vec3f lua_get_vec3(lua_State* L,const int num)
{
	int idx;
	if (num>=0) idx=num+1;
	else idx=num;

	if (!lua_istable(L,idx))
	{
		return Vec3f(0.0f,0.0f,0.0f);
	}
	Vec3f v;
	lua_rawgeti(L,idx,1);
	v.x=lua_get_number_argument(L,-2,0.0f);
	lua_pop(L,1);
	lua_rawgeti(L,idx,2);
	v.y=lua_get_number_argument(L,-2,0.0f);
	lua_pop(L,1);
	lua_rawgeti(L,idx,3);
	v.z=lua_get_number_argument(L,-2,0.0f);
	lua_pop(L,1);

	return v;
}

Vec4f lua_get_vec4(lua_State* L,const int num)
{
	int idx;
	if (num>=0) idx=num+1;
	else idx=num;

	if (!lua_istable(L,idx))
	{
		return Vec4f(0.0f,0.0f,0.0f,0.0f);
	}
	Vec4f v;
	lua_rawgeti(L,idx,1);
	v.x=lua_get_number_argument(L,-2,0.0f);
	lua_pop(L,1);
	lua_rawgeti(L,idx,2);
	v.y=lua_get_number_argument(L,-2,0.0f);
	lua_pop(L,1);
	lua_rawgeti(L,idx,3);
	v.z=lua_get_number_argument(L,-2,0.0f);
	lua_pop(L,1);
	lua_rawgeti(L,idx,4);
	v.w=lua_get_number_argument(L,-2,0.0f);
	lua_pop(L,1);

	return v;
}

void lua_push_rect(lua_State* L,const Rect& rect)
{
	lua_newtable(L);
	lua_push_number(L,rect.x1);
	lua_rawseti(L,-2,1);
	lua_push_number(L,rect.y1);
	lua_rawseti(L,-2,2);
	lua_push_number(L,rect.x2);
	lua_rawseti(L,-2,3);
	lua_push_number(L,rect.y2);
	lua_rawseti(L,-2,4);
}

void lua_push_vec2(lua_State* L,const Vec2f& v)
{
	lua_newtable(L);
	lua_push_number(L,v.x);
	lua_rawseti(L,-2,1);
	lua_push_number(L,v.y);
	lua_rawseti(L,-2,2);
}

void lua_push_vec3(lua_State* L,const Vec3f& v)
{
	lua_newtable(L);
	lua_push_number(L,v.x);
	lua_rawseti(L,-2,1);
	lua_push_number(L,v.y);
	lua_rawseti(L,-2,2);
	lua_push_number(L,v.z);
	lua_rawseti(L,-2,3);
}

void lua_push_vec4(lua_State* L,const Vec4f& v)
{
	lua_newtable(L);
	lua_push_number(L,v.x);
	lua_rawseti(L,-2,1);
	lua_push_number(L,v.y);
	lua_rawseti(L,-2,2);
	lua_push_number(L,v.z);
	lua_rawseti(L,-2,3);
	lua_push_number(L,v.w);
	lua_rawseti(L,-2,4);
}

LuaGlue mb_string_tokenize(lua_State* L)
{
	std::string string = lua_get_string_argument(L,0,"");
	std::string tokens = lua_get_string_argument(L,1,"");
	bool no_string = lua_get_boolean_argument(L,2,false);

	Tokenizer tokenizer;
	if(no_string) tokenizer.set_no_string(no_string);
	tokenizer.clear_delimiters();
	for(size_t i=0;i<tokens.size();i++)
		tokenizer.add_delimiter(tokens[i]);

	tokenizer.tokenize(string.c_str());

	LUAArray l;
	for(unsigned int i=0;i<tokenizer.get_token_number();i++)
	{
		l.push_string(tokenizer.get_token(i));
	}
	l.push(L);

	return 1;
}

void lua_bind_helper_functions(LUAEnv* env)
{
	env->add_function("string_tokenize",mb_string_tokenize);
}
