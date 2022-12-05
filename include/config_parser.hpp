#ifndef CONFIG_PARSER_H
# define CONFIG_PARSER_H
# include <string>
# include <iostream>
# include <cstring>
# include <fstream>
# include <sstream>
# include <exception>
# include "toml/parse.hpp"


using namespace std;

//functions
void			verif_folder_file(string path);
TOML::parse		config_parsing(const char * path);

//exceptions
class ConfigError : public std::exception
{
    private:
    const char *_msg;

    public:
    ConfigError(const char *msg) : _msg(msg){}
    virtual ~ConfigError() _NOEXCEPT {}
    const char *	what(void) const throw()
    {
        return _msg;
    }
};

class FileError : public std::exception
{
    private:
     std::string _msg;

    public:
    FileError(const char *msg, const string file) : _msg(msg)
    {
        std::stringstream ss;

        ss << "The file " << file;
        ss << msg;
        _msg = ss.str();
    }
    virtual ~FileError() _NOEXCEPT {}
    const char *	what(void) const throw()
    {
        return _msg.c_str();
    }
};



#endif