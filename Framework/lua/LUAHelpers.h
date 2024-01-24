#ifndef __LUAHELPERS_H
#define __LUAHELPERS_H

#include "LUAMain.h"
#include "../MathLib.h"
#include "../Logger.h"

bool lua_isarray(lua_State* L,int idx);

size_t	lua_get_argument_count(lua_State* L);
// Get "normal" arguments
const char*	lua_get_string_argument(lua_State* L,
								int num,
								const char* pdefault = NULL);
float		lua_get_number_argument(lua_State* L,
								int num,
								float ddefault = 0.0f);
bool		lua_get_boolean_argument(lua_State* L,
								 int num,
								 bool bdefault = false);
void*		lua_get_pointer_argument(lua_State* L,
									 int num,
									 void* bdefault = NULL);
LUAFunctionType	lua_get_function_argument(lua_State* L,
											  int num,
											  LUAFunctionType func=NULL);
// Get arguments on a table
const char*	lua_get_string_argument_ext(lua_State* L,
										int num,
										const char* field_name,
										const char* pdefault = NULL);
float		lua_get_number_argument_ext(lua_State* L,
										int num,
										const char* field_name,
										float ddefault = 0.0f);
bool		lua_get_boolean_argument_ext(lua_State* L,
										 int num,
										 const char* field_name,
										 bool bdefault = false);
void*		lua_get_pointer_argument_ext(lua_State* L,
										 int num,
										 const char* field_name,
										 void* bdefault = NULL);

void		lua_push_string(lua_State* L, const char* pstring);
void		lua_push_number(lua_State* L, float value);
void		lua_push_boolean(lua_State* L, bool value);
void		lua_push_nil(lua_State* L);
void		lua_push_table(lua_State* L);
void		lua_push_function(lua_State* L,LUAFunctionType func);
void		lua_push_user(lua_State* L,void* ptr);
void		lua_push_reference(lua_State* L,LUAReference* ptr);
// Tabled versions
void		lua_push_string(lua_State* L, const char* field_name,const char* pstring);
void		lua_push_number(lua_State* L, const char* field_name,float value);
void		lua_push_boolean(lua_State* L, const char* field_name,bool value);
void		lua_push_nil(lua_State* L,const char* field_name);
void		lua_push_function(lua_State* L,const char* field_name,LUAFunctionType func);

int			lua_throw_error(lua_State* L,const char* fmt, ...);


Vec2f		lua_get_vec2(lua_State* L,const int num);
Vec3f		lua_get_vec3(lua_State* L,const int num);
Vec4f		lua_get_vec4(lua_State* L,const int num);
Rect		lua_get_rect(lua_State* L,const int num);
Color		lua_get_color(lua_State* L,const int num);

void		lua_push_rect(lua_State* L,const Rect& rect);
void		lua_push_vec2(lua_State* L,const Vec2f& v);
void		lua_push_vec3(lua_State* L,const Vec3f& v);
void		lua_push_vec4(lua_State* L,const Vec4f& v);

template<class T>
inline	T* lua_get(lua_State* L,const int num)
{
	LUAReference*	ptr=(LUAReference*)(lua_get_pointer_argument(L,num));
	T*				ret=dynamic_cast<T*>(ptr);
	if (!ret) 
		log("Error on type of parameter %li!",num);
	return ret;
}

#endif