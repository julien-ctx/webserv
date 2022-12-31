# include <string>
# include <iostream>
# include <fstream>
# include <cstring>
# include <exception>
# include "../includes/toml/parse.hpp"
# include "../includes/config_parser.hpp"
# include "../includes/server.hpp"
# include "../includes/response.hpp"

using namespace TOML;

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

void *serv_routine(void *server)
{
	try
	{
		Response resp;
		Server *serv;

		serv = (Server *)server;
		serv->binder();
		serv->listener();
		serv->launch();
	}
	catch (const std::exception &e)
	{
		std::cerr << RED << "Server error : " << e.what() << RESET << std::endl;
	}
		return NULL;
}

int main(int ac, char **av)
{
	parse pars;
	try
	{
		if (ac < 2)
			pars = config_parsing("config/default.TOML");
		else
			pars = config_parsing(av[1]);

	}
	catch (const std::exception &e)
	{
        std::cerr << RED << "Config error : " << e.what() << RESET << std::endl;
		return (1);
    }

	TOML::parse::pointer point = pars.at_key_parent(string("server"), "");

	try
	{
		size_t serv_size = point->_array.size();	
		if (serv_size > MAX_SERVERS)
			exit_error("too much servers", 1);
		pthread_t *threads = new pthread_t[serv_size];

		if (serv_size == 1)
		{
			Server *serv = new Server(&pars, 0);
			serv_routine(serv);
		}
		else
		{
			for (size_t i = 0; i < serv_size; i++)
			{
				Server *serv = new Server(&pars, i);
				usleep(100);
				pthread_create(&threads[i], NULL, serv_routine, serv);
			}
		}

		while (1);
	}
	catch (const std::exception &e)
	{
		std::cerr << RED << "Server error : " << e.what() << RESET << std::endl;
		return 1;
	}
	return 0;
}
