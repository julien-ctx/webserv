#pragma once

#include "mime.hpp"
#include "utils.hpp"
#include "server.hpp"
#include "Uri.hpp"
#include "request.hpp"

std::string status_to_string(int status)
{
    if (status == 100)
        return "Continue";
    else if (status == 200)
        return "OK";
    else if (status == 201)
        return "Created";
    else if (status == 202)
        return "Accepted";
    else if (status == 301 )
        return "Moved Permanently";
    else if (status == 302 )
        return "Found";
    else if (status == 304 )
        return "Not Modified";
    else if (status == 400 )
        return "Bad Request";
    else if (status == 401)
         return "Unauthorized";
    else if (status == 404)
        return "Not Found";
    else if (status == 405 )
        return "Method Not Allowed";
    else if (status == 408 )
        return "Request Timeout";
   else if (status == 418)
       return "I'm a teapot";
    else if (status == 500)
        return "Internal Server Error";
    else if (status == 501)
        return "Not Implemented";
    else if (status == 503)
        return "Service Unavailable";
    else if (status == 505)
        return "HTTP Version Not Supported";
    else 
        return "Unknow status code";
}

class Response
{
public:

	int				_method;
	std::string		_version;
	Uri _uri;
	std::map<std::string, std::string>  _headers;
	std::string                         _body;
	int _status;
	size_t	_length;

	std::string _content;

public:
	/* ----- Constructors ----- */
	Response() {}
	Response(const Request& req) : _method(req._method), _version(req._version), _uri(req._uri), _headers(req._headers), _status(req._status), _length(req._length) {}

	~Response() {}
    /* ------------------------ */

	Uri GetUri() const
	{ return _uri; }

	std::string mime_parser()
	{
		std::string s = _headers.at("Accept");
		std::string str;
		for (std::string::iterator it = s.begin(); *it != ',' && it != s.end(); it++)
			str += *it;
		return str;
	}

	std::string getData() const {return this->_content;}
	size_t getDataSize() const {return this->_content.size();}

	int methodGET(struct kevent *ev_list , int i, std::string error_loc, std::string path)
	{
		std::stringstream	s;
		std::ifstream 		file;
		std::stringstream buffer;

		_content.clear();
		s.clear();
		buffer.clear();
		file.open("." + path + _uri._path);
		// DEBUG2("Response GET");
		if (!file)
			return send_error(404, ev_list, i, error_loc);
		_status = 200;
		buffer << file.rdbuf();
		s << _version << " " << _status  << " " << status_to_string(_status) << "\r\n";
		s << "Content-Length: " << buffer.str().size() << "\r\n";
		s << "Content-Type: " << mime(_uri._path) << "\r\n\r\n";
		_content = s.str();
		this->_content += buffer.str();
		file.close();
		return send(ev_list[i].ident, _content.c_str(), _content.size(), 0);
	}

	int methodDELETE(struct kevent *ev_list , int i, std::string cgi_dir)
	{
		std::string file;
		std::stringstream content;
		std::string msg = "Couldn't delete file";

		file =  "." + cgi_dir + GetUri().GetPath();
		struct stat s;
		if (stat(file.c_str(), &s) == 0 && S_ISREG(s.st_mode))
			if (!remove(file.c_str()))
				msg = "File deleted";
		
		std::string html = "<!DOCTYPE html>\
		<html lang=\"en\" >\
		<h3>" + msg + "</h3>\
		</html>";
	
		_status = 200;
		content << _version << " " << _status  << " " << status_to_string(_status) << "\r\n";
		content << "Content-Length: " << html.size() << "\r\n";
		content << "Content-Type: " "text/html" << "\r\n\r\n";
		content << html;
		return send(ev_list[i].ident, content.str().c_str(), content.str().size(), 0);
	}

	void send_redirection(struct kevent *ev_list, int &i, std::string redir_loc)
	{
		std::string response;
		response = "HTTP/1.1 301 Moved Permanently\r\n";
		response += "Location: " + redir_loc + "\r\n\r\n";
		send(ev_list[i].ident, response.c_str(), response.size(), 0);
	}

	void set_cookies(struct kevent *ev_list, int &i, std::string cookie_page)
	{
		std::ifstream file;
		std::stringstream s;
		std::stringstream content;
  		file.open("." + cookie_page);
		if (!file)
			exit_error("cookie page couldn't be opened");
		content << file.rdbuf();
		s << "HTTP/1.1 200 OK\r\n";
		s << "Content-Length: " <<  content.str().size() << "\r\n";
		s << "Set-Cookie: session_id=marwan\r\n";
		s << "Content-Type: text/html\r\n\r\n";	
		s << content.str();
		file.close();
		send(ev_list[i].ident, s.str().c_str(), s.str().size(), 0);
	}

	void set_error(int status, std::string &content)
	{
		size_t start = 0;
		while ((start = content.find("*ERROR_NO*")) != std::string::npos)
			content.replace(start, 10, std::to_string(status));
	}

	bool send_error(int status, struct kevent *ev_list, int i, std::string error_loc)
	{
		std::string 		file_name;
		std::ifstream 		file;
		std::stringstream 	s;
		std::stringstream 	file_n;
		std::stringstream buffer;

		file_name = "." + error_loc;
		file.open(file_name);
		if (!file)
			std::cout << RED << "Cannot respond with " << status << std::endl << RESET;
		buffer << file.rdbuf();
		std::string file_content = buffer.str();
		set_error(status, file_content);
		s << _version << " " << status << " " << status_to_string(status) << "\r\n"; 
		s << "Content-Length: " <<  file_content.size() << "\r\n";
		s << "Content-Type: text/html\r\n\r\n";
		_content = s.str();
		s.clear();
		_content += file_content;
		file.close();
		return send(ev_list[i].ident, _content.c_str(), _content.size(), 0);
	}
};
