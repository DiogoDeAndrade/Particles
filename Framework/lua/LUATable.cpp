#include "LUATable.h"
#include "LUAHelpers.h"
#include "../Logger.h"

LUATable::LUATable(void)
{
}

LUATable::~LUATable(void)
{
	clear();
}

void LUATable::clear()
{
	scArrayElems::iterator it=_elems.begin();
	while (it!=_elems.end())
	{
		if ((*it).second.type==SCLUA_ARRAY) delete (*it).second.a;
		else if ((*it).second.type==SCLUA_TABLE) delete (*it).second.t;
		++it;
	}
	_elems.clear();
	_keys.clear();
}

bool LUATable::load(lua_State* env,const int index)
{
	clear();

	if (index>=0)
	{
		int idx;
		idx=index+1;

		if (!lua_istable(env,idx))
		{
			return false;
		}

		lua_pushnil(env);  /* first key */
		while (lua_next(env,idx)!=0) 
		{
			/* uses 'key' (at index -2) and 'value' (at index -1) */ 
			//add your own code to get the key and value
			std::string key=lua_tostring(env,-2);
			scArrayElem elem;
			// Get element
			int lt=lua_type(env,-1);

			switch (lt)
			{
				case LUA_TNUMBER:
					elem.type=SCLUA_NUMBER;
					elem.v=lua_get_number_argument(env,-2,0.0f);
					break;
				case LUA_TSTRING:
					elem.type=SCLUA_STRING;
					elem.str=lua_get_string_argument(env,-2,NULL);
					break;
				case LUA_TBOOLEAN:
					elem.type=SCLUA_BOOL;
					elem.b=lua_get_boolean_argument(env,-2,0.0f);
					break;
				case LUA_TUSERDATA:
					elem.type=SCLUA_REFERENCE;
					elem.r=(LUAReference*)lua_touserdata(env,-2);
					break;
				case LUA_TNIL:
					elem.type=SCLUA_NIL;
					break;
				case LUA_TFUNCTION:
					elem.type=SCLUA_FUNCTION;
					elem.func=lua_get_function_argument(env,-2,NULL);
					break;
				case LUA_TTABLE:
					if (lua_isarray(env,-1))
					{
						elem.type=SCLUA_ARRAY;
						elem.a=new LUAArray;
						elem.a->load(env,-1);
					}
					else
					{
						elem.type=SCLUA_TABLE;
						elem.t=new LUATable;
						elem.t->load(env,-1);
					}
					break;
				default:
#ifdef __WIN32
					__asm int 3
#endif
					break;
			}
			_elems.insert(std::make_pair(key,elem));		
			_keys.push_back(key);

			/* removes 'value'; keeps 'key' for next iteration */
			lua_pop(env, 1);
		}
	}
	else
	{
		int idx;
		idx=index;

		if (!lua_istable(env,idx))
		{
			return false;
		}

		lua_pushnil(env);  /* first key */
		while (lua_next(env,idx-1)!=0) 
		{
			/* uses 'key' (at index -2) and 'value' (at index -1) */ 
			//add your own code to get the key and value
			std::string key=lua_tostring(env,-2);
			scArrayElem elem;
			// Get element
			int lt=lua_type(env,-1);

			switch (lt)
			{
			case LUA_TNUMBER:
				elem.type=SCLUA_NUMBER;
				elem.v=lua_get_number_argument(env,-2,0.0f);
				break;
			case LUA_TSTRING:
				elem.type=SCLUA_STRING;
				elem.str=lua_get_string_argument(env,-2,NULL);
				break;
			case LUA_TBOOLEAN:
				elem.type=SCLUA_BOOL;
				elem.b=lua_get_boolean_argument(env,-2,0.0f);
				break;
			case LUA_TUSERDATA:
				elem.type=SCLUA_REFERENCE;
				elem.r=(LUAReference*)lua_touserdata(env,-2);
				break;
			case LUA_TNIL:
				elem.type=SCLUA_NIL;
				break;
			case LUA_TFUNCTION:
				elem.type=SCLUA_FUNCTION;
				elem.func=lua_get_function_argument(env,-2,NULL);
				break;
			case LUA_TTABLE:
				if (lua_isarray(env,-1))
				{
					elem.type=SCLUA_ARRAY;
					elem.a=new LUAArray;
					elem.a->load(env,-1);
				}
				else
				{
					elem.type=SCLUA_TABLE;
					elem.t=new LUATable;
					elem.t->load(env,-1);
				}
				break;
			default:
#ifdef __WIN32
					__asm int 3
#endif
					
					break;
			}
			_elems.insert(std::make_pair(key,elem));		
			_keys.push_back(key);

			/* removes 'value'; keeps 'key' for next iteration */
			lua_pop(env, 1);
		}
	}

	return true;
}

bool LUATable::push(lua_State* env)
{
	int idx;

	lua_push_table(env);

	scArrayElems::iterator it=_elems.begin();
	while (it!=_elems.end())
	{
		switch ((*it).second.type)
		{
			case SCLUA_NIL: lua_push_nil(env,(*it).first.c_str()); break;
			case SCLUA_STRING: lua_push_string(env,(*it).first.c_str(),(*it).second.str.c_str()); break;
			case SCLUA_NUMBER: lua_push_number(env,(*it).first.c_str(),(*it).second.v); break;
			case SCLUA_BOOL: lua_push_number(env,(*it).first.c_str(),(*it).second.b); break;
			case SCLUA_FUNCTION: lua_push_function(env,(*it).first.c_str(),(*it).second.func); break;
			case SCLUA_TABLE: 
				idx=lua_gettop(env);
				lua_pushstring(env,(*it).first.c_str());
				(*it).second.t->push(env); 
				lua_settable(env,idx);
				break;
			case SCLUA_ARRAY: 
				idx=lua_gettop(env);
				lua_pushstring(env,(*it).first.c_str());
				(*it).second.a->push(env,true); 
				lua_settable(env,idx);
				break;
			default:
#ifdef __WIN32
				__asm int 3
#endif
				
				break;
		}
		++it;
	}
	return true;
}

void LUATable::dump()
{
	scArrayElems::iterator it=_elems.begin();
	while (it!=_elems.end())
	{
		dump((*it).first,&((*it).second));
		++it;
	}
}

void LUATable::dump(const std::string& key_name,scArrayElem* elem)
{
	switch (elem->type)
	{
		case SCLUA_NIL: log("%s=nil",key_name.c_str()); break;
		case SCLUA_BOOL: log("%s=%s",key_name.c_str(),(elem->b)?("true"):("false")); break;
		case SCLUA_STRING: log("%s=\"%s\"",key_name.c_str(),elem->str.c_str()); break;
		case SCLUA_NUMBER: log("%s=%f",key_name.c_str(),elem->v); break;
		case SCLUA_FUNCTION: log("%s=function",key_name.c_str()); break;
		case SCLUA_REFERENCE: log("%s=0x%x",key_name.c_str(),elem->r); break;
		case SCLUA_ARRAY: log("%s=array:",key_name.c_str()); elem->a->dump(); break;
		case SCLUA_TABLE: log("%s=table",key_name.c_str()); elem->t->dump(); break;
	}
}

size_t LUATable::get_size() 
{ 
	return _keys.size(); 
}

std::string LUATable::get_key(const size_t index)
{
	return _keys[index];
}

scLuaType LUATable::get_type(const size_t index) 
{ 
	return _elems[_keys[index]].type; 
}

float LUATable::get_float(const size_t index) 
{ 
	return _elems[_keys[index]].v; 
}

std::string	LUATable::get_string(const size_t index) 
{ 
	return _elems[_keys[index]].str; 
}

bool LUATable::get_bool(const size_t index) 
{ 
	return _elems[_keys[index]].b; 
}

LUAReference* LUATable::get_reference(const size_t index) 
{ 
	return _elems[_keys[index]].r; 
}

LUAArray* LUATable::get_array(const size_t index) 
{ 
	return _elems[_keys[index]].a; 
}

LUATable* LUATable::get_table(const size_t index) 
{ 
	return _elems[_keys[index]].t; 
}

scLuaType LUATable::get_type(const std::string& index) 
{ 
	if (_elems.count(index)==0) return SCLUA_UNKNOWN;
	return _elems[index].type; 
}

float LUATable::get_float(const std::string& index) 
{ 
	return _elems[index].v; 
}

std::string	LUATable::get_string(const std::string& index) 
{ 
	return _elems[index].str; 
}

bool LUATable::get_bool(const std::string& index) 
{ 
	return _elems[index].b; 
}

LUAReference* LUATable::get_reference(const std::string& index) 
{ 
	return _elems[index].r;
}

LUAArray* LUATable::get_array(const std::string& index) 
{ 
	return _elems[index].a;
}

LUATable* LUATable::get_table(const std::string& index) 
{ 
	return _elems[index].t;
}

void LUATable::push_float(const std::string& index,const float f)
{
	if (_elems.count(index)==0)
	{
		_elems.insert(make_pair(index,scArrayElem(f)));
		_keys.push_back(index);
		return;
	}
	_elems[index].type=SCLUA_NUMBER;
	_elems[index].v=f;
}

void LUATable::push_string(const std::string& index,const std::string& str)
{
	if (_elems.count(index)==0)
	{
		_elems.insert(make_pair(index,scArrayElem(str)));
		_keys.push_back(index);
		return;
	}
	_elems[index].type=SCLUA_STRING;
	_elems[index].str=str;
}

void LUATable::push_bool(const std::string& index,const bool b)
{
	if (_elems.count(index)==0)
	{
		_elems.insert(make_pair(index,scArrayElem(b)));
		_keys.push_back(index);
		return;
	}
	_elems[index].type=SCLUA_BOOL;
	_elems[index].b=b;
}

void LUATable::push_reference(const std::string& index,LUAReference* p)
{
	if (_elems.count(index)==0)
	{
		_elems.insert(make_pair(index,scArrayElem(p)));
		_keys.push_back(index);
		return;
	}
	_elems[index].type=SCLUA_REFERENCE;
	_elems[index].r=p;
}

void LUATable::push_function(const std::string& index,LUAFunctionType pfunction)
{
	if (_elems.count(index)==0)
	{
		_elems.insert(make_pair(index,scArrayElem(pfunction)));
		_keys.push_back(index);
		return;
	}
	_elems[index].type=SCLUA_FUNCTION;
	_elems[index].func=pfunction;
}

void LUATable::push_array(const std::string& index,LUAArray* a)
{
	if (_elems.count(index)==0)
	{
		_elems.insert(make_pair(index,scArrayElem(a)));
		_keys.push_back(index);
		return;
	}
	_elems[index].type=SCLUA_ARRAY;
	_elems[index].a=a;
}

void LUATable::push_table(const std::string& index,LUATable* t)
{
	if (_elems.count(index)==0)
	{
		_elems.insert(make_pair(index,scArrayElem(t)));
		_keys.push_back(index);
		return;
	}
	_elems[index].type=SCLUA_TABLE;
	_elems[index].t=t;
}
