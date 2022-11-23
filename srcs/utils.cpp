#include "../includes/utils.hpp"

void	exit_error(std::string str)
{
	std::cout << RED << "Error: " + str << std::endl << RESET;
	exit(1);
}
