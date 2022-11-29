#pragma once

#include "Uri.hpp"
#include "utils.hpp"





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

int string_to_method(const std::string& method_string) {
if (method_string == "GET")
    return 0;
else if (method_string == "POST")
    return 1;
else if (method_string == "DELETE")
    return 2;
else
    throw std::invalid_argument("Unexpected HTTP method");
}





// A Request object represents a single HTTP request
// It have method n URI(url) so that the server can identify
// the corresponding resource and action


class Request
{

    public:

Request() : _version("HTTP/1.1")  {}//: _method(0) {}  // vide par default? --> a voir | 0 pour GET
~Request() {}

void SetMethod(int method)
{ _method = method; }

void SetUri(const Uri& uri)
{ _uri = std::move(uri); }

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

Uri GetUri() const
{ return _uri; }

std::string GetVersion() const
{ return _version; }

std::string GetBody() const
{ return _body; }

std::map<std::string, std::string> GetHeaders() const
{ return _headers; }

size_t GetBodyLenght() const
{ return _body.length(); }


friend std::string to_string(const Request& request); // --> a coder

// transforme une string en une requete
void string_to_request(const std::string& request_string)
{
    std::string         start_line, header_lines, message_body;
    std::istringstream  iss;                          // voir https://www.youtube.com/watch?v=KUx9YfHkllk pour plus d'explications
    std::string         line, method, path, version;  // for first line
    std::string         key, value;                   // for header
    Uri                 uri;
    size_t              lpos = 0, rpos = 0;

    rpos = request_string.find("\r\n", lpos);
    if (rpos == std::string::npos) // npos --> means "until the end of the string"
        throw std::invalid_argument("Could not find request start line"); // aucun \r\n --> sus

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
        throw std::invalid_argument("Invalid header format");
    SetMethod(string_to_method(method));
    if (path == "/")
        path += "index.html";
    SetUri(Uri(path));
    if (version.compare(GetVersion()) != 0)
        throw std::logic_error("wrong HTTP version");
    
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
    SetBody(message_body);
}


// private:

std::map<std::string, std::string>  _headers;
std::string                         _version;
std::string                         _body;

int _method;
Uri _uri;


};
