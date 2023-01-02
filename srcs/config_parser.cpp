# include "../includes/config_parser.hpp"
# include "../includes/config_utiles.hpp"
 using namespace std;
 //limits values
//int
void	limits_value_int(__int64_t min, __int64_t max, string key, __int64_t actual_value)
{
	if (actual_value < min || actual_value > max)
		throw TypeError(" has an invalid value", key);
}
 //str
bool	limits_value_str(vector<string> vec, string actual_value)
{
	for (size_t i = 0; i < vec.size(); i++)
	{
		if (!vec[i].compare(actual_value))
			return true;
	}
	return false;
}
 void	address_parse(string value)
{
	size_t begin = 0;
	size_t dot_counter = 0;
	size_t nbr_counter = 0;
	size_t address_counter = 0;
	if (value[0] == '.' || value[value.size() -1] == '.')
		throw TypeError(" has an invalid value", string("address"));
	for (size_t i = 0; i < value.size(); i++)
	{
		if ((value[i] > '9' || value[i] < '0') && value[i] != '.')
			throw TypeError(" has an invalid value", string("address"));
		if (value[i] == '.')
		{
			dot_counter++;
			address_counter++;
			if (address_counter > 4 || dot_counter > 1 || ft_atoi(value.substr(begin, nbr_counter)) > 255)
				throw TypeError(" has an invalid value", string("address"));
			nbr_counter = 0;
			begin = i + 1;
		}
		else
		{
			nbr_counter++;
			if (nbr_counter > 3)
				throw TypeError(" has an invalid value", string("address"));
			dot_counter = 0;
		}
	}
	if (address_counter != 3)
		throw TypeError(" has an invalid value", string("address"));
	
}
//verify if value exist and if dosn't add it with a default value
//tables
void	exist_good_type(TOML::parse *pars, string key, TOML::types t, bool b, string str)
{
	TOML::parse::pointer	point;
	point = pars->at_key_parent(key, pars->_here);
	if (point == NULL)
	{
		if (t == (TOML::T_table))
			pars->insert_table(key, b);
		else if (t == TOML::T_array)
			pars->insert_array(key, str);
		else
			pars->insert(key, str);
		point = pars->at_key_parent(key, pars->_here);
	}
	if (point->_typing != t || (t == TOML::T_table && b != point->_is_array_table))
		throw TypeError(" hasn't the good type", key);
}
 void	verif_content(TOML::parse *pars)
{
	TOML::parse::pointer					point;
	vector<pair<int, string> >				vec_port_addr;
	vector<pair<int, pair<int, string> > >	vec_first;
	bool									is_first;
	vector<string>							methods;
	vector<string>							cgi_ex;
	vector<string>							route_name;
	// TOML::value								bruh;
 	// for (size_t i = 0; i < pars->_hash_tables.size(); i++)
	// {
	// 	std::cout << pars->_hash_tables[i];
	// }
 	methods.push_back(string("POST"));
	methods.push_back(string("GET"));
	methods.push_back(string("DELETE"));
 	//to see with others
	cgi_ex.push_back(string("py"));
	cgi_ex.push_back(string("sh"));
	cgi_ex.push_back(string("pl"));
	pars->_here = string("");
	exist_good_type(pars, string("server"), TOML::T_table, true,string(""));
	point = pars->at_key_parent(string("server"), string(""));
	pars->_here = "server";
	if (pars->at_key_parent(to_string(0),  pars->_here) == NULL)
	{
		pars->insert_table(to_string(0), false);
		point->_array.push_back(*pars->at_key_parent(to_string(0), pars->_here));
	}
	for (size_t i = 0; i < point->_array.size(); i++)
	{
		pars->_here = "server";
		pars->adding_here(to_string(i));
		exist_good_type(pars, string("port"), TOML::T_int, false, string("4242"));
		limits_value_int(0, 65535, string("port"), pars->at_key_parent(string("port"), pars->_here)->_int);
		// bruh = *(pars->at_key_parent(string("port"), pars->_here));
		// std::cout << bruh << std::endl;
 		exist_good_type(pars, string("address"), TOML::T_string, false, string("\"0.0.0.0\""));
		address_parse(pars->at_key_parent(string("address"), pars->_here)->_string);
		// bruh = *(pars->at_key_parent(string("address"), pars->_here));
		// std::cout << bruh << std::endl;
 		vec_port_addr.push_back(pair<int, string>(pars->at_key_parent(string("port"), pars->_here)->_int, pars->at_key_parent(string("address"), pars->_here)->_string));
 		if (!vec_first.size())
		{
			vec_first.push_back(pair<int, pair<int, string> >(i, vec_port_addr[vec_port_addr.size() - 1]));
			exist_good_type(pars, string("default"), TOML::T_bool, false, string("true"));
			pars->at_key_parent(string("default"), pars->_here)->_bool = true;
		}
		else
		{
			is_first = true;
			for (size_t j = 0; j < vec_first.size(); j++)
			{
				if (vec_port_addr[vec_port_addr.size() - 1].first == vec_first[j].second.first
					&& !vec_port_addr[vec_port_addr.size() - 1].second.compare(vec_first[j].second.second) )
					is_first = false;
			}
			if (!is_first)
			{
				exist_good_type(pars, string("default"), TOML::T_bool, false, string("false"));
				pars->at_key_parent(string("default"), pars->_here)->_bool = false;
			}
			else
			{
				vec_first.push_back(pair<int, pair<int, string> >(i, vec_port_addr[vec_port_addr.size() - 1]));
				exist_good_type(pars, string("default"), TOML::T_bool, false, string("true"));
				pars->at_key_parent(string("default"), pars->_here)->_bool = true;
			}
		}
		exist_good_type(pars, string("cgi_dir"), TOML::T_string, true, string("\"/cgi\""));
		exist_good_type(pars, string("cgi_extension"), TOML::T_array, true, string("[\"py\", \"sh\", \"pl\"]"));
		for (size_t k = 0; k < pars->at_key_parent(string("cgi_extension"), pars->_here)->_array.size(); k++)
		{
			if (!limits_value_str(cgi_ex, pars->at_key_parent(to_string(k), (pars->_here + string(".cgi_extension")))->_string))
					throw TypeError(" has an invalid value", to_string(k));
		}
 		exist_good_type(pars, string("server_name"), TOML::T_array, false, string("[\"\"]"));
 		if (pars->at_key_parent(to_string(0), (pars->_here + string(".server_name")))->_typing != TOML::T_string )
			throw TypeError(" hasn't the good type", to_string(i));
		
		exist_good_type(pars, string("body_size"), TOML::T_int, false, string("1048576"));
		limits_value_int(0, 1048576, string("body_size"), pars->at_key_parent(string("body_size"), pars->_here)->_int);
		
		// we'll see for error pages with others
 		exist_good_type(pars, string("location"), TOML::T_table, true, string(""));
		point = pars->at_key_parent(string("location"), pars->_here);
		pars->adding_here(string("location"));
		if (pars->at_key_parent(to_string(0),  pars->_here) == NULL)
		{
			pars->insert_table(to_string(0), false);
			point->_array.push_back(*pars->at_key_parent(to_string(0), pars->_here));
		}
		route_name.clear();
		for (size_t j = 0; j < point->_array.size(); j++)
		{
			pars->_here = string("server.") + to_string(i) + string(".location");
			pars->adding_here(to_string(j));
			exist_good_type(pars, string("route"), TOML::T_string, true, string("\"/\""));
			if (limits_value_str(route_name, pars->at_key_parent(string("route"), (pars->_here))->_string))
				throw ConfigError("Only one name per route per server");
			route_name.push_back(pars->at_key_parent(string("route"), (pars->_here))->_string);
			exist_good_type(pars, string("root"), TOML::T_string, true, string("\"/www\""));
			exist_good_type(pars, string("index"), TOML::T_string, true, string("\"\""));
			exist_good_type(pars, string("auto_index"), TOML::T_bool, true, string("false"));
			exist_good_type(pars, string("uploadable"), TOML::T_bool, true, string("false"));
			exist_good_type(pars, string("error_page"), TOML::T_string, false, string("\"\""));
			exist_good_type(pars, string("cookie_page"), TOML::T_string, false, string("\"\""));
 			//to see with the others
 			exist_good_type(pars, string("allowed_methods"), TOML::T_array, true, string("[\"POST\", \"GET\", \"DELETE\"]"));
			if (pars->at_key_parent(to_string(0), (pars->_here + string(".allowed_methods")))->_typing != TOML::T_string )
				throw TypeError(" hasn't the good type", to_string(i));
			for (size_t k = 0; k < pars->at_key_parent(string("allowed_methods"), pars->_here)->_array.size(); k++)
			{
				if(!limits_value_str(methods, pars->at_key_parent(to_string(k), (pars->_here + string(".allowed_methods")))->_string))
						throw TypeError(" has an invalid value", to_string(k));
			}
 			if (pars->at_key_parent(string("redirect"), pars->_here) != NULL
				&& pars->at_key_parent(string("redirect"), pars->_here)->_typing != TOML::T_string )
				throw TypeError(" hasn't the good type", string("redirect"));
 			point = pars->at_key_parent(string("location"), (string("server.") + to_string(i)));
		}
 		point = pars->at_key_parent(string("server"), string(""));
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
	verif_content(&pars);
	return pars;
}
