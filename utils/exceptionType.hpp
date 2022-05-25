#ifndef EXCEPTIONTYPE_HPP
#define EXCEPTIONTYPE_HPP

#include "ExceptionHandler.hpp"

struct	file_error : public ExceptionHandler
{
	file_error() : ExceptionHandler("File Error") { }
	file_error(std::string const &description) : ExceptionHandler("File Error", description) { }
};

struct	config_error : public ExceptionHandler
{
	config_error() : ExceptionHandler("Config Error") { }
	config_error(std::string const &description) : ExceptionHandler("Config Error", description) { }
};

struct	config_parsing_error : public ExceptionHandler
{
	config_parsing_error() : ExceptionHandler("Config Parsing Error") { }
	config_parsing_error(std::string const &description) : ExceptionHandler("Config Parsing Error", description) { }
};

struct	mimetypes_parsing_error : public ExceptionHandler
{
	mimetypes_parsing_error() : ExceptionHandler("MimeTypes Parsing Error") { }
	mimetypes_parsing_error(std::string const &description) : ExceptionHandler("MimeTypes Parsing Error", description) { }
};

struct	socket_error : public ExceptionHandler
{
	socket_error() : ExceptionHandler("Socket Error") { }
	socket_error(std::string const &description) : ExceptionHandler("Socket Error", description) { }
};

#endif