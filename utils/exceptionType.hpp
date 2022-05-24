#ifndef EXCEPTIONTYPE_HPP
#define EXCEPTIONTYPE_HPP

#include "ExceptionHandler.hpp"

struct	config_error : public ExceptionHandler
{
	config_error() : ExceptionHandler("Config Error") { }
	config_error(std::string const &description) : ExceptionHandler("Config Error", description) { }
};

#endif