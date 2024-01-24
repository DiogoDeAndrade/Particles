#ifndef __TOKENS_H
#define __TOKENS_H

#include <vector>

class Tokenizer  
{
public:
	typedef std::vector<char*>	Strings;
	typedef std::vector<char>	Delimiters;

	Tokenizer();
	virtual ~Tokenizer();

			void	clear_tokens();
			int		tokenize(const char *string,long max_tokens=-1);
			int		find(char *string);
			char	*get_token(unsigned int i);
	inline	char	*operator[](unsigned int i) { return get_token(i); }
	inline	size_t	get_token_number() { return _strings.size(); }

			void	add_delimiter(char c);
			bool	is_delimiter(char c);
			void	clear_delimiters();

	inline	void	set_no_string(bool b) { _no_string=b; }

protected:
	Strings		_strings;
	Delimiters	_delimiters;	
	bool		_no_string;
};

#endif // !defined(AFX_TOKENS_H__6EAE2E40_9BBC_41CE_AFB7_4A878112472D__INCLUDED_)
