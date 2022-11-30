# include <string>
# include <iostream>
# include <cstring>
# include <exception>
# include "../include/toml/parse.hpp"
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
	o << "Key = " << val._key;
	if (val._typing != TOML::T_table)
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
	else
	{
		o <<std::endl << "Typing = table has ";
		if (!val._is_array_table)
			o << "not " ;
		o << "an array" << std::endl;
	}
	o << "key parent = " << val._parent->_key << std::endl;
	return (o);
}

using namespace TOML;
int main(int ac, char **av)
{
	// (void)ac;
	// (void)av;
	parse	yo;
	try
	{
		if (ac == 1)
			yo.insert("king kong", "\'Yolo\'");
		else
		{
			for (size_t i = 1; i < static_cast<size_t>(ac) ; i++)
			{
				yo.parse_line(std::string(av[i]), i - 1);
			}
		}
	}
	catch( const std::exception & e ) {
        std::cerr << e.what() << std::endl;
		exit (1);
    }
	for (size_t i = 0; i < yo._hash_tables.size(); i++)
	{
		std::cout << yo._hash_tables[i];
	}
	
	return 0;
}

