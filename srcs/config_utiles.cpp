# include "../include/config_utiles.hpp"

using namespace std;

vector<string>  ft_split(string str, string occ)
{
    vector<string> vec;
	size_t  begin = 0;
	size_t  len = 0;

	for (size_t i = 0; i < str.length(); i++ )
	{
		if (!occ.compare(str.substr(i, occ.length())))
		{
			vec.push_back(str.substr(begin, len));
			i += occ.length();
			begin = i;
			len = 0;
		}
		len++;
	}
	vec.push_back(str.substr(begin, len));
	return vec;
}

int	ft_atoi(string str)
{
	int	nbr = 0;
	int	power;
	for (size_t i = 0; i <  str.length(); i++)
	{
		power = 1;
		for (size_t j = 0; j <  (str.length() - i - 1); j++)
			power *= 10;
		nbr += (power * static_cast<int>(str[i]- 48));
	}
	return nbr;
}