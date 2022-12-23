#pragma once

#include "utils.hpp"
#include "request.hpp"

class CGI
{
private:
	std::string _output;
	std::string _path;
	std::string _type;
	std::string _cgi_dir;
	std::vector<std::string> _env;
public:
	CGI(std::string file, std::string cgi_dir) : _output(""), _path("." + file), _cgi_dir("." + cgi_dir) {}
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

	bool isCGI(Request &request, std::vector<std::string> &ext)
	{
		std::string path = request.GetUri().GetPath();

		if (std::find(ext.begin(), ext.end(), path.substr(path.find_last_of(".") + 1)) != ext.end())
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
		std::string header;
		if (_path.find("upload.py") != string::npos)
			header = std::string("HTTP/1.1 201 Created\n");
		else
			header = std::string("HTTP/1.1 200 OK\n");
		header += ("Content-Length: " + std::to_string(this->_output.size()) + "\nContent-Type: text/html\r\n\n");
		return send(fd, (header + this->_output).c_str(), (header + this->_output).size(), 0);
	}

	void set_error(int status, std::string &content)
	{
		size_t start = 0;
		while ((start = content.find("*ERROR_NO*")) != std::string::npos)
			content.replace(start, 10, std::to_string(status));
	}

	bool sendError(uintptr_t &fd, std::string error_loc)
	{
		std::string 		file_name;
		std::ifstream 		file;
		std::stringstream 	s;
		std::stringstream 	file_n;
		std::stringstream   buffer;
        std::string         content;

		file_name = "." + error_loc;
		file.open(file_name);
		if (!file)
			std::cout << RED << "Cannot respond with " << 408 << std::endl << RESET;
		buffer << file.rdbuf();
		std::string file_content = buffer.str();
		set_error(413, file_content);
		s << "HTTP/1.1 413 Request Entity Too Large" << "\r\n"; 
		s << "Content-Length: " << file_content.size() << "\r\n";
		s << "Content-Type: text/html\r\n\r\n";
        content += s.str();
		content += file_content;
		file.close();
		return send(fd, content.c_str(), content.size(), 0);
	}

	bool execute(uintptr_t &fd, Request &rq, std::string error_loc)
	{
		int r;
		int out[2];
		int in[2];
		if (pipe(out) < 0)
			exit_error("pipe function failed");
		int id = fork();
		if (id < 0)
			exit_error("fork function failed");
		else if (!id)
		{
			if (pipe(in) < 0)
				exit_error("pipe function failed");
			fcntl(in[1], F_SETFL, O_NONBLOCK);
			ssize_t written = write(in[1], (rq.GetBody() + _cgi_dir).c_str(), rq.GetBodyLength() + _cgi_dir.size());
			close(in[1]);
			close(out[0]);
			if (written != (ssize_t)(rq.GetBodyLength() + _cgi_dir.size()))
				exit(1);
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
		waitpid(0, &r, 0);
		char buf[BUFFER_SIZE];
		std::memset(buf, 0, BUFFER_SIZE);
		close(out[1]);
		int ret = 0;
		while ((ret = read(out[0], buf, BUFFER_SIZE)) > 0)
		{
			buf[ret] = 0;
			this->_output += std::string(buf, ret);
		}
		close(out[0]);
		return r == 0 ? sendOutput(fd) : sendError(fd, error_loc);

	}
};
