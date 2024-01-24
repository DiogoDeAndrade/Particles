#ifndef __LUASCRIPT_H
#define __LUASCRIPT_H

#include <string>

class LUAScript
{
public:
	LUAScript(void);
public:
	virtual ~LUAScript(void);

	virtual	std::string	get_script_id() { return _script_id; }
	virtual	const char*	get_script_ptr() { return _script.c_str(); }
	virtual	bool		load_from_file(const std::string& filename);
	virtual	bool		load_from_string(const std::string& str);
	virtual	bool		add_from_string(const std::string& str);
	virtual	bool		clear();

protected:
	std::string	_script;
	std::string	_script_id;
};

#endif