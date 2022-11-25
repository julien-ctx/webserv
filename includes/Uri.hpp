#pragma once 

//#include "utils.hpp"


// Define object that can be used to represent a Unique Resource Identifier
// which is useful for parsing and request handling.
// Here we handle only URL (cause no need URN/ISBN)

/*      URL example
 
          userinfo       host      port
          ┌──┴───┐ ┌──────┴──────┐ ┌┴┐
  https://john.doe@www.example.com:123/forum/questions/?tag=networking&order=newest#top
  └─┬─┘   └───────────┬──────────────┘└───────┬───────┘ └───────────┬─────────────┘ └┬┘
  scheme          authority                  path                 query           fragment


We'll only use Scheme / Host / Port / Path      */


class Uri {

    public:

Uri() {}
Uri(std::string path) : _path(path) {}
~Uri() {}

void SetPath(std::string path)
{ _path = path; }

void SetSheme(std::string scheme)
{ _scheme = scheme; }

void SetHost(std::string host)
{ _host = host; }

void SetPort(std::uint16_t port)
{ _port = port; }

std::string GetPath()
{ return _path; }

std::string GetScheme()
{ return _scheme; }

std::string GetHost()
{ return _host; }

std::uint16_t GetPort()
{ return _port; }

    private:

std::string     _path;
std::string     _scheme;
std::string     _host;
std::uint16_t   _port; // size_t , uint16_t --> a voir 

};