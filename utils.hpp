#pragma once

#include <string>
#include <map>
#include <vector>
#include <iostream>
#include <algorithm>
#include <sstream>
#include "HttpMessages.hpp"
#include "Request.hpp"
#include "Uri.hpp"


std::string to_string(HttpMethod method) {
switch (method) {
    case HttpMethod::GET:       return "GET";
    case HttpMethod::POST:      return "POST";
    case HttpMethod::DELETE:    return "OPTIONS";
    }
    return "";
}

std::string to_string(HttpStatusCode status_code) {
switch (status_code) {
    case HttpStatusCode::OK:                    return "OK";
    case HttpStatusCode::BadRequest:            return "Bad Request";
    case HttpStatusCode::InternalServerError:   return "Internal Server Error";
    }
    return "";
}


HttpMethod string_to_method(const std::string& method_string) {
std::string method_string_uppercase;
std::transform(method_string.begin(), method_string.end(), std::back_inserter(method_string_uppercase),
                [](char c) { return toupper(c); });   // Secu pour tout passer en maj | explication --> https://en.cppreference.com/w/cpp/algorithm/transform
if (method_string_uppercase == "GET")
    return HttpMethod::GET;
else if (method_string_uppercase == "POST")
    return HttpMethod::POST;
else if (method_string_uppercase == "DELETE")
    return HttpMethod::DELETE;
else
    throw std::invalid_argument("Unexpected HTTP method");
}
