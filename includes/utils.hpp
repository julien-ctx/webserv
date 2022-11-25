#pragma once

#include <string>
#include <map>
#include <vector>
#include <iostream>
#include <algorithm>
#include <sstream>
#include <cctype>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/event.h>
#include <unistd.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <sys/socket.h>
#include <sys/un.h>
#include <netdb.h>

#include "HttpMessages.hpp"
#include "request.hpp"
#include "Uri.hpp"


#define RESET   "\033[0m"
#define BLACK   "\033[1m\033[30m"      /* Bold Black */
#define RED     "\033[1m\033[31m"      /* Bold Red */
#define GREEN   "\033[1m\033[32m"      /* Bold Green */
#define YELLOW  "\033[1m\033[33m"      /* Bold Yellow */
#define BLUE    "\033[1m\033[34m"      /* Bold Blue */
#define MAGENTA "\033[1m\033[35m"      /* Bold Magenta */
#define CYAN    "\033[1m\033[36m"      /* Bold Cyan */
#define WHITE   "\033[1m\033[37m"      /* Bold White */


std::string method_to_string(int method) {
switch (method) {
    case 0 :       return "GET";
    case 1 :      return "POST";
    case 2 :    return "OPTIONS";
    }
    return "";
}

std::string statue_to_string(int status_code) {
switch (status_code) {
    case 200 :                    return "OK";
    case 400 :            return "Bad Request";
    case 500 :   return "Internal Server Error";
    }
    return "";
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


void exit_error (std::string str);