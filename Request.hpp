#pragma once

#include "utils.hpp"

// A Request object represents a single HTTP request
// It have method n URI(url) so that the server can identify
// the corresponding resource and action

class Request : public HttpMessage {

    public:

Request() : _method(HttpMethod::GET) {}
~Request() = default;

void SetMethod(HttpMethod method)
{ _method = method; }

void SetUri(const Uri& uri)
{ _uri = std::move(uri); }

HttpMethod method() const
{ return _method; }

Uri uri() const
{ return _uri; }

friend std::string to_string(const Request& request);

// transforme une string en une requete
Request string_to_request(const std::string& request_string)
{
    std::string         start_line, header_lines, message_body;
    std::istringstream  iss;                          // voir https://www.youtube.com/watch?v=KUx9YfHkllk pour plus d'explications
    Request             request;                      // return value
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
            message_body = request_string.substr(lpos, rpos - lpos);
    }
    iss.clear();  // parse the start line
    iss.str(start_line);
    iss >> method >> path >> version;   // >> = ' ' donc : GET /info.html HTTP/1.1 --> GET>>/info.html>>HTTP/1.1
    if (!iss.good() && !iss.eof())
        throw std::invalid_argument("Invalid header format");
    request.SetMethod(string_to_method(method));
    request.SetUri(Uri(path));
    if (string_to_version(version) != request.version())
        throw std::logic_error("wrong HTTP version");






}

    private:

HttpMethod  _method;
Uri         _uri;

}