#include "LUAScript.h"
#include "LUA.h"
#include "../Logger.h"

LUAScript::LUAScript(void)
{
	_script="";
	_script_id="Unknown";
}

LUAScript::~LUAScript(void)
{
}

bool LUAScript::load_from_file(const std::string& filename)
{
	// Try filename
	FILE* file=NULL;
	fopen_s(&file,filename.c_str(),"rt");
	if (!file)
	{
		log("Can't read script file \"%s\"!",filename.c_str());
		return false;
	}
	
	char buffer[8192];

	_script="";
	while (!feof(file))
	{
		memset(buffer,0,8192);
		fgets((char*)&buffer,8192,file);
		if (buffer[0]!=0)
		{		
			_script+=buffer;
		}
	}

	fclose(file);
 
	_script_id=filename;

	return true;
}

bool	LUAScript::load_from_string(const std::string& str)
{
	_script=str;
	_script_id="string";
	return true;
}

bool	LUAScript::add_from_string(const std::string& str)
{
	_script=_script+"\n"+str;
	_script_id="string";
	return true;
}

bool	 LUAScript::clear()
{
	_script="";
	return true;
}
