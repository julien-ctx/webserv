# include <string>
# include <iostream>
# include <fstream>
# include <cstring>
# include <exception>
# include "../include/toml/parse.hpp"
# include "../include/config_parser.hpp"
class b
{
private:
	std::string s;
public:
	b() : s("bruh"){}
	~b()
	{
		std::cout << "issou" << std::endl;
	}
};

class a
{
	private:
		b b;
	public:
		a() : b() {}
		~a()
		{
			std::cout << "Daft punk" << std::endl;
		}
};


std::ostream& operator<< (std::ostream& o, TOML::value& val)
{
	if (val._typing == TOML::T_table)
		o << "\033[0;31m" << std::endl;
	if (val._typing == TOML::T_int)
		o << "\033[0;32m" << std::endl;
	if (val._typing == TOML::T_float)
		o << "\033[0;33m" << std::endl;
	if (val._typing == TOML::T_bool)
		o << "\033[0;34m" << std::endl;
	if (val._typing == TOML::T_string)
		o << "\033[0;36m" << std::endl;
	if (val._typing == TOML::T_array)
		o << "\033[0;35m" << std::endl;
	o << "Key = " << val._key;
	if (val._typing != TOML::T_table && val._typing != TOML::T_array)
	{
		o << " Value = ";
		if (val._typing == TOML::T_int)
			o << val._int << std::endl << "Typing = int";
		if (val._typing == TOML::T_float)
			o << val._float << std::endl << "Typing = float";
		if (val._typing == TOML::T_bool)
			o << val._bool << std::endl << "Typing = bool";
		if (val._typing == TOML::T_string)
			o << val._string << std::endl << "Typing = string";
		o << std::endl;
	}
	else if (val._typing == TOML::T_table)
	{
		o <<std::endl << "Typing = table has ";
		if (!val._is_array_table)
			o << "not " ;
		o << "an array" << std::endl;
		if (val._is_array_table)
		{
			for(size_t i = 0;i <val._array.size() ;i++)
			{
				o << "child = " << val._array[i]._key << std::endl;
			}
		}
	}
	if (val._typing == TOML::T_array)
	{
		o << " Value = "<< std::endl;
		for(size_t i = 0;i <val._array.size() ;i++)
		{
			o << "child = " << val._array[i]._key << std::endl;
		}
		o << "Typing = array" << std::endl;
	}
	o << "key parent = " << val._parent << " coord = " << &val << "\033[0m" << std::endl;
	return (o);
}

using namespace TOML;
#include "../includes/server.hpp"
#include "../includes/response.hpp"

void *serv_thread(void *server)
{
	Response resp;
	Server *serv;

	serv = (Server *)server;
	serv->binder();
	serv->listener();
	serv->launch();
	return NULL;
}

int main(int ac, char **av)
{

	parse yo;
	try
	{
		if (ac < 2)
			yo = config_parsing("config/default.TOML");
		else
			yo = config_parsing(av[1]);

	}
	catch( const std::exception & e ) {
        std::cerr << "Config error : " << e.what() << std::endl;
		return (1);
    }
	for (size_t i = 0; i < yo._hash_tables.size(); i++)
	{
		std::cout << yo._hash_tables[i];
	}

	pthread_t s1;
	// pthread_t s2;

	Server serv1(4242);
	// Server serv2(4343);

	pthread_create(&s1, NULL, serv_thread, &serv1);
	// pthread_create(&s2, NULL, serv_thread, &serv2);
	while (1);




}

