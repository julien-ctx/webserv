#include "../includes/utils.hpp"

void	exit_error(std::string str, int error_no)
{
	std::cout << RED << "Error: " + str << std::endl << RESET;
	exit(error_no);
}
