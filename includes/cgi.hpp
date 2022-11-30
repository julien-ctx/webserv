#pragma once

#include "utils.hpp"
#include "request.hpp"

class CGI
{
private:
	std::string _output;
	std::string _path;
	std::string _type;
	std::stringstream _html;
	std::string _name;
public:
	CGI(std::string file) : _output(""), _path("./www/" + file), _html(""), _name("") {}
	~CGI() {}

	bool isCGI(Request &request)
	{
		std::string path = request.GetUri().GetPath();
		if ((path.substr(path.find_last_of(".") + 1) == "py") ||
			(path.substr(path.find_last_of(".") + 1) == "pl") ||
			(path.substr(path.find_last_of(".") + 1) == "sh"))
		{
			this->_type = path.substr(path.find_last_of(".") + 1);
			return true;
		}
		return false;
	}

	bool sendOutput(uintptr_t fd)
	{
		this->_name = this->_path.substr(this->_path.find_last_of("/") + 1);
		std::ifstream file;
		file.open("./www/snippets/header.html");
		// if (file.fail())
		// need to check errors and unexisting file
		// need to check environment variables too
		this->_html << file.rdbuf();
		file.close();
		file.clear();
		this->_html << ("<title>" + this->_name + "</title>");
		file.open("./www/snippets/links.html");
		this->_html << file.rdbuf();
		file.close();
		file.clear();
		this->_html << "<body><h1>" + this->_output + "</h1>";
		file.open("./www/snippets/footer.html");
		this->_html << file.rdbuf();
		file.close();
		file.clear();
		std::string header = std::string("HTTP/1.1 200 OK\nContent-Length: ") + std::to_string(this->_html.str().size()) + "\n Content-Type: text/html\r\n";
		send(fd, (header + this->_html.str()).c_str(), (header + this->_html.str()).size(), 0);
		return true;
	}

	bool execute(uintptr_t fd)
	{
		(void)fd;
		int fds[2];
		if (pipe(fds) < 0)
			exit_error("pipe function failed");
		int id = fork();
		if (id < 0)
			exit_error("fork function failed");
		else if (!id)
		{
			dup2(fds[1], STDOUT_FILENO);
			close(fds[1]);
			char *cmd[3];
			std::string exec;
			if (this->_type == "py")
				exec = "/usr/local/bin/python3";
			else if (this->_type == "pl")
				exec = "/usr/bin/perl";
			else if (this->_type == "sh")
				exec = "/bin/bash";
			cmd[0] = const_cast<char *>(exec.c_str());
			cmd[1] = const_cast<char *>(this->_path.c_str());
			cmd[2] = NULL;
			if (execve(cmd[0], cmd, environ) < 0)
				exit_error("Invalid CGI program");
		}
		char buf[2];
		std::memset(buf, 0, 1);
		close(fds[1]);
		while (read(fds[0], buf, 1) > 0)
		{
			buf[1] = 0;
			this->_output += std::string(buf);
		}
		close(fds[0]);
		return sendOutput(fd);
	}
};
