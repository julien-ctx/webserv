#pragma once

#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/event.h>
#include <unistd.h>
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>

#include "server.hpp"
#define CSS 42
#define HTML 43

void	exit_error(std::string str);

class Response
{
private:
	std::string _html;
	std::string _css;

public:
	/* ----- Constructors ----- */
	Response() {}

	~Response() {}
    /* ------------------------ */

	std::string getHTML() const {return this->_html;}
	size_t getHTMLSize() const {return this->_html.size();}
	std::string getCSS() const {return this->_css;}
	size_t getCSSSize() const {return this->_css.size();}

	int fileSize(std::string const path)
	{
		std::ifstream file;
		file.open(path, std::ios_base::binary);
		file.seekg(0, std::ios_base::end);
		int size = file.tellg();
		file.close();
		return size;
	}

	void setData(std::string const path, int type)
	{
		if (type == CSS)
			this->_css = "HTTP/1.1 200 OK\n";
		else
			this->_html = "HTTP/1.1 200 OK\n";
		if (type == CSS)
			this->_css += "Content-Type: text/css\n\n";
		else
			this->_html += "Content-Type: text/html\n\n";
		std::ifstream file;
		file.open(path);
		if (!file)
			exit_error("opening index failed");
		std::stringstream buffer;
		buffer << file.rdbuf();
		if (type == CSS)
			this->_css += buffer.str();
		else
			this->_html += buffer.str();
		file.close();

	}
};
