#include "Tokenizer.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Tokenizer::Tokenizer()
{
	add_delimiter('\t');
	add_delimiter(10);
	add_delimiter(13);
	add_delimiter(32);
	_no_string=false;
}

Tokenizer::~Tokenizer()
{
	clear_tokens();
	clear_delimiters();
}

void Tokenizer::clear_tokens()
{
	for (unsigned long i=0; i<_strings.size(); i++)
	{
		free (_strings[i]);
	}
	_strings.clear();
}

int Tokenizer::tokenize(const char *string,long max_tokens)
{
	clear_tokens();

	const char *begin=string;
	const char *end=string;
	bool exit=false;

	if (!string) return 0;

	do
	{
		// Search for first non zero
		bool inner=false;
		do
		{
			if (*begin==0) { inner=true; exit=true; }
			else if (!is_delimiter(*begin)) inner=true;
			else begin++;
		} while (!inner);

		if ((*begin=='\"') && (!_no_string))
		{
			if (!exit)
			{
				// Search for ending of token (" or ')
				inner=false;
				end=begin;
				do
				{
					end++;
					if (*end==0)
					{
						inner=true;
						exit=true;
					}
					if (*end=='\"')
					{
						inner=true;
					}
				} while (!inner);
				end++;
			}
		}
		else
		{
			if (!exit)
			{
				// Search for ending of token
				inner=false;
				end=begin;
				do
				{
					end++;
					if (*end==0)
					{
						inner=true;
						exit=true;
					}
					if (is_delimiter(*end))
					{
						inner=true;
					}
				} while (!inner);
			}
		}

		// Get size of token
		int len=int(end-begin);
		if (len>0)
		{
			// Allocate memory
			char *tmp;
			if ((*begin=='\"') && (!_no_string))
			{
				tmp=(char*)malloc(len-1);
				memcpy(tmp,begin+1,len-2);
				*(tmp+len-2)=0;
			}
			else 
			{
				tmp=(char*)malloc(len+1);
				memcpy(tmp,begin,len);
				*(tmp+len)=0;
			}
			
			_strings.push_back(tmp);
		}

		begin=end;

		if (max_tokens>0)
		{
			if (_strings.size()==max_tokens) return (int)(_strings.size());
		}

	} while (!exit);

	return (int)(_strings.size());
}

int Tokenizer::find(char *string)
{
	for (unsigned long i=0; i<_strings.size(); i++)
	{
		char *tmp=get_token(i);
		if (tmp)
		{
			if (!_stricmp(string,tmp)) return i;
		}
	}

	return -1;
}

char *Tokenizer::get_token(unsigned int i)
{
	if (i==-1) return NULL;

	if (i>=_strings.size()) return NULL;

	return _strings[i];
}

void Tokenizer::add_delimiter(char c)
{
	_delimiters.push_back(c);
}

bool Tokenizer::is_delimiter(char c)
{
	int j=(int)(_delimiters.size());

	for (int i=0; i<j; i++)
	{
		if (_delimiters[i]==c) return true;
	}
	return false;
}

void Tokenizer::clear_delimiters()
{
	_delimiters.clear();
}
