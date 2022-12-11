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
        typedef TOML::value						type_table;
        typedef __int64_t						type_int;
        typedef float							type_float;
        typedef bool							type_bool;
        typedef std::string						type_string;
        typedef std::vector<type_table>		type_array;
        typedef std::tm							type_date_time;


		typedef type_table*					pointer;
		typedef type_table&					reference;
		typedef const type_table&			const_reference;


		//iterators
        typedef type_array::iterator				iterator;
        typedef type_array::reverse_iterator		reverse_iterator;

		//variable
		public:
		type_table	_root; //the root of all values
		type_array	_hash_tables; //the array where all values are in it
		type_string	_here; //the table where we are now

		public:
		//constructors / destructors

		parse(): _root("", false), _hash_tables(type_array()), _here(_root._key){}
		parse(type_string config_file): _root("", false), _hash_tables(type_array()), _here(_root._key)
		{
			begin_parse(config_file);
		}
		~parse() {}

		//operators
		TOML::parse	operator=(const TOML::parse &copy)
		{
			this->_hash_tables.clear();
			for (size_t i = 0; i < copy._hash_tables.size(); i++)
			{
				this->_hash_tables.push_back(copy._hash_tables[i]);
			}
			this->_here = copy._here;
			
			return *this;
		}

		//iterators
		public:
		iterator			begin() { return this->_hash_tables.begin();}
		iterator			end() { return this->_hash_tables.end();}
		reverse_iterator	rbegin() { return this->_hash_tables.rbegin();}
		reverse_iterator	rend() { return this->_hash_tables.rend();}
		//  to do private
		public:
		//insertion of new values
		// insertion of int/bool/float
		void	insert(type_string key, type_string val);
		void	insert_array(type_string key, type_string str);
		void	insert_table(type_string key, bool is_array);
		private:
		//parse
		void	begin_parse(type_string config_file);
		void	parse_line(type_string str, size_t line_nbr);
		bool	is_hexa(char c);
		bool	is_lower(char c);
		bool	is_upper(char c);
		bool	is_whitespace(char c);
		bool	valid_key_char(char c);
		bool	valid_antislash(char c);
		bool	str_is_string(type_string str);
		bool	str_is_nbr(type_string str);
		bool	str_is_int(type_string str);
		bool	str_is_float(type_string str);
		bool	str_is_bool(type_string str);
		bool	str_is_date(type_string str);
		bool	str_is_array(type_string str);
		bool	str_is_table(type_string str);
		bool	only_binary(type_string str);
		bool	only_octal(type_string str);

		//utiles
		type_string					table_last_key(type_string str, TOML::types t, bool is_array, size_t line_nbr);
		std::vector<type_string>	str_split(type_string str, type_string ocu);
		type_string					check_empty_string(type_string str, size_t len);
		type_string					double_quote_change_string(type_string str);
		bool						unicode_interpreter(size_t pos, type_string &str);
        void						wspace_trimmer(size_t pos, type_string &str);
		float						atof(type_string str);
		float						str_base_to_int(type_string str, size_t base);
		float						char_to_int(char c);
		void						child_correct_parent(value::type_array to_change, value::type_array changer);
		type_table					new_table(type_string key, bool	has_array);
		public:
		type_string					adding_here(type_string table_key);

		public:
		//searching
		pointer				at_key_parent(type_string key, type_string parent);
		type_array			by_table(type_string parent);
		type_array			by_key(type_string key);


    };



}

#endif