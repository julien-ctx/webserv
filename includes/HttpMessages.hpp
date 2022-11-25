#pragma once
#include "utils.hpp"

enum HttpMethod {
    GET,    // 0
    POST,   // 1
    DELETE, // 2
};

enum HttpStatusCode {
    OK = 200,
    BadRequest = 400,
    InternalServerError = 500,
};

// Defines the common interface of an HTTP request and HTTP response.
// Each message will have an HTTP version, collection of header fields, and message content.
// The collection of headers and content can be empty.

class HttpMessage {

    public:

HttpMessage() : _version("HTTP/1.1") {}
virtual ~HttpMessage() {}

void SetHeader(const std::string& key, const std::string& value)
{ _headers[key] = value; }

void RemoveHeader(const std::string& key)
{ _headers.erase(key); }

void ClearHeader()
{ _headers.clear(); }

void SetBody(const std::string& body)
{
    _body = body;
    SetBodyLength();
}

void ClearBody()
{
    _body.clear();
    SetBodyLength();
}

std::string version() const
{ return _version; }

std::string GetHeaderByKey(const std::string& key) const
{
    if (_headers.count(key) > 0)
        return _headers.at(key); // at mieux que [] --> std::exeption
    return std::string();
}

std::map<std::string, std::string> GetHeaders() const
{ return _headers; }

std::string GetBody() const
{ return _body; }

size_t GetBodyLenght() const
{ return _body.length(); }

    protected:
std::string                         _version;
std::map<std::string, std::string>  _headers;
std::string                         _body;


void SetBodyLength()
{ SetHeader("Content-Length", std::to_string(_body.length())); }

};
