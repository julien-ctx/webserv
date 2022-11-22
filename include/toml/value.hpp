#ifndef VALUE_H
# define VALUE_H
# include <string>
# include <iostream>
# include <cstring>
# include <fstream>
# include <array>
# include <ctime>
# include "types.hpp"
# include "exceptions.hpp"

namespace   TOML
{
    class value
    {
        //type of variable
        public:
        typedef TOML::value				 type_table;
        typedef __int64_t				 type_int;
        typedef float					 type_float;
        typedef bool					 type_bool;
        typedef std::string				 type_string;
        typedef std::vector<TOML::value> type_array;
        typedef std::tm					 type_date_time;

		
		//variable
		TOML::types		_typing;
		type_string		_key;
		type_table		*_parent;
		bool			_is_array_table;


		type_int		_int;
		type_float		_float;
		type_bool		_bool;
		type_string		_string;
		type_array		_array;
		type_date_time	_date_time;

		public:
		//constructor & destructor
		// construct from a float/int/bool
		value(type_string key, float nbr, TOML::types t) : _typing(t), _key(key)
		{
			if (t == T_int)
				_int = static_cast<type_int> (nbr);
			else if (t == T_bool)
				_bool = static_cast<type_bool> (nbr);
			else if (t == T_float)
				_float = nbr;
			else
				throw TOML::TypeUndefined("Trying to initiated a logic type with a wrong type");
		}
		// construct from a string
		value(type_string key, type_string str) : _typing(T_string), _key(key), _string(str) {}
		// construct from an array
		value(type_string key, type_array ar) : _typing(T_array), _key(key), _array(ar) {}
		// construct from a date_time
		value(type_string key, type_date_time dt) : _typing(T_date_time), _key(key), _date_time(dt) {}
		// construct from a table
		value(type_string key, bool is_array) : _typing(T_table), _key(key), _is_array_table(is_array), _array() {}

		~value() {}

    }
}

#endif