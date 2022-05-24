#ifndef EXCEPTIONHANDLER_HPP
#define EXCEPTIONHANDLER_HPP

#include "webserv.hpp"

class ExceptionHandler : public std::exception
{
protected:
	std::string _type;
	std::string _description;
	std::string _message;

public:
	ExceptionHandler(std::string const &type);
	ExceptionHandler(std::string const &type, std::string const &description);
	virtual ~ExceptionHandler() _NOEXCEPT;
	virtual const char* what() const throw();
};

#endif