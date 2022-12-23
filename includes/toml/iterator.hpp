#ifndef ITERATOR_H
# define ITERATOR_H
# include <string>
# include <iostream>
# include <cstring>
# include <fstream>
# include "value.hpp"

namespace TOML
{
    class iterator
    {
        
        private :
        TOML::value *_here;

        public:
        iterator(void);
        iterator(void);
        ~iterator(void);
            
        iterator(const iterator &copy);

        iterator& operator=(iterator &copy);
        iterator& operator=(const iterator &copy);
    };
}
#endif