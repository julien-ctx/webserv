# 42_webserv - C++ Web Server

## Overview
42_webserv is a **C++ implementation of a web server** that is capable of handling multiple clients simultaneously using threads an non-blocking functions.

<img src="https://raw.githubusercontent.com/julien-ctx/42_webserv/master/assets/webserv.png">

## Features
- Parsing of a config file (**TOML**) to specify different rules for different roots/routes.
- Up to **128 clients** at the same time thanks to I/O multiplexing using kqueue.
- Handling of the following HTTP methods: **GET, POST, DELETE**.
- Error handling for invalid requests with **HTTP status codes**.
- File upload through a **Python CGI**.
- Support for multiple CGI scripts (**Python, Perl, Bash**).
- **Server names** to specify which hostname should be used to handle a specific request.
- Cookie session management using Javascript embedded snippets.
## Getting Started
You can run the project using the following syntax:

```
./webserv config/default.TOML
```
*You can change or edit the config file according to your needs.*

Then, all you have to do is opening Chrome browser and pasting the address and the port in the URL bar.

## Authors

- [@julien-ctx](https://github.com/julien-ctx)
- [@ktroude](https://github.com/ktroude)
- [@YeetusTitus](https://github.com/YeetusTitus)
