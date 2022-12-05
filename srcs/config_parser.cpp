# include "../include/config_parser.hpp"
# include "../include/config_utiles.hpp"

using namespace std;

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