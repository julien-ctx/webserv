#pragma once

#include "mime.hpp"
#include "utils.hpp"
#include "server.hpp"
#include "Uri.hpp"
#include "request.hpp"

std::string status_to_string(int status)
{
    if (status == 200) // OK
        return "OK";
    else if (status == 201) // OK
        return "Created";
    else if (status == 301) // OK
        return "Moved Permanently";
    else if (status == 400) // Needs to be done
        return "Bad Request";
	else if (status == 403)
		return "Forbidden"; // OK
    else if (status == 404) // OK
        return "Not Found";
    else if (status == 405) // OK
        return "Method Not Allowed";
	else if (status == 413) // OK
        return "Payload Too Large";
    else if (status == 408) // Being handled
        return "Request Timeout";
    else if (status == 501) // OK
        return "Not Implemented";
    else if (status == 505) // Needs to be done
        return "HTTP Version Not Supported"; 
    else 
        return "";
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
	Response(const Request& req) : _method(req.GetMethod()), _version(req.GetVersion()), _uri(req.GetUri()), _headers(req.GetHeaders()), _status(req.GetStatus()), _length(req.GetLength()) {}

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

	int autoindex_listing(struct kevent *ev_list , int i, std::ifstream &file, std::string &path, std::string &root, bool &autoindex, std::string error_loc)
	{
		std::stringstream headers;
		std::string html;
		DIR *dir = opendir(path.c_str());
		if (!dir)
			return send_error(403, ev_list, i, error_loc);

		struct dirent *entry;
		std::ifstream css;
		css.open("." + root + "/style.css");

		html = "<!DOCTYPE html>\n";
		html += "<html lang=\"en\" >\n";
		html += "<head>\n";
		html += "	<meta charset=\"UTF-8\">\n";
		html += "	<title>Webserv | Autoindex</title>\n";
		html += "	<link rel=\"icon\" href=\"images/favicon.ico\" type=\"image/x-icon\" />\n";
		if (css)
		{
			html += "<style>\n";
			std::stringstream css_content;
			css_content << css.rdbuf();
			html += css_content.str();
			html += "</style>\n";
			css.close();
		}
		html += "</head>\n";
		html += "<h1 class=\"smaller-h1\">" + _uri._path + "</h1>\n";
		if (autoindex)
		{
			html += "<nav>";
			html += "<ul>";
			while ((entry = readdir(dir)) != nullptr)
			{
				std::string name(entry->d_name);
				html += "<li><a href='" + _uri._path + "/" + name + "'>" + name + "</a></li>\n";;
			}
			html += "</ul>\n</nav>\n";
		}
		else
			html += "<p>Directory listing is not activated</p>";
		html += "</html>\n";
		headers << "HTTP/1.1 200 OK\r\n";
		headers << "Content-Length: " <<  html.size() << "\r\n";
		headers << "Content-Type: text/html\r\n";
		if (_headers.count("Host") > 0)
			headers << "Server: " + _headers.at("Host") + "\r\n\r\n";
		else
			headers << "\r\n";
		closedir(dir);
		file.close();
		return send(ev_list[i].ident, (headers.str() + html).c_str(), (headers.str() + html).size(), 0);
	}

	int methodGET(struct kevent *ev_list , int &i, std::string error_loc, std::string path, bool autoindex)
	{
		std::stringstream	s;
		std::ifstream 		file;
		std::stringstream buffer;
		std::string file_name;
		struct stat sb;

		_content.clear();
		file_name = "." + path + _uri._path;
		file.open(file_name);

    	if (stat(file_name.c_str(), &sb) == 0 && S_ISDIR(sb.st_mode))
			return autoindex_listing(ev_list, i, file, file_name, path, autoindex, error_loc);
		else if (!file)
			return send_error(404, ev_list, i, error_loc);
		_status = 200;
		buffer << file.rdbuf();
		s << _version << " " << _status  << " " << status_to_string(_status) << "\r\n";
		s << "Content-Length: " << buffer.str().size() << "\r\n";
		s << "Content-Type: " << mime(_uri._path) << "\r\n";
		if (_headers.count("Host") > 0)
			s << "Server: " + _headers.at("Host") + "\r\n\r\n";
		else
			s << "\r\n";
		_content = s.str();
		this->_content += buffer.str();
		file.close();
		return send(ev_list[i].ident, _content.c_str(), _content.size(), 0);
	}

	int methodDELETE(struct kevent *ev_list, int &i, std::string cgi_dir)
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
		content << "Content-Type: " "text/html" << "\r\n";
		if (_headers.count("Host") > 0)
			content << "Server: " + _headers.at("Host") + "\r\n\r\n";
		else
			content << "\r\n";
		content << html;
		return send(ev_list[i].ident, content.str().c_str(), content.str().size(), 0);
	}

	void send_redirection(struct kevent *ev_list, int &i, std::string redir_loc)
	{
		std::string response;
		response = "HTTP/1.1 301 Moved Permanently\r\n";
		response += "Location: " + redir_loc + "\r\n";
		if (_headers.count("Host") > 0)
			response += "Server: " + _headers.at("Host") + "\r\n\r\n";
		else
			response += "\r\n";
		send(ev_list[i].ident, response.c_str(), response.size(), 0);
	}

	int set_cookies(struct kevent *ev_list, int &i, std::string cookie_page, std::string error_loc)
	{
		std::ifstream file;
		std::stringstream s;
		std::stringstream content;
  		file.open("." + cookie_page);
		if (!file)
			return send_error(403, ev_list, i, error_loc);
		content << file.rdbuf();
		s << "HTTP/1.1 200 OK\r\n";
		s << "Content-Length: " <<  content.str().size() << "\r\n";
		s << "Set-Cookie: session_id=marwan; path=/; expires=Fri, 31 Dec 9999 23:59:59 GMT\r\n";
		s << "Content-Type: text/html\r\n";	
		if (_headers.count("Host") > 0)
			s << "Server: " + _headers.at("Host") + "\r\n\r\n";
		else
			s << "\r\n";
		s << content.str();
		file.close();
		return send(ev_list[i].ident, s.str().c_str(), s.str().size(), 0);
	}

	void set_error(int status, std::string &content)
	{
		size_t start = 0;
		while ((start = content.find("*ERROR_NO*")) != std::string::npos)
			content.replace(start, 10, std::to_string(status));
		while ((start = content.find("*ERROR_MSG*")) != std::string::npos)
			content.replace(start, 11, status_to_string(status));
	}

	bool send_error(int status, struct kevent *ev_list, int i, std::string error_loc)
	{
		std::string 		file_name;
		std::ifstream 		file;
		std::stringstream 	s;
		std::stringstream buffer;
		std::string file_content;

		file_name = "." + error_loc;
		if (file_name == "./")
			file_name.clear();
		else
			file.open(file_name);
		if (!file || file_name.empty())
			file_content = "<h1>Error " + std::to_string(status) + "</h1>";
		else
		{
			buffer << file.rdbuf();
			file_content = buffer.str();
			set_error(status, file_content);
			file.close();
		}
		s << _version << " " << status << " " << status_to_string(status) << "\r\n"; 
		s << "Content-Length: " <<  file_content.size() << "\r\n";
		s << "Content-Type: text/html\r\n";
		if (_headers.count("Host") > 0)
			s << "Server: " + _headers.at("Host") + "\r\n\r\n";
		else
			s << "\r\n";
		_content = s.str();
		_content += file_content;
		return send(ev_list[i].ident, _content.c_str(), _content.size(), 0);
	}
};
