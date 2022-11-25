#pragma once

#include <string>
#include <map> 
#include <iostream>

enum HTTP_STATUS
{
    STATUS_200,
    STATUS_401,
    STATUS_404,
    STATUS_500
};

class Response {
  
public:

Response() {}
~Response() {}

void setStatus(HTTP_STATUS s)
{
    _status = s;
    if (_status != STATUS_200)
    setBody(getErrorBody(_status));
}

void setBody(const std::string& b)
{
    _body = b;
    if (!_body.empty())
        _headers["Content-Length"] = std::to_string(_body.size());
    else
        _headers.erase("Content-Length");
}

void addHeader(const std::string& name, const std::string& value) { _headers[name] = value; }

const std::map<std::string, std::string>& getHeaders() const { return _headers; }

HTTP_STATUS getStatus() const { return _status; }

const std::string& getBody() const { return _body; }

std::ostream& printResponse(std::ostream& os) const
{
  os << "HTTP/1.1 " << getStatusString(_status) << "\r\n";

  for (std::map<std::string, std::string>::const_iterator it = _headers.begin(); it != _headers.end(); it++)
    os << it->first << ": " << it->second << "\r\n";
  os << "\r\n";

  if (!_body.empty()) {
    os << _body;
  }

  return os;
}

private:

    HTTP_STATUS                        _status;
    std::map<std::string, std::string> _headers;
    std::string                        _body;
  
};

std::ostream& operator<<(std::ostream& os, const Response& resp)
    { return resp.printResponse(os); }

const char* getErrorBody(HTTP_STATUS status)
{
    switch (status)
    {
        case STATUS_401: return "Authorization required\n";
        case STATUS_404: return "Url not found\n";
        case STATUS_500: return "Server error\n";
    }
    return ("");
}

const char* getStatusString(HTTP_STATUS status)
{
    switch (status)
    {
        case STATUS_200: return "200 OK";
        case STATUS_401: return "401 Unauthorized";
        case STATUS_404: return "404 Not Found";
        case STATUS_500: return "500 Internal Server Error";
    }
    return "";
}