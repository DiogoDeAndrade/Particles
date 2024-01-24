#ifndef __SCLUATABLE_H
#define __SCLUATABLE_H

#include "LUAEnv.h"
#include <map>

class LUAReference;

class LUATable
{
protected:
	struct scArrayElem
	{
		scArrayElem() { type=SCLUA_NIL; }
		scArrayElem(const float _v) { type=SCLUA_NUMBER; v=_v; }
		scArrayElem(const bool _b) { type=SCLUA_BOOL; b=_b; }
		scArrayElem(const std::string& _str) { type=SCLUA_STRING; str=_str; }
		scArrayElem(LUATable* _t) { type=SCLUA_TABLE; t=_t; }
		scArrayElem(LUAArray* _a) { type=SCLUA_ARRAY; a=_a; }
		scArrayElem(LUAFunctionType _func) { type=SCLUA_FUNCTION; func=_func; }
		scArrayElem(LUAReference* _ref) { type=SCLUA_REFERENCE; r=_ref; }
		scArrayElem(const scArrayElem& elem) 
		{
			type=elem.type;
			v=elem.v;
			b=elem.b;
			str=elem.str;
			a=elem.a;
			t=elem.t;
			func=elem.func;
			r=elem.r;
		}
		scLuaType			type;
		float				v;
		bool				b;
		std::string			str;
		LUATable*			t;
		LUAArray*			a;
		LUAFunctionType	func;
		LUAReference*		r;
	};
	typedef std::map<std::string,scArrayElem>	scArrayElems;
	typedef std::vector<std::string>			scKeyNames;
public:
	LUATable(void);
	virtual ~LUATable(void);

			void		clear();
			bool		load(lua_State* env,const int index);
			bool		push(lua_State* env);
			void		dump();

			size_t			get_size();
			std::string		get_key(const size_t index);
			scLuaType		get_type(const size_t index);
			float			get_float(const size_t index);
			std::string		get_string(const size_t index);
			bool			get_bool(const size_t index);
			LUAReference*	get_reference(const size_t index);
			LUAArray*		get_array(const size_t index);
			LUATable*		get_table(const size_t index);

			scLuaType		get_type(const std::string& index);
			float			get_float(const std::string& index);
			std::string		get_string(const std::string& index);
			bool			get_bool(const std::string& index);
			LUAReference*	get_reference(const std::string& index);
			LUAArray*		get_array(const std::string& index);
			LUATable*		get_table(const std::string& index);

			void		push_float(const std::string& index,const float f);
			void		push_string(const std::string& index,const std::string& str);
			void		push_bool(const std::string& index,const bool b);
			void		push_function(const std::string& index,LUAFunctionType pfunction);
			void		push_reference(const std::string& index,LUAReference* ptr);
			void		push_table(const std::string& index,LUATable* ptr);
			void		push_array(const std::string& index,LUAArray* ptr);

protected:
			void		dump(const std::string& key_name,scArrayElem* elem);

protected:
	scArrayElems	_elems;
	scKeyNames		_keys;
};

#endif