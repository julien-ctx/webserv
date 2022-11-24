# include <string>
# include <iostream>
# include <cstring>
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

bool	only_binary(std::string str)
    {
        for (size_t i = 0; i < str.length(); i++)
            if (str[i] != '0' && str[i] != '1')
                return false;
        return true;
    }

bool	only_octal(std::string str)
    {
        for (size_t i = 0; i < str.length(); i++)
            if (str[i] < '0' || str[i] > '7')
                return false;
        return true;
    }

bool    is_hexa(char c)
    {
        if ((c >= '0' && c <= '9') || (c >= 'A' && c <= 'F') || (c >= 'a' && c <= 'f'))
            return true;
        return false;
    }

//change
bool	str_is_int(std::string str)
    {
        size_t  count_nbr = 0;
        for (size_t i = 0; i < str.length(); i++)
        {
            if (i == 0 && str[i] != '-' && str[i] != '+' && (str[i] < '0' || str[i] > '9'))
                return false;
            if (i == 1 && (str[i] < '0' || str[i] > '9') && (str[i] != '_' || str[0] < '0' || str[0] > '9')
                && (str[i] != 'x' || str[0] != '0'))
                {
                    if (str[i] == 'o' && str[0] == '0' && str.length() > 2 && only_octal(str.substr(2, str.length() - 2)))
                        break;
                    else if (str[i] == 'b' && str[0] == '0' && str.length() > 2 && only_binary(str.substr(2, str.length() - 2)))
                        break;
                    else
						return false;
                }
            if (i > 1 && (str[i] < '0' || str[i] > '9')
                && (str[i] != '_' || str[1] != 'x' || count_nbr != 4)
                && (!is_hexa(str[i]) || str[1] != 'x')
                && (str[i] != '_' || str[1] == 'x' || !count_nbr || count_nbr > 3))
				{
					std::cout << str[i]<<std::endl;
                	return false;
				}
            if (str[i] == '_')
                count_nbr = 0;
            else if (is_hexa(str[i]))
                count_nbr++;
        }
        if (!is_hexa(str[str.length() - 1]))
            return false;
        return true;
    }

	float		char_to_int(char c)
	{
		if (c >= '0' && c <= '9')
			return static_cast<float>(c - 48);
		if (c >= 'a' && c <= 'f')
			return static_cast<float>(c - 87);
		if (c >= 'A' && c <= 'F')
			return static_cast<float>(c - 55);
		return 0;
	}

//change
	float		str_base_to_int(std::string str, size_t base)
	{
		float	nbr = 0;
		float	power;
        bool  minus = false;
        if (str[0] == '-')
        {
            str.erase(0, 1);
            minus = true;
        }
		for (size_t i = 0; i <  str.length(); i++)
		{
			power = 1;
			for (size_t j = 0; j <  (str.length() - i - 1); j++)
				power *= base;
			nbr += (power * char_to_int(str[i]));
		}
        if (minus)
            nbr *= (-1);
		return nbr;
	}

	float	atof(std::string str)
    {
		float	nbr;
		//int
		//supress every underscore
		for (size_t i = 0; i < str.length() ; i++)
		{
			if (str[i] == '_' || str[i] == '+')
			{
				str.erase(i, 1);
				if (i > 0)
					i--;
			}
		}
		//convert to binary, octal or hexadecimal
		if (str[1] == 'b')
			nbr = str_base_to_int(str.substr(2, str.length() - 2), 2);
		else if (str[1] == 'o')
			nbr = str_base_to_int(str.substr(2, str.length() - 2), 8);
		else if (str[1] == 'x')
			nbr = str_base_to_int(str.substr(2, str.length() - 2), 16);
		else
			nbr = str_base_to_int(str, 10);
		return nbr;
	}

int main(int ac, char **av)
{
	(void)ac;
	(void)av;
	// unsigned int k= 47;
	// char i = k;
	std::string str("0xA");
	if (str_is_int(str))
		std::cout << atof(str) << "|"<< std::endl;
	else
		std::cout << "You suck !"<< std::endl;

	return 0;
}


