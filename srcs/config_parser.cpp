# include "../include/config_parser.hpp"
# include "../include/config_utiles.hpp"

using namespace std;

//limits values
//int
void	limits_value_int(__int64_t min, __int64_t max, string key, __int64_t actual_value)
{
	if (actual_value < min || actual_value > max)
		throw (" has an invalid value", key);
}

//str
void	limits_value_str(vector<string> vec,string key, string actual_value)
{
	for (size_t i = 0; i < vec.size(); i++)
	{
		if (!vec[i].compare(actual_value))
			return ;
	}
		throw (" has an invalid value", key);
}

void	adress_parse(string value)
{
	size_t begin = 0;
	size_t dot_counter = 0;
	size_t nbr_counter = 0;
	size_t adress_counter = 0;
	if (value[0] == '.' || value[value.size() -1] == '.')
		throw (" has an invalid value", string("adress"));
	for (size_t i = 0; i < value.size(); i++)
	{
		if ((value[i] > '9' || value[i] < '0') && value[i] != '.')
			throw (" has an invalid value", string("adress"));
		if (value[i] == '.')
		{
			dot_counter++;
			adress_counter++;
			if (adress_counter > 4 || dot_counter > 1 || ft_atoi(value.substr(begin, nbr_counter)) > 255)
				throw (" has an invalid value", string("adress"));
			nbr_counter = 0;
			begin = i + 1;
		}
		else
		{
			nbr_counter++;
			if (nbr_counter > 3)
				throw (" has an invalid value", string("adress"));
			dot_counter = 0;
		}
	}
	if (adress_counter != 3)
		throw (" has an invalid value", string("adress"));
	
}
//verify if value exist and if dosn't add it with a default value
//tables
void	exist_good_type(TOML::parse *pars, string key, TOML::types t, bool b, string str)
{
	TOML::parse::pointer	point;
	point = pars->at_key_parent(key, pars->_here);
	if (point == NULL)
	{
		if (t == TOML::types::T_table)
			pars->insert_table(key, b);
		else if (t == TOML::types::T_array)
			pars->insert_array(key, str);
		else
			pars->insert(key, str);
		point = pars->at_key_parent(key, pars->_here);
	}
	if (point->_typing != t)
		throw TypeError("hasn't the good type", key);
}

void	verif_content(TOML::parse *pars)
{
	TOML::parse::pointer	point;
	vector<int>				vec_port;
	vector<string>			vec_addr;



	pars->_here = string("");
	exist_good_type(pars, string("server"), TOML::types::T_table, true,string(""));
	point = pars->at_key_parent(string("server"), pars->_here);
	for (size_t i = 0; i < point->_array.size(); i++)
	{
		pars->_here = "server";
		pars->adding_here(to_string(i));

		exist_good_type(pars, string("port"), TOML::types::T_int, false, string("4242"));
		limits_value_int(0, 65535, string("port"), pars->at_key_parent(string("port"), pars->_here)->_int);

		exist_good_type(pars, string("adress"), TOML::types::T_string, false, string("\"0.0.0.0\""));
		adress_parse(pars->at_key_parent(string("adress"), pars->_here)->_string);

		vec_port.push_back(pars->at_key_parent(string("port"), pars->_here)->_int);
		vec_addr.push_back(pars->at_key_parent(string("adress"), pars->_here)->_string);
		for (size_t i = 0; i < vec_port.size(); i++)
		{
			if (pars->at_key_parent(string("port"), pars->_here)->_int == vec_port[i])
			{
				for (size_t j = 0; j < vec_addr.size(); j++)
				{
					if (!pars->at_key_parent(string("adress"), pars->_here)->_string.compare(vec_addr[j]))
						throw ConfigError("Can't having two servers with the same adress and same port");
				}
			}
		}
		exist_good_type(pars, string("server_name"), TOML::types::T_array, false, string("[\"\"]"));
		exist_good_type(pars, string("body_size"), TOML::types::T_int, false, string("1048576"));
		limits_value_int(0, 1048576, string("body_size"), pars->at_key_parent(string("body_size"), pars->_here)->_int);
		// we'll see for error pages
		// exist_good_type(pars, string("location"), TOML::types::T_table, false, string(""));
	}
	


}

//verify if the folder or
void	verif_folder_file(string path)
{
	if (path[path.size() - 1] == '/')
		throw ConfigError("Invalid Path");
	for (size_t i = 0; i < path.size(); i++)
	{
		if (path[i] == '/' && path[i + 1] == '/')
			throw ConfigError("Invalid Path");
	}
	vector<string> vec(ft_split(path, string("/")));

	string str_file(vec[vec.size() - 1]);
	if (str_file.size() < 6 || str_file.substr(str_file.size() - 5, 5).compare(".TOML"))
		throw FileError(" isn't a TOML file", str_file);

	ifstream file(path);
	if (!file)
		throw FileError(" doesn't exist", str_file);
	file.close();
}


TOML::parse    config_parsing(const char * path)
{
	string	str_file;
	if (path == NULL)
		str_file = "config/default.TOML";
	else
		str_file = path;
	verif_folder_file(str_file);
	TOML::parse pars(str_file);

	return pars;
}