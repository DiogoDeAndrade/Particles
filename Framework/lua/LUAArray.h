#ifndef __SCLUALIST_H
#define __SCLUALIST_H

#include "LUAEnv.h"
#include "LUATable.h"

class LUAReference;

class LUAArray
{
protected:
	struct scArrayElem
	{
		scArrayElem() { type=SCLUA_NIL; }
		scArrayElem(const float _v) { type=SCLUA_NUMBER; v=_v; }
		scArrayElem(const bool _b) { type=SCLUA_BOOL; b=_b; }
		scArrayElem(const std::string& _str) { type=SCLUA_STRING; str=_str; }
		scArrayElem(LUAArray* _a,const bool _should_delete) { type=SCLUA_ARRAY; a=_a; should_delete=_should_delete; }
		scArrayElem(LUATable* _t,const bool _should_delete) { type=SCLUA_TABLE; t=_t; should_delete=_should_delete; }
		scArrayElem(LUAReference* _ref) { type=SCLUA_REFERENCE; r=_ref; }
		scArrayElem(const scArrayElem& elem) 
		{
			type=elem.type;
			v=elem.v;
			b=elem.b;
			str=elem.str;
			a=elem.a;
			t=elem.t;
			r=elem.r;
			should_delete=elem.should_delete;
		}
		scLuaType		type;
		float			v;
		bool			b;
		std::string		str;
		LUAArray*		a;
		LUATable*		t;
		LUAReference*	r;
		bool			should_delete;
	};
	typedef std::vector<scArrayElem>	scArrayElems;
public:
	LUAArray(void);
	virtual ~LUAArray(void);

			void		clear();
			bool		load(lua_State* env,const int index);
			bool		push(lua_State* env,const bool as_table=true);
			void		dump();

			size_t			get_size();
			scLuaType		get_type(const size_t index);
			float			get_float(const size_t index);
			std::string		get_string(const size_t index);
			bool			get_bool(const size_t index);
			LUAArray*		get_array(const size_t index);
			LUATable*		get_table(const size_t index);
			LUAReference*	get_reference(const size_t index);

			void		set_size(const size_t size);
	inline	void		set_type(const size_t index,const scLuaType type) { _elems[index].type=type; }
	inline	void		set_nil(const size_t index) { _elems[index].type=SCLUA_NIL; }
	inline	void		set_float(const size_t index,const float f) { _elems[index].type=SCLUA_NUMBER; _elems[index].v=f; }
	inline	void		set_string(const size_t index,const std::string& str) { _elems[index].type=SCLUA_STRING; _elems[index].str=str; }
	inline	void		set_bool(const size_t index,const bool b) { _elems[index].type=SCLUA_BOOL; _elems[index].b=b; }
	inline	void		set_array(const size_t index,LUAArray* a,const bool should_delete=true) { _elems[index].type=SCLUA_ARRAY; _elems[index].a=a; _elems[index].should_delete=should_delete; }
	inline	void		set_table(const size_t index,LUATable* t) { _elems[index].type=SCLUA_TABLE; _elems[index].t=t; }
	inline	void		set_reference(const size_t index,LUAReference* ptr) { _elems[index].type=SCLUA_REFERENCE; _elems[index].r=ptr; }
	inline	void		push_float(const float f) { scArrayElem e(f); _elems.push_back(e); }
	inline	void		push_string(const std::string& str) { scArrayElem e(str); _elems.push_back(e); }
	inline	void		push_bool(const bool b) { scArrayElem e(b); _elems.push_back(e); }
	inline	void		push_array(LUAArray* a,bool should_delete=true) { scArrayElem e(a,should_delete); _elems.push_back(e); }
	inline	void		push_table(LUATable* t,bool should_delete=true) { scArrayElem e(t,should_delete); _elems.push_back(e); }
	inline	void		push_reference(LUAReference* r) { scArrayElem e(r); _elems.push_back(e); }
	inline	void		push_nil() { scArrayElem e; _elems.push_back(e); }

protected:
			void		dump(scArrayElem* elem);

protected:
	scArrayElems	_elems;
};

#endif