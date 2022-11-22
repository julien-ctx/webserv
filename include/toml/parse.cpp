#include "parse.hpp"

namespace TOML
{
    //insertion of new values
    // interprete and change the string val, so it can be include in the _hash_tables
    void	parse::insert(type_string key, type_string val)
    {
        if (is_string(val))
        {
            if (val[0] == '"')
            {
                if (!val.substr(0, 3).compare("\"\"\"") )
                    val = double_quote_change_string(check_empty_string(val, 6));
                else
                    val = double_quote_change_string(check_empty_string(val, 2));
            }
            else
                if (!val.substr(0, 3).compare("'''") )
                    val = check_empty_string(val, 6);
                else
                    val = check_empty_string(val, 2);

            type_table new_value(key, val);
        }
        else if (is_nbr(val))
        {
            TOML::types	t;
            if (is_bool(val))
                t = T_bool;
            else if (is_int(val))
                t = T_int;
            else
                t = T_float;
            type_table new_value(key, ft::atof(val), t);
        }
        else if (is_array(val))
        {
            
        }
    }

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
    parse::type_string parse::double_quote_change_string(type_string &str)
    {
        bool    already_replace = false;
        for (size_t i = 0; i < str.length(); i++)
        {
            if (i > 0 && str[i - 1] == '\\' && !already_replace)
            {

                if (is_whitespace(str[i]))
                    wspace_trimmer(i, str);
                else
                    switch (str[i])
                    {
                        case 'b':
                        str.replace(i - 1, 2, "\b");
                        break;
                        case 't':
                        str.replace(i - 1, 2, "\t");
                        break;
                        case 'n':
                        str.replace(i - 1, 2, "\n");
                        break;
                        case 'f':
                        str.replace(i - 1, 2, "\f");
                        break;
                        case 'r':
                        str.replace(i - 1, 2, "\r");
                        break;
                        case '"':
                        str.replace(i - 1, 2, "\"");
                        break;
                        case '\\':
                        str.replace(i - 1, 2, "\\");
                        already_replace = true;
                        break;
                        case 'u':
                        already_replace = unicode_interpreter(i - 1, str);
                        break;
                    }
            }
            else
                already_replace = false;
        }
        
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

}