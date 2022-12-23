#pragma once

#include <stdlib.h>
#include <ctype.h>
#include "Uri.hpp"
#include "utils.hpp"
#include <string.h>

std::string ltrim(const std::string &s) {
    size_t start = s.find_first_not_of(" \n\r\t\f\v"); // trouve le premier char qui n'est pas dans la string
    return (start == std::string::npos) ? "" : s.substr(start);
}

std::string rtrim(const std::string &s) {
    size_t end = s.find_last_not_of(" \n\r\t\f\v"); // trouve dernier char qui n'est pas contenue dans cette string;
    return (end == std::string::npos) ? "" : s.substr(0, end + 1);
}

std::string trim(const std::string & s) {
	return rtrim(ltrim(s)); // del whitespace 
}

static char	**ft_malloc_error(char **tab)
{
	unsigned int	i;

	i = 0;
	while (tab[i])
	{
		free(tab[i]);
		i++;
	}
	free(tab);
	return (NULL);
}

static int	ft_count_line_tab(char const *s, char c)
{
	int	i;
	int	secu;
	int	count;

	i = 0;
	secu = 0;
	count = 0;
	while ((size_t)i < strlen(s))
	{
		if (s[i] == c)
			secu = 0;
		else if (!secu)
		{
			secu = 1;
			count++;
		}
		i++;
	}
	return (count);
}

static void	ft_get_next_str(char **next_str, unsigned int *next_str_len, char c)
{
	unsigned int	i;

	*next_str += *next_str_len;
	*next_str_len = 0;
	i = 0;
	while (**next_str && **next_str == c)
		(*next_str)++;
	while ((*next_str)[i])
	{
		if ((*next_str)[i] == c)
			return ;
		(*next_str_len)++;
		i++;
	}
}

char	**split(char const *s, char c)
{
	char			**tab;
	char			*next_str;
	unsigned int	n_slen;
	unsigned int	count;
	unsigned int	i;

	if (!s)
		return (NULL);
	n_slen = 0;
	i = 0;
	count = ft_count_line_tab(s, c);
	tab = (char **)malloc(sizeof(char *) * (count + 1));
	if (!(tab))
		return (NULL);
	next_str = (char *)s;
	while (i < count)
	{
		ft_get_next_str(&next_str, &n_slen, c);
		tab[i] = (char *)malloc(sizeof(char) * (n_slen + 1));
		if (!(tab[i]))
			return (ft_malloc_error(tab));
		strlcpy(tab[i++], next_str, n_slen + 1);
	}
	tab[i] = NULL;
	return (tab);
}



std::string DelWhiteSpace(std::string str)
{
    for (size_t i = 0; i < str.length(); i++)
    {
        if ( std::isspace(str[i]) != 0)
        {
            str.erase(i, 1);
            i -= 1;
        }
    }
    return str;
}

std::string DelTab(std::string str)
{
    for (size_t i = 0; i < str.length(); i++)
    {
        if (str[i] == '\r')
        {
            str.erase(i, 1);
            i -= 1;
        }
    }
    return str;
}

int string_to_method(const std::string& method_string) {
if (method_string == "GET")
    return 0;
else if (method_string == "POST")
    return 1;
else if (method_string == "DELETE")
    return 2;
else
    return 3;
}

// A Request object represents a single HTTP request
// It have method n URI(url) so that the server can identify
// the corresponding resource and action

class Request
{

private :

    std::map<std::string, std::string>  _headers;
    std::string                         _version;
    std::string                         _body;

    int     _status;
    int     _method;
    Uri     _uri;
    size_t  _length;

    std::string         _index;
    std::string         _root;
    std::string         _route;
    std::vector<int>    _methods;
    std::string         _error_page;
    std::string         _status_route;
    std::string         _status_root;

public:

    Request() : _version("HTTP/1.1")  {}//: _method(0) {}  // vide par default? --> a voir | 0 pour GET
    Request(std::string _index, std::string _root, std::string _route, std::vector<int> _methods, std::string _error_page, std::string _status_route, std::string _status_root) :
                        _version("HTTP/1.1"), _index(_index), _root(_root), _route(_route), _methods(_methods), _error_page(_error_page), _status_route(_status_route), _status_root(_status_root) {}
    ~Request() {}

    void SetMethod(int method)
    { _method = method; }

    void SetUri(const Uri& uri)
    { _uri = uri; }

    void SetStatus(int s)
    { _status = s; }

    void SetBody(const std::string& body)
    {
        _body = body;
        SetBodyLength();
    }

    void SetBodyLength()
    { SetHeader("Content-Length", std::to_string(_body.length())); }

    void SetHeader(const std::string& key, const std::string& value)
    { _headers[key] = value; }

    void RemoveHeader(const std::string& key)
    { _headers.erase(key); }

    void ClearHeader()
    { _headers.clear(); }

    int GetMethod() const
    { return _method; }

    int GetStatus() const
    { return _status; }

    Uri GetUri() const
    { return _uri; }

    std::string GetVersion() const
    { return _version; }

    std::string GetBody() const
    { return _body; }

    size_t GetLength() const
    { return _length; }

    std::map<std::string, std::string> GetHeaders() const
    { return _headers; }

    size_t GetBodyLength() const
    { return _body.length(); }

    // transforme une string en une request
    void string_to_request(const std::string request_string)
    {
        std::string         start_line, header_lines, message_body;
        std::istringstream  iss;                          // voir https://www.youtube.com/watch?v=KUx9YfHkllk pour plus d'explications
        std::string         line, method, path, version;  // for first line
        std::string         key, value;                   // for header
        Uri                 uri;
        size_t              lpos = 0, rpos = 0;
        std::map<std::string, std::string>::iterator it;

        rpos = request_string.find("\r\n", lpos);
        if (rpos == std::string::npos) // npos --> means "until the end of the string"
        {
            _status = 400;
            return;
        }
        
        start_line = request_string.substr(0, rpos);  // si bug essayer substr(lpos, rpos - lpos) mais lpos = 0 ici
        lpos = rpos + 2; // +1 pour \r +1 pour \n --> +2
        rpos = request_string.find("\r\n\r\n", lpos);   // --> r\n\r\n = debut du body / fin header
        if (rpos != std::string::npos) // on a trouve l'header
        {
            header_lines = request_string.substr(lpos, rpos - lpos);
            lpos = rpos + 4;  // +4 pour les \r\n\r\n
            rpos = request_string.length();
            if (lpos < rpos) // si il y a quelque chose apres \r\n\r\n --> c'est le body
                message_body = request_string.substr(lpos, rpos - lpos); // no need more path --> 
        }
        
        iss.clear();  // parse the start line
        iss.str(start_line);
        iss >> method >> path >> version;   // >> = ' ' donc : GET /info.html HTTP/1.1 --> GET>>/info.html>>HTTP/1.1
        if (!iss.good() && !iss.eof())
            _status = 400;
        SetMethod(string_to_method(method));
        if (path == "/")
            path += _index;
        SetUri(Uri(path));
        if (version.compare(GetVersion()) != 0 && _method == 0)
            _status = 505;
        iss.clear();  // parse header fields
        iss.str(header_lines);
        while (std::getline(iss, line)) // --> cet overload de getline = gnl en gros
        {
            std::istringstream header_stream(line); // 
            std::getline(header_stream, key, ':'); // getline until ':'
            std::getline(header_stream, value);
            key = DelWhiteSpace(key);
            value = DelWhiteSpace(value);
            SetHeader(key, value);
        }
        _length = _headers.find("Content-Length") == _headers.end() ? 0 : atoi(_headers.find("Content-Length")->second.c_str());
        if (_headers.find("Transfer-Encoding") != _headers.end())
        {
            bool chunk = false;
            char **array = split(it->second.c_str(), ',');
            for (int i = 0; array[i] != NULL; i++)
            {
                if (strstr((const char *)(array + i), "chunked") != NULL)
                    chunk = true;
                free(array + i);
            }
            free(array);
            if (it == _headers.end() && chunk == false)
            {
                _status = 411;
                return ;
            }
            if (chunk == true)
            {
                errno = 0;
                long data_size = strtol(message_body.c_str(), NULL, 16);
                if (errno != 0)
                {
                    _status = 406;
                    return ;   
                }
                std::stringstream body_msg;
                std::string line, body;
                long chunk_size, current_size = 0;
                while (std::getline(body_msg, line))
                {
                    chunk_size = line.length() + 1;
                    if (chunk_size == 0)
                        break;
                    if (current_size <= data_size)
                    {
                        current_size += chunk_size = 1;
                        line = trim(line);
                        line += '\n';
                        body += line;
                    }
                    if (current_size == chunk_size && std::getline(body_msg, line))
                    {
                        chunk_size = strtol(line.c_str(), NULL, 16);
                        current_size = 0;
                    }
                }
                SetBody(body);
            }
        }
        else
            SetBody(message_body);
        _status = 0;
        return;
    }
};
