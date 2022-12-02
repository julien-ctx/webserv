#pragma once

#include "mime.hpp"
#include "utils.hpp"
#include "server.hpp"
#include "Uri.hpp"
#include "request.hpp"

long GetFileSize( std::ifstream & Fichier ) 
{ 
    // sauvegarder la position courante 
    long pos = Fichier.tellg(); 
    // se placer en fin de fichier 
    Fichier.seekg( 0 , std::ios_base::end ); 
    // récupérer la nouvelle position = la taille du fichier 
    long size = Fichier.tellg() ; 
    // restaurer la position initiale du fichier 
    Fichier.seekg( pos,  std::ios_base::beg ) ;
	Fichier.close(); 
    return size ; 
}

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



void	exit_error(std::string str);

class Response
{
//private:
public:

int				_method;
std::string		_version;
Uri _uri;
std::map<std::string, std::string>  _headers;
std::string                         _body;
int _status;

std::string _content;

public:
	/* ----- Constructors ----- */
	Response() {}
	Response(const Request& req) : _method(req._method), _version(req._version), _uri(req._uri), _headers(req._headers) {}

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

	int fileSize(std::string const path)
	{
		std::ifstream file;
		file.open(path, std::ios_base::binary);
		file.seekg(0, std::ios_base::end);
		int size = file.tellg();
		file.close();
		return size;
	}

	int methodGET(struct kevent *ev_list , int i)
	{
		std::stringstream	s;
		std::ifstream 		file;
		std::stringstream buffer;

		_content.clear();
		s.clear();
		buffer.clear();
		file.open("./www" + _uri._path);
		if (!file)
		{
			_status = 404;
			return send_404(ev_list, i);
		}
		_status = 200;
		buffer << file.rdbuf();
		s << _version << " " << _status  << " " << status_to_string(_status) << "\r\n";
		s << "Content-Length: " << GetFileSize(file) << "\r\n";
		s << "Content-Type: " << mime(_uri._path) << "\r\n\r\n"; // utiliser MINME ici 
		_content = s.str();
		this->_content += buffer.str();
		file.close();
		return send(ev_list[i].ident, _content.c_str(), _content.size(), 0);
	}

	bool send_404(struct kevent *ev_list , int i)
	{
		std::ifstream 		file;
		std::stringstream 	s;
		std::stringstream buffer;

		file.open("./www/404.html");
		if (!file)
			std::cout << RED << "Cannot respond with 404\n" << RESET;
		buffer << file.rdbuf();
		s << _version << " " << _status << " " << status_to_string(_status) << "\r\n"; 
		s << "Content-Length: " <<  GetFileSize(file) << "\r\n";
		s << "Content-Type: text/html\r\n\r\n";
		_content = s.str();
		s.clear();
		_content += buffer.str();
		file.close();
		return send(ev_list[i].ident, _content.c_str(), _content.size(), 0);
	}

};
