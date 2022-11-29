#ifndef TYPES_H
# define TYPES_H
# include <string>
# include <iostream>
# include <cstring>
# include <fstream>

namespace TOML
{
    enum types
    {
        T_int,
        T_float,
        T_bool,
        T_string,
        T_array,
        T_table,
        T_date_time,
        T_no_type
    };
}


#endif