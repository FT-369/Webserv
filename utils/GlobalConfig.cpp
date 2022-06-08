#include "GlobalConfig.hpp"

std::map<std::string, std::string> GlobalConfig::_mime_types;
std::map<std::string, std::string> GlobalConfig::_status_code;
std::string GlobalConfig::_default_type = "text/plain";

GlobalConfig::~GlobalConfig()
{

}

std::map<std::string, std::string> GlobalConfig::getMimeTypes() { return GlobalConfig::_mime_types; }
std::map<std::string, std::string> GlobalConfig::getStatusCode() { return GlobalConfig::_status_code; }
std::string GlobalConfig::getDefaultType() { return GlobalConfig::_default_type; }

void GlobalConfig::initMimeTyeps()
{
	std::ifstream is(MIME_TYPES);
	std::string buffer;
	std::string line = "";
	std::vector<std::string> split_line;

	if (is.fail())
	{
		throw file_error("Unable to find the file: " + std::string(MIME_TYPES));
	}
	while (std::getline(is, buffer))
	{
		line += buffer;
	}

	split_line = ft_split(line, ";");
	if (ft_trim(split_line[split_line.size() - 1]) != "")
	{
		throw mimetypes_parsing_error("line does not end with ';'");
	}
	else
	{
		split_line.pop_back();
	}

	for (size_t i = 0; i < split_line.size(); i++)
	{
		std::vector<std::string> key_value = ft_split_space(split_line[i]);
		if (key_value.size() < 2)
		{
			throw mimetypes_parsing_error("Invalid directive value");
		}
		for (size_t i = 1; i < key_value.size(); i++)
		{
			GlobalConfig::_mime_types[key_value[i]] = key_value[0];
		}
	}
}

void GlobalConfig::initStatusCode()
{
	GlobalConfig::_status_code["100"] = "Continue";
	GlobalConfig::_status_code["101"] = "Switching Protocols";
	GlobalConfig::_status_code["200"] = "OK";
	GlobalConfig::_status_code["201"] = "Created";
	GlobalConfig::_status_code["202"] = "Accepted";
	GlobalConfig::_status_code["203"] = "Non-Authoritative";
	GlobalConfig::_status_code["204"] = "No Content";
	GlobalConfig::_status_code["205"] = "Reset Content";
	GlobalConfig::_status_code["206"] = "Partial Content";
	GlobalConfig::_status_code["300"] = "Multiple Choices";
	GlobalConfig::_status_code["301"] = "Moved Permanently";
	GlobalConfig::_status_code["302"] = "Found";
	GlobalConfig::_status_code["303"] = "See Other";
	GlobalConfig::_status_code["304"] = "Not Modified";
	GlobalConfig::_status_code["305"] = "Use Proxy";
	GlobalConfig::_status_code["307"] = "Temporary Redirect";
	GlobalConfig::_status_code["400"] = "Bad Request";
	GlobalConfig::_status_code["401"] = "Unauthorized";
	GlobalConfig::_status_code["403"] = "Forbidden";
	GlobalConfig::_status_code["404"] = "Not Found";
	GlobalConfig::_status_code["405"] = "Method Not Allowed";
	GlobalConfig::_status_code["406"] = "Not Acceptable";
	GlobalConfig::_status_code["407"] = "Proxy Authentication";
	GlobalConfig::_status_code["408"] = "Request Timeout";
	GlobalConfig::_status_code["409"] = "Conflict";
	GlobalConfig::_status_code["410"] = "Gone";
	GlobalConfig::_status_code["411"] = "Length Required";
	GlobalConfig::_status_code["412"] = "Precondition Failed";
	GlobalConfig::_status_code["413"] = "Request Entity Too Large";
	GlobalConfig::_status_code["414"] = "Request URI Too Long";
	GlobalConfig::_status_code["415"] = "Unsupported Media Type";
	GlobalConfig::_status_code["416"] = "Range Not Satisfiable";
	GlobalConfig::_status_code["500"] = "Internal Server Error";
	GlobalConfig::_status_code["501"] = "Not Implemented";
	GlobalConfig::_status_code["502"] = "Bad Gateway";
	GlobalConfig::_status_code["503"] = "Service Unavailable";
	GlobalConfig::_status_code["504"] = "Gateway Timeout";
	GlobalConfig::_status_code["505"] = "HTTP Version Not Supported";
}
