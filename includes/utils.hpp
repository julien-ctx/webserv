#pragma once

#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/event.h>
#include <unistd.h>
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <sys/socket.h>
#include <sys/un.h>
#include <netdb.h>
#include <fcntl.h>
#include <vector>
#include <map>

#define BUFFER_SIZE 30000
#define TIMEOUT 1000
#define EVENTS_NUM 4

#define RESET   "\033[0m"
#define BLACK   "\033[1m\033[30m"      /* Bold Black */
#define RED     "\033[1m\033[31m"      /* Bold Red */
#define GREEN   "\033[1m\033[32m"      /* Bold Green */
#define YELLOW  "\033[1m\033[33m"      /* Bold Yellow */
#define BLUE    "\033[1m\033[34m"      /* Bold Blue */
#define MAGENTA "\033[1m\033[35m"      /* Bold Magenta */
#define CYAN    "\033[1m\033[36m"      /* Bold Cyan */
#define WHITE   "\033[1m\033[37m"      /* Bold White */

void exit_error (std::string str);

extern char **environ;

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

enum EventType {
    S_READ,
    C_READ,
    C_WRITE,
    C_TIMER
};
