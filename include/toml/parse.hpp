#ifndef PARSE_H
# define PARSE_H
# include <string>
# include <iostream>
# include <cstring>
# include <fstream>
# include <vector>
# include "value.hpp"

namespace TOML
{
	//this class parse TOML and tidy it
    class parse
    {
        public:

		//Types
        typedef TOML::value					type_table;
        typedef __int64_t					type_int;
        typedef float						type_float;
        typedef bool						type_bool;
        typedef std::string					type_string;
        typedef std::vector<type_table>		type_array;
        typedef std::tm						type_date_time;

		typedef std::vector<type_table>::allocator_type	Allocator;

		typedef type_table*					pointer;
		typedef type_table&					reference;
		typedef const type_table&			const_reference;


		//iterators
        typedef std::vector<type_table>::iterator		iterator;
        typedef std::vector<const type_table>::iterator	const_iterator;

		//variable
		private:
		type_table	_root; //the root of all values
		type_array	_hash_tables; //the array where all values are in it
		pointer		_here; //the value where we are now
		Allocator	_allocator;

		public:
		parse(): _root("", false), _hash_tables(type_array()), _here(&_root), _allocator(Allocator()) {}
		~parse();

		//insertion of new values
		// insertion of int/bool/float
		private:
		void	parse::insert(type_string key, type_string val);
		// insertion of table
		void	insert_table(type_string key);
		//parse
		bool	is_hexa(char c);
		bool	is_lower(char c);
		bool	is_upper(char c);
		bool	is_whitespace(char c);
		bool	valid_antislash(char c);
		bool	str_is_string(type_string str);
		bool	str_is_nbr(type_string str);
		bool	str_is_int(type_string str);
		bool	str_is_float(type_string str);
		bool	str_is_bool(type_string str);
		bool	str_is_date(type_string str);
		bool	str_is_array(type_string str);
		//utiles 
		type_string	check_empty_string(type_string str, int len);
		type_string	double_quote_change_string(type_string str);
		bool 		parse::unicode_interpreter(size_t pos, type_string &str);
        void 		wspace_trimmer(size_t pos, type_string &str);
		float		atof(type_string str);


    }

}

#endif