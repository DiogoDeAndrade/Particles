#include "LUAArray.h"
#include "LUAHelpers.h"
#include "../Logger.h"

LUAArray::LUAArray(void)
{
}

LUAArray::~LUAArray(void)
{
	clear();
}

void LUAArray::clear()
{
	for (size_t i=0; i<_elems.size(); i++)
	{
		if (_elems[i].type==SCLUA_ARRAY)
		{
			if (_elems[i].should_delete) delete _elems[i].a;
		}
		else if (_elems[i].type==SCLUA_TABLE)
		{
			if (_elems[i].should_delete) delete _elems[i].t;
		}
	}
	_elems.clear();
}

bool LUAArray::load(lua_State* env,const int index)
{
	_elems.clear();

	int idx;
	if (index>=0) idx=index+1;
	else idx=index;

	if (!lua_istable(env,idx))
	{
		return false;
	}
	size_t elem_count=lua_objlen(env,idx);
	for (int i=1; i<int(elem_count+1); i++)
	{
		lua_rawgeti(env,idx,i);

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
			case LUA_TTABLE:
				{
					size_t elem_count=lua_objlen(env,-1);
					if (elem_count>0)
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
				}
				break;
			case LUA_TUSERDATA:
				elem.type=SCLUA_REFERENCE;
				elem.r=(LUAReference*)lua_touserdata(env,-1);
				break;
			case LUA_TLIGHTUSERDATA:
				elem.type=SCLUA_REFERENCE;
				elem.r=(LUAReference*)lua_touserdata(env,-1);
				break;
			case LUA_TNIL:
				elem.type=SCLUA_NIL;
				break;
			default:
#ifdef __WIN32
				__asm int 3
#endif
				
				break;
		}
		_elems.push_back(elem);
		lua_pop(env,1);
	}
	return true;
}

bool LUAArray::push(lua_State* env,const bool as_table)
{
	if (as_table) lua_newtable(env);
	for (int i=0; i<int(_elems.size()); i++)
	{
		switch (_elems[i].type)
		{
			case SCLUA_NIL: lua_pushnil(env); break;
			case SCLUA_STRING: lua_push_string(env,_elems[i].str.c_str()); break;
			case SCLUA_NUMBER: lua_push_number(env,_elems[i].v); break;
			case SCLUA_BOOL: lua_push_boolean(env,_elems[i].b); break;
			case SCLUA_REFERENCE: lua_push_reference(env,_elems[i].r); break;
			case SCLUA_ARRAY:
				{
					_elems[i].a->push(env,true);
				}
				break;
			case SCLUA_TABLE:
				{
					_elems[i].t->push(env);
				}
				break;
#ifdef __WIN32
				__asm int 3
#endif
				break;
		}
		if (as_table) lua_rawseti(env,-2,i+1);
	}
	return true;
}

void LUAArray::set_size(const size_t size)
{
	for (size_t i=_elems.size(); i<size; i++)
	{
		scArrayElem elem;
		elem.type=SCLUA_NIL;
		_elems.push_back(elem);
	}
}

void LUAArray::dump()
{
	for (size_t i=0; i<_elems.size(); i++)
	{
		dump(&(_elems[i]));
	}
}

void LUAArray::dump(scArrayElem* elem)
{
	switch (elem->type)
	{
		case SCLUA_NIL: log("nil"); break;
		case SCLUA_BOOL: log("%s",(elem->b)?("true"):("false")); break;
		case SCLUA_STRING: log("\"%s\"",elem->str.c_str()); break;
		case SCLUA_NUMBER: log("%f",elem->v); break;
		case SCLUA_REFERENCE: log("0x%x",elem->r); break;
		case SCLUA_ARRAY:
			{
				log("{");
				for (size_t i=0; i<elem->a->_elems.size(); i++)
				{
					dump(&(elem->a->_elems.at(i)));
				}			
				log("}");
			}
			break;
		case SCLUA_TABLE:
			{
				log("t{");
				elem->t->dump();
				log("}");
			}
			break;
	}
}

LUAArray*	LUAArray::get_array(const size_t index) 
{ 
	LUAArray* tmp=_elems[index].a;
	return tmp; 
}

LUATable*	LUAArray::get_table(const size_t index) 
{ 
	LUATable* tmp=_elems[index].t;
	return tmp; 
}

size_t LUAArray::get_size() 
{ 
	return _elems.size(); 
}

scLuaType LUAArray::get_type(const size_t index) 
{ 
	return _elems[index].type; 
}

float LUAArray::get_float(const size_t index) 
{ 
	return _elems[index].v; 
}

LUAReference* LUAArray::get_reference(const size_t index) 
{ 
	return _elems[index].r; 
}

std::string	LUAArray::get_string(const size_t index) 
{ 
	return _elems[index].str; 
}

bool LUAArray::get_bool(const size_t index) 
{ 
	return _elems[index].b; 
}
