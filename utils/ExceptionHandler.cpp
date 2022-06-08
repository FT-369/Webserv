#include "ExceptionHandler.hpp"

ExceptionHandler::ExceptionHandler(std::string const &type) : _type(type) {
	_message = _type;
}

ExceptionHandler::ExceptionHandler(std::string const &type, std::string const &description) : _type(type), _description(description) {
	_message = _type + ": " + _description;
}

ExceptionHandler::~ExceptionHandler() _NOEXCEPT { }

const char* ExceptionHandler::what() const throw() {
	return _message.c_str();
}