#pragma once

#include "utils.hpp"
#include "server.hpp"
#include "client.hpp"

#define CSS 42
#define HTML 43

void	exit_error(std::string str);

class Response
{
private:
	std::string _content;

public:
	/* ----- Constructors ----- */
	Response() {}

	~Response() {}
    /* ------------------------ */

	std::string getData() const {return this->_content;}
	size_t getDataSize() const {return this->_content.size();}

	int fileSize(std::string const path)
	{
		std::ifstream file;
		file.open(path, std::ios_base::binary);
		file.seekg(0, std::ios_base::end);
		int size = file.tellg();
		file.close();
		return size;
	}

	std::string getIndex(std::string const path)
    {
		this->_content += "HTTP/1.1 200 OK\n";
		this->_content += "Content-Type: text/html\n\n";
		std::ifstream file;
		file.open(path);
		if (!file)
			exit_error("opening index failed");
		std::stringstream buffer;
		buffer << file.rdbuf();
		this->_content += buffer.str();
		file.close();
		return this->_content;
    }

	std::string getCSS(std::string const path)
    {
		this->_content = "";
		this->_content += "HTTP/1.1 200 OK\n";
		this->_content += "Content-Type: text/css\n\n";
		std::ifstream file;
		file.open(path);
		if (!file)
			exit_error("opening index failed");
		std::stringstream buffer;
		buffer << file.rdbuf();
		this->_content += buffer.str();
		file.close();
		return this->_content;
    }

	std::string getFav(std::string const path)
    {
		this->_content = "";
		this->_content += "HTTP/1.1 200 OK\n";
		this->_content += "Content-Type: image/avif\n\n";
		std::ifstream file;
		file.open(path);
		if (!file)
			exit_error("opening index failed");
		std::stringstream buffer;
		buffer << file.rdbuf();
		this->_content += buffer.str();
		file.close();
		return this->_content;
    }
};
