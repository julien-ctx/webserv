#pragma once

#include "utils.hpp"

class CGI
{
private:
	std::string _output;
	std::string _html;
	std::string _file;
	
public:
	/* ----- Constructors ----- */
	CGI(std::string file) : _output(""), _file(file), _html("") {}
	~CGI() {}
    /* ------------------------ */
// https://stackoverflow.com/questions/478898/how-do-i-execute-a-command-and-get-the-output-of-the-command-within-c-using-po
	std::string getOutput()
	{
		
	}
};
