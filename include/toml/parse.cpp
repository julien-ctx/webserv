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
                    val = check_empty_string(val, 6);
                else
                    val = check_empty_string(val, 2);
            }
            type_table new_value(key, val);
        }
        else if (str_is_nbr(val))
        {
            TOML::types	t;
            if (val.compare("false") == 0 || val.compare("true") == 0)
                t = T_bool;
            else if (str_is_int(val))
                t = T_int;
            else
                t = T_float;
            type_table new_value(key, TOML::atof(val), t);
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

    bool    parse::is_hexa(char c)
    {
        if ((c >= '0' && c <= '9') || (c >= 'A' && c <= 'F') || (c >= 'a' && c <= 'f'))
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

	bool	parse::valid_antislash(char c)
    {
        if (c == 'b' || c == 't' || c == 'r' || c == 't' || c == '"'  || c == 'u' || c == '\\' || is_whitespace(c))
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
                for (size_t i = 3; i < str.length() - 3; i++)
                    if (i > 3 && str[i - 1] == '\\' && !valid_antislash(str[i]))
                        return false;
                if (str[str.length() - 4] == '\\')
                    return false;
            }
            return true;
        }
        if (str.length() >= 3 && (str[0] == '"' && str[str.length() - 1] == '"'
            || str[0] == '\'' && str[str.length() - 1] == '\''))
        {
            if (str[0] == '"')
            {
                for (size_t i = 1; i < str.length() - 1; i++)
                    if (i > 1 && str[i - 1] == '\\' && (!valid_antislash(str[i]) || is_whitespace(str[i]) || str[i] == '"'))
                        return false;
                if (str[str.length() - 2] == '\\')
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

	bool	str_is_int(type_string str)
    {
        size_t  count_nbr = 0;
        for (size_t i = 0; i < str.lenght(); i++)
        {
            if (i == 0 && str[i] != '-' && str[i] != '+' && (str[i] < '0' || str[i] > '9'))
                return false;
            if (i == 1 && (str[i] < '0' || str[i] > '9')
                && (str[i] != '_' || str[i - 1] < '0' || str[i - 1] > '9')
                && (str[i] != 'x' || str[i - 1] != '0')
                && (str[i] != 'o' || str[i - 1] != '0')
                && (str[i] != 'b' || str[i - 1] != '0'))
                return false;
            if (i > 1 && (str[i] < '0' || str[i] > '9')
                && (str[i] != '_' || str[1] != 'x' || count_nbr != 4)
                && (str[i] != '_' || str[1] == 'o' || str[1] == 'b' || str[1] == 'x' || !count_nbr || count_nbr > 3))
                return false;
            if (i == '_')
                count_nbr = 0;
            else if ((str[i] >= '0' || str[i] <= '9'))
                count_nbr++;
        }
        if ((str[str.lenght() - 1] < '0' || str[str.lenght() - 1] > '9'))
            return false;
        return true;
    }

    bool    parse::str_is_bool(type_string str)
    {
        if (str.compare("true") == 0 || str.compare("false") == 0)
            return true;
        return false;
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

    void parse::wspace_trimmer(size_t pos, type_string &str)
    {
        size_t  i = pos;
        while(is_whitespace(str[i]))
            i++;
        str.erase(pos, i);
    }

}