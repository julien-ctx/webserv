#ifndef EXCEPTIONS_H
# define EXCEPTIONS_H
# include <string>
# include <iostream>
# include <cstring>
# include <fstream>
# include <sstream>
# include <exception>

namespace TOML
{
    class TypeUndefined : public std::exception
	{
        private:
        const char *_msg;

		public:
        TypeUndefined(const char *msg) : _msg(msg){}
        ~TypeUndefined(){}
		const char *	what(void) const throw()
        {
           return _msg;
        }
	};

    class ErrorParse : public std::exception
	{
        private:
        std::string _msg;

		public:
        ErrorParse(const char * msg, size_t line)
        {
			std::stringstream ss;

			ss << what;
			ss << " at line " << line;
			_msg = ss.str();
        }
        ~ErrorParse(){}
		const char *	what(void) const throw()
        {
            return _msg.c_str();
        }
	};

    
    
}

#endif