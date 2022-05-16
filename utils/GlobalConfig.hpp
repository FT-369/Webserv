#ifndef GLOBALCONFIG_HPP
#define GLOBALCONFIG_HPP

#include "utils.hpp"
#include <map>
#include <fstream>

#define MIME_TYPES "./setting/mime.types"

class GlobalConfig
{
private:
	static std::map<std::string, std::string> _mime_types;
	static std::map<std::string, std::string> _status_code;
	static std::string _default_type;

public:
	~GlobalConfig();
	static std::map<std::string, std::string> getMimeTypes();
	static std::map<std::string, std::string> getStatusCode();
	static std::string getDefaultType();
	static int initMimeTyeps();
	static void initStatusCode();

private:
	GlobalConfig();
};

#endif