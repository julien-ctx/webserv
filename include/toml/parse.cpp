#include "parse.hpp"

namespace TOML
{
    //insertion of new values
    // interprete and change the string val, so it can be include in the _hash_tables
    void	parse::insert(type_string key, type_string val)
    {
        if (str_is_string(val))
        {
            if (val[0] == '"')
            {
                if (!val.substr(0, 3).compare("\"\"\"") )
                    val = double_quote_change_string(check_empty_string(val, 6));
                else
                    val = double_quote_change_string(check_empty_string(val, 2));
            }
            else
            {
                if (!val.substr(0, 3).compare("'''") )
                {
                    val = check_empty_string(val, 6);
                    if (val[0] == '\n')
                        val.erase(0, 1);
                }
                else
                    val = check_empty_string(val, 2);
            }
            type_table new_value(key, val);
        }
        else if (str_is_nbr(val))
        {
            TOML::types	t;
            if (str_is_bool(val))
                t = T_bool;
            else if (str_is_int(val))
                t = T_int;
            else
                t = T_float;
            type_table new_value(key, TOML::parse::atof(val), t);
        }
        else if (str_is_array(val))
        {
            
        }
    }

    //parse
    bool    parse::is_whitespace(char c)
    {
        if (c == ' ' || c == '\t' || c == '\n' || c == '\v' || c == '\f' || c == '\r')
            return true;
        return false;
    }

    bool    parse::is_lower(char c)
    {
        if ((c >= 'a' && c <= 'z'))
            return true;
        return false;
    }

    bool    parse::is_upper(char c)
    {
        if ((c >= 'A' && c <= 'Z'))
            return true;
        return false;
    }

	bool	parse::valid_key_char(char c)
    {
        if (is_hexa(c) || c == '"'|| c == '\'' || c == '_' || || c == '.' || is_whitespace(c))
            return true;
        return false;
    }

	bool	parse::valid_antislash(char c)
    {
        if (c == 'b' || c == 't' || c == 'r' || c == 't' || c == '"'  || c == 'u' || c == '\\' || (is_whitespace(c) && c != '\n'))
            return true;
        return false;
    }

    bool    parse::str_is_string(type_string str)
    {
        if (str.length() >= 6
            && ((str.substr(0, 3).compare("\"\"\"") == 0 && str.substr(str.length() - 3, 3).compare("\"\"\"") == 0)
            || (str.substr(0, 3).compare("'''") == 0 && str.substr(str.length() - 3, 3).compare("'''") == 0)))
        {
            if (str[0] == '"')
            {
				size_t	count_dquote = 0;
                for (size_t i = 3; i < str.length() - 3; i++)
				{
                    if (i > 3 && str[i - 1] == '\\' && !valid_antislash(str[i]))
                        return false;
					if (str[i] == '"' && str[i - 1] != '\\')
						count_dquote++;
					else
						count_dquote = 0;
					if (count_dquote >= 3)
						return false;
				}
                if (str[str.length() - 4] == '\\'  && str[str.length() - 5] != '\\')
                    return false;
            }
			if (str[0] == '\'')
            {
				size_t	count_quote = 0;
                for (size_t i = 3; i < str.length() - 3; i++)
				{
					if (str[i] == '\'')
						count_quote++;
					else
						count_quote = 0;
					if (count_quote >= 3)
						return false;
				}
            }
            return true;
        }
        if (str.length() >= 3 && ((str[0] == '"' && str[str.length() - 1] == '"') || (str[0] == '\'' && str[str.length() - 1] == '\'')))
        {
            if (str[0] == '"')
            {
                for (size_t i = 1; i < str.length() - 1; i++)
				{
                    if (i > 1 && str[i - 1] == '\\' && (!valid_antislash(str[i]) || is_whitespace(str[i]) || str[i] == '"'))
                        return false;

					if (i > 1 && str[i - 1] == '\\' && str[i] == 'u')
					{
						if (str.size() < 8 )
							return false;
						for(size_t j = i + 1; j < i + 5; j++)
							if (!is_hexa(str[j]))
								return false;
					}
					if (str[i] == '\n')
						return false;
				}
                if (str[str.length() - 2] == '\\')
                    return false;
            }
			if (str[0] == '\'')
            {
				for (size_t i = 1; i < str.length() - 1; i++)
					if (str[i] == '\n')
						return false;
            }
            return true;
        }
        return false;
    }

    bool    parse::str_is_nbr(type_string str)
    {
        if (str_is_bool(str) || str_is_int(str) || str_is_float(str))
            return true;
        return false;
    }

	bool	parse::str_is_int(type_string str)
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
                	return false;
            if (str[i] == '_')
                count_nbr = 0;
            else if (i == 1 && str[i] == 'x')
                count_nbr = 0;
			else if (is_hexa(str[i]))
                count_nbr++;
        }
        if (!is_hexa(str[str.length() - 1]))
            return false;
        return true;
    }

	bool	parse::str_is_float(type_string str)
    {
        bool    exponent = false;
		if (str[str.length() - 1] < '0' || str[str.length() - 1] > '9')
			return false;
        for (size_t i = 0; i < str.length(); i++)
        {
            if (i == 0 && str[i] != '-' && str[i] != '+' && (str[i] < '0' || str[i] > '9'))
                return false;
            if (i > 0  && (str[i] < '0' || str[i] > '9'))
            {
                if (str[i] == '-' || str[i] == '+')
				{
                    if (str[i - 1] != 'e' && str[i - 1] != 'E')
                        return false;
				}
                else if (str[i] == 'e' || str[i] == 'E' || str[i] == '.')
				{
                    if (exponent || str[i - 1] == '.')
                        return false;
				}
                else
                    return false;
				if (str[i] == 'e' || str[i] == 'E')
                    exponent = true;
            }
        }
        return true;
    }

    bool    parse::str_is_bool(type_string str)
    {
        if (str.compare("true") == 0 || str.compare("false") == 0)
            return true;
        return false;
    }

    bool    parse::str_is_table(type_string str)
    {
        bool    quote_mode = false;
        size_t  str_begin;
        size_t  str_len;
		char	cquote;
		bool	was_space = false;

        for (size_t i = 0; i < str.length(); i++)
        {
			if (quote_mode)
				str_len++;
            if (i == 0 && str[i] == '.')
                return false;
            if ((!valid_key_char(str[i]) && !quote_mode)
				||	(i > 0 && !quote_mode && was_space && valid_key_char(str[i]) && !is_whitespace(str[i]) && str[i] != '.'))
                return false;
			if ((i == 0 && valid_key_char(str[i]) && !is_whitespace(str[i]))
				|| (i > 0 && valid_key_char(str[i]) && !is_whitespace(str[i]) && str[i] != '.'
					&& (is_whitespace(str[i - 1]) || str[i - 1] == '.')))
				was_space = true;
			if (str[i] != '.' && !quote_mode)
				was_space = false;
            if (str[i] == '"' || str[i] == '\'')
            {
                if (i > 0 && !quote_mode && str[i - 1] != '.' && !is_whitespace(str[i - 1]))
					return false;
                if (quote_mode && cquote == str[i])
                {
					if ((i + 1 )< str.length() && str[i + 1] != '.' && !is_whitespace(str[i + 1]))
						return false;
					quote_mode == false;
					if (!str_is_string(str.substr(str_begin, str_len)))
						return false;
                }
                else
                {
                    quote_mode == true;
                    str_begin = i;
					cquote = str[i];
                }
            }
        }
		if (str[str.length() - 1] == '.')
			return false;
        return true;
    }

	bool	parse::only_binary(type_string str)
    {
        for (size_t i = 0; i < str.length(); i++)
            if (str[i] != '0' && str[i] != '1')
                return false;
        return true;
    }

    bool	parse::only_octal(type_string str)
    {
        for (size_t i = 0; i < str.length(); i++)
            if (str[i] < '0' || str[i] > '7')
                return false;
        return true;
    }


    //utiles
	// if the string contain only quote or double quotes return a default string
    // else return what is between
    parse::type_string parse::check_empty_string(type_string str, int len)
    {
        if (str.length() > len)
            str = str.substr(((len / 2)), str.length() - len);
        else
            str = type_string();
        return str;
    }

    // interprete the string given and return the interpretation
    parse::type_string parse::double_quote_change_string(type_string str)
    {
        bool    already_replace = false;
        for (size_t i = 0; i < str.length(); i++)
        {
            if (i > 0 && str[i - 1] == '\\' && !already_replace)
            {
                i--;
                if (is_whitespace(str[i + 1]))
                {
                    str.erase(i, 1);
                    wspace_trimmer(i, str);
                }
                else
                {
                    switch (str[i + 1])
                    {
                        case 'b':
                        str.replace(i, 2, "\b");
                        break;
                        case 't':
                        str.replace(i, 2, "\t");
                        break;
                        case 'n':
                        str.replace(i, 2, "\n");
                        break;
                        case 'f':
                        str.replace(i, 2, "\f");
                        break;
                        case 'r':
                        str.replace(i, 2, "\r");
                        break;
                        case '"':
                        str.replace(i, 2, "\"");
                        break;
                        case '\\':
                        str.replace(i, 2, "\\");
                        already_replace = true;
                        break;
                        case 'u':
                        already_replace = unicode_interpreter(i, str);
                        break;
                    }
                }
            }
            else
                already_replace = false;
        }
        return str;
    }

    bool parse::unicode_interpreter(size_t pos, type_string &str)
    {
        const long code = std::strtol(str.substr(pos + 2, 6).c_str(), NULL, 16);
        if (code <= 0x7F) //code ascii 0 to 127
        {
            str.replace(pos, 6, 1, static_cast<char>(code));
            if (code == 0x5C)
                return true;
        }
        else if (code >= 0x0080 && code <= 0x07FF) // Two point unicode 128 to 2048
        {
            unsigned char unicode[3] = { 0b11000000, 0b10000000, 0x00 };
            unicode[1] |= (code)		& 0b111111;
            unicode[0] |= (code >> 6)	& 0b011111;
            str.replace(pos, 6, reinterpret_cast<const char *>(unicode));
        }
        else // Three point unicode  //2048 to FFFF
        {
            unsigned char unicode[4] = { 0b11100000, 0b10000000, 0b10000000, 0x00 };
            unicode[2] |= (code)		& 0b111111;
            unicode[1] |= (code >> 6)	& 0b111111;
            unicode[0] |= (code >> 12)	& 0b001111;
            str.replace(pos, 6, reinterpret_cast<const char *>(unicode));
        }
        return false;
    }

    void	parse::wspace_trimmer(size_t pos, type_string &str)
    {
        size_t  i = pos;
		size_t	count= 0;
        while(is_whitespace(str[i++]))
            count++;
        str.erase(pos, count);
    }

    float	parse::atof(type_string str)
    {
        //bool
		if (str.compare("true") == 0)
			return 1;
		if (str.compare("false") == 0)
			return 0;
		
		float	nbr;
		//int
		if (str_is_int(str))
		{
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
		}
		//float
		else
		{
            //using dot for knowing where is the comma
            //using expo for knowing where is exponantiel
            size_t  dot = 0;
            size_t  is_expo = 0;
            for (size_t i = 0; i < str.length() ; i++)
            {
                if (str[i] == '.')
                    dot = i;
                if (str[i] == '.' || str[i] == '+')
                {
                    str.erase(i, 1);
                    if (i > 0)
                    {
					    i--;
                        is_expo--;
                    }
                    
                }
                if (str[i] == 'e' || str[i] == 'E')
                    break;
                is_expo++;
            }
            nbr = str_base_to_int(str.substr(0, is_expo), 10);
            //dividing to the original comma (what ?)
            float  ten = 1;
            if (dot > 0)
            {
                dot = (is_expo - dot);
                for (size_t i = 0; i < dot ; i++)
                    ten *= 10;
                nbr /= ten;
                ten = 1;
            }
            //multiplying with the exponential
            if (is_expo < str.length())
            {
				int64_t	exponent;
                exponent = static_cast<int64_t>(str_base_to_int(str.substr(is_expo + 1, str.length() - is_expo), 10));
                for (int64_t i = 0; i < exponent; i++)
                    ten *= 10;
				for (int64_t i = 0; i > exponent; i--)
                    ten /= 10;
                nbr *= ten;
            }
		}
		return nbr;
    }

	float		parse::str_base_to_int(std::string str, size_t base)
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

	float		parse::char_to_int(char c)
	{
		if (c >= '0' && c <= '9')
			return static_cast<float>(c - 48);
		if (c >= 'a' && c <= 'f')
			return static_cast<float>(c - 87);
		if (c >= 'A' && c <= 'F')
			return static_cast<float>(c - 55);
		return 0;
	}

}