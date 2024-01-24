
#include "Logger.h"
#include <stdio.h>
#include <stdarg.h>

bool g_first_log=true;

void log_string(const char* str)
{
	FILE *file=NULL;
	if (g_first_log) fopen_s(&file,"log.txt","wt");
	else fopen_s(&file,"log.txt","at");
	if (!file) return;

	fprintf(file,"%s\n",str);

	fclose(file);
	g_first_log=false;
}

void log(const char* str,...)
{
	static	char	buffer[8192];
	va_list			argptr;

	va_start(argptr,str);
	vsprintf_s(buffer,8192,str,argptr);
	va_end(argptr);

	log_string((char*)&buffer);
}
