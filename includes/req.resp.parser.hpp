#pragma once

#include <iostream>
#include <map>
#include <string>
#include <algorithm>

#include "request.hpp"
#include "respons.hpp"

//Blocking Parser
class Parser {

    public:

Parser() {}
~Parser() {}

void receiveRequest(std::istream& stream, Request& request)
{
	readWord(stream, request._method);
	readWord(stream, request._url);

	std::string httpVersion;
	readWord(stream, httpVersion);

	readHeaders(stream, request._headers);

	std::string body;
	size_t bodyLen = getBodyLen(request._headers);
	if (bodyLen) {
		readBody(stream, request._body, bodyLen);
	}
}



void receiveResponse(std::istream& stream, Response& response)
{
	std::string httpVersion, status;
	char c;
  
	readWord(stream, httpVersion);
	stream.get(c);
	while (stream.good() && c != '\r')
	{ //Till the end
    	status += c;
    	stream.get(c);
  	}
	throwIfNotGood(stream);
  	if (c == '\r') 
  	{
    	stream.get(c);
    	if (c != '\n')
      		throw std::runtime_error("Parser error: '\\n' symbol is expected");
	}
}

HTTP_STATUS parseResponseStatusFromString(const std::string& status)
 {
  if (status == "200 OK" || status == "200 Ok") 
  	return STATUS_200;
  else if (status.substr(0, 4) == "401 ") 
  	return STATUS_401;
  else if (status == "404 Not Found") 
  	return STATUS_404;
  else if (status == "500 Internal Server Error") 
  	return STATUS_500;

  return STATUS_200; //unaccessible
}

  public:

void readWord(std::istream& stream, std::string& word)
{
  char c;
  stream.get(c);
  while (stream.good() && c != ' ' && c != '\r')
  {
    word += c;
    stream.get(c);
  }
}

void readHeaders(std::istream& stream, std::map<std::string, std::string>&headers)
{
	std::string name;
	std::string value;
	
	while (readHeader(stream, name, value))
	{
		headers[name] = value; //use insert
		name.clear();
		value.clear();
	}
  	headers[name] = value; //use insert
}

bool readHeader(std::istream& stream, std::string& name, std::string& value)
{
	char c;
	bool isName = true;

	stream.get(c);
	while (stream.good() && c != '\r') 
	{
		if (c == ':')				// apres les :
		{
			if (stream.peek() == ' ') // peek -> c + 1
				stream.get(c);
			if (name.empty())
				std::cerr << "No Header" << std::endl;
			if (isName) 
			{
				isName = false;
				stream.get(c);
				continue;
			}
		}
		if (isName)				// avant les :
		{
			name += c;
			stream.get(c);
		}
		else					// skip osef char
		{
			value += c;
			stream.get(c);
		}
	}
}

size_t getBodyLen(const std::map<std::string, std::string>& headers)
{
	std::map<std::string, std::string>::const_iterator it = headers.find("content-length");
	if (it != headers.end())
	{
	    size_t bytes = std::stoul(it->second);
	    return bytes;
	}
	return 0;
}

void readBody(std::istream& stream, std::string& body, const size_t bodyLen)
{
	size_t read = 0;

	while (stream.good() && read < bodyLen)
	{
	    body += stream.get();
	    ++read;
	}
	throwIfNotGood(stream);
}

};



void throwIfNotGood(std::istream& stream)
{
	if (!stream.good()) 
	{
		if (stream.eof())
    		std::cout << "End of stream" << std::endl;
    	else 
    		std::cout << "Stream connexion issue" << std::endl;
  	}
}