#pragma once

#include "utils.hpp"
#include "request.hpp"

class CGI
{
private:
	std::string _output;
	std::string _path;
	std::string _type;
public:
	CGI(std::string file) : _output(""), _path("./www/" + file) {}
	~CGI() {}

	bool isCGI(Request &request)
	{
		std::string path = request.GetUri().GetPath();
		if ((path.substr(path.find_last_of(".") + 1) == "py") ||
			(path.substr(path.find_last_of(".") + 1) == "pl") ||
			(path.substr(path.find_last_of(".") + 1) == "sh"))
		{
			std::ifstream file(this->_path);
			if (file.fail())
				return false;
			this->_type = path.substr(path.find_last_of(".") + 1);
			return true;
		}
		return false;
	}

	bool sendOutput(uintptr_t fd)
	{
		std::string header = std::string("HTTP/1.1 200 OK\n");
		header += ("Content-Length: " + std::to_string(this->_output.size()) + "\nContent-Type: text/html\r\n\n");
		std::cout << (header + this->_output).c_str() << std::endl;
		return send(fd, (header + this->_output).c_str(), (header + this->_output).size(), 0);
	}

	bool execute(uintptr_t fd)
	{
		int fds[2];
		if (pipe(fds) < 0)
			exit_error("pipe function failed");
		int id = fork();
		if (id < 0)
			exit_error("fork function failed");
		else if (!id)
		{
			close(fds[0]);
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
		waitpid(0, NULL, 0);
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
