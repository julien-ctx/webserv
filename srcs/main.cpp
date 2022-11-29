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

using namespace TOML;
int main(int ac, char **av)
{
	// (void)ac;
	// (void)av;
	parse	yo;
	try
	{
		if (ac == 1)
			yo.insert("king kong", "\"\"\"\\n i'am\"\"\" ");
		else
			yo.insert("king kong", av[1]);
	}
	catch( const std::exception & e ) {
        std::cerr << e.what() << std::endl;
		exit (1);
    }
	std::cout <<" Parent Key = " << yo._hash_tables[0]._parent->_key << " Key = " << yo._hash_tables[0]._key <<" value = ";
	if (yo._hash_tables[0]._typing == T_int)
		std::cout << yo._hash_tables[0]._int << std::endl;
	if (yo._hash_tables[0]._typing == T_float)
		std::cout << yo._hash_tables[0]._float << std::endl;
	if (yo._hash_tables[0]._typing == T_bool)
		std::cout << yo._hash_tables[0]._bool << std::endl;
	if (yo._hash_tables[0]._typing == T_string)
		std::cout << yo._hash_tables[0]._string << std::endl;
	return 0;
}

