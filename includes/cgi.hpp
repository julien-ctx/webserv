#pragma once

#include "utils.hpp"
#include "request.hpp"

class CGI
{
private:
	std::string _output;
	std::string _path;
	std::string _type;
	std::vector<std::string> _env;
public:
	CGI(std::string file) : _output(""), _path("./www/" + file) {}
	~CGI() {}

	char **getEnv()
	{
		char **cmd = new char*[this->_env.size() + 1];
		int i;
		for (i = 0; i < static_cast<int>(this->_env.size()); i++)
			cmd[i] = strdup(this->_env[i].c_str());
		cmd[i] = NULL;
		return cmd;
	}

	bool isCGI(Request &request)
	{
		std::string path = request.GetUri().GetPath();
		if ((path.substr(path.find_last_of(".") + 1) == "py") ||
			(path.substr(path.find_last_of(".") + 1) == "pl") ||
			(path.substr(path.find_last_of(".") + 1) == "php") ||
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

	bool sendOutput(uintptr_t &fd)
	{
		std::string header = std::string("HTTP/1.1 200 OK\n");
		header += ("Content-Length: " + std::to_string(this->_output.size()) + "\nContent-Type: text/html\r\n\n");
		return send(fd, (header + this->_output).c_str(), (header + this->_output).size(), 0);
	}

	bool execute(uintptr_t &fd, Request &rq)
	{
		int out[2];
		int in[2];
		if (pipe(out) < 0)
			exit_error("pipe function failed");
		int id = fork();
		if (id < 0)
			exit_error("fork function failed");
		else if (!id)
		{
			pipe(in);
			write(in[1], rq.GetBody().c_str(), rq.GetBodyLength());
			close(in[1]);

			close(out[0]);
			dup2(in[0], STDIN_FILENO);
			close(in[0]);
			dup2(out[1], STDOUT_FILENO);
			close(out[1]);

			char *cmd[3];
			std::string exec;
			if (this->_type == "py")
				exec = "/usr/local/bin/python3";
			else if (this->_type == "pl")
				exec = "/usr/bin/perl";
			else if (this->_type == "php")
				exec = "/opt/homebrew/bin/php";
			else if (this->_type == "sh")
				exec = "/bin/bash";
			cmd[0] = const_cast<char *>(exec.c_str());
			cmd[1] = const_cast<char *>(this->_path.c_str());
			cmd[2] = NULL;

			for (int i = 0; environ[i]; i++)
				_env.push_back(std::string(environ[i]));
			if (rq.GetHeaders().count("Content-Type"))
				_env.push_back("CONTENT_TYPE=" + rq.GetHeaders().at("Content-Type"));
			if (rq.GetHeaders().count("Content-Length"))
				_env.push_back("CONTENT_LENGTH=" + rq.GetHeaders().at("Content-Length"));
			_env.push_back("QUERY_STRING=");
			_env.push_back("SCRIPT_NAME=" + rq.GetUri().GetPath());
			
			switch (rq.GetMethod())
			{
				case GET:
					_env.push_back("REQUEST_METHOD=GET");
					break;
				case POST:
					_env.push_back("REQUEST_METHOD=POST");
					break;
				case DELETE:
					_env.push_back("REQUEST_METHOD=DELETE");
					break;
			}

			if (execve(cmd[0], cmd, getEnv()) < 0)
				exit_error("Invalid CGI program");
		}
		waitpid(0, NULL, 0);
		char buf[2];
		std::memset(buf, 0, 1);
		close(out[1]);
		while (read(out[0], buf, 1) > 0)
		{
			buf[1] = 0;
			this->_output += std::string(buf);
		}
		close(out[0]);
		return sendOutput(fd);
	}
};
