#include "Response.hpp"

Response::Response(std::map<std::string, std::string> const &mime_types, std::vector<ConfigLocation> locations)
	: mime_types(mime_types), request(NULL), locations(locations), route(NULL)
{
}

Response::Response(std::map<std::string, std::string> const &mime_types, Request *request, std::vector<ConfigLocation> locations)
	: mime_types(mime_types), request(request), locations(locations), route(NULL)
{
}

Response::~Response()
{
}

std::map<std::string, std::string> Response::getMimeType() { return mime_types; }

std::string Response::getContentType(std::string file)
{
	// std::string content_type = config.getDefaultType();
	std::string content_type = "text/html";

	size_t rpos = file.rfind(".");
	std::string extension;

	if (rpos == std::string::npos)
	{
		return content_type;
	}
	extension = file.substr(rpos + 1);

	if (mime_types.find(extension) != mime_types.end())
		content_type = mime_types[extension];
	return content_type;
}

void Response::makeStartLine()
{
	start_line = request->protocol + " " + status + " " + status_code[status] + "\r\n";
}

void Response::makeHeader()
{
	header["Content-type"] = getContentType(request->getPath());
	header["Content-length"] = std::to_string(entity.length());
	header["Server"] = "Mac Web Server";
}

void Response::makeEntity(std::string file)
{
	std::string buffer;
	std::string filePath;
	// std::cout << "경로 : " << this->route->getCommonDirective().root + "/" + this->route->getCommonDirective().index[0] << std::endl;
	// std::ifstream is(this->route->getCommonDirective().root + "/" + this->route->getCommonDirective().index[0]);
	std::ifstream is(file);

	if (is.fail())
	{
		status = "404";
		return; // throw error
	}
	while (std::getline(is, buffer))
	{
		entity += buffer + "\n";
	}
	status = "200";
}

void Response::setRedirect()
{
	if (!route->getReturnCode())
		return;
	this->status = std::to_string(route->getReturnCode());
	header["Location"] = route->getReturnDate();
}

void Response::mappingPath()
{
	std::string path = request->getPath();
	int path_len = path.size();
	std::cout << "path : " << path << std::endl;
	file = "";
	for (int i = path_len - 1; i >= -1; i--)
	{
		if (i == path_len - 1 || path[i + 1] == '/')
		{
			for (int j = 0; j < locations.size(); j++)
			{
				if (path.substr(0, i + 1) == locations[j].getUrl() || path.substr(0, i + 1) == locations[j].getUrl() + "/")
				{
					route = new ConfigLocation(locations[j].getUrl(), locations[j].getCommonDirective());
					if (i != path_len - 1)
						file = path.substr(i + 1);
					return ;
				}
				else if (i == -1 && locations[j].getUrl() == "/")
				{
					route = new ConfigLocation("/", locations[j].getCommonDirective());
					file = request->getPath();
					return ;
				}
			}
		}
	}
}

std::string Response::makeResponse()
{
	std::string send_data;
	std::map<std::string, std::string>::iterator it;
	//요청 url <=> location 매핑
	mappingPath();
	std::cout << "[Mapping Path] url: " << route->getUrl() << ", file:" << file << std::endl;
	//요청 method가 limitExcept에 존재하지 않으면 405 error
	if (find(route->getCommonDirective().limit_except.begin(), route->getCommonDirective().limit_except.end(),
			 request->getMethod()) == route->getCommonDirective().limit_except.end())
	{
		makeErrorResponse("405");
	}
	else if (request->getMethod() == "GET")
	{
		std::cout << "GET" << std::endl;
		makeGetResponse();
	}
	else if (request->getMethod() == "POST")
	{
		std::cout << "POST" << std::endl;
		// makePostResponse();
	}
	else if (request->getMethod() == "DELETE")
	{
		std::cout << "DELETE" << std::endl;
		makeDeleteResponse();
	}
	setRedirect();
	send_data += this->start_line;
	for (it = this->header.begin(); it != this->header.end(); it++)
	{
		send_data += it->first + ": " + it->second + "\r\n";
	}
	send_data += "\r\n" + entity + "\r\n";
	return send_data;
}

std::string Response::settingRoute()
{
	std::string entityFile = "./static_file/404.html";
	std::string root = route->getCommonDirective().root;
	std::vector<std::string> indexPage = route->getCommonDirective().index;

	if (file == "")
	{

		if (!indexPage.empty())
		{
			for (size_t i = 0; i < indexPage.size(); i++)
			{
				std::ifstream idx(root + "/" + indexPage[i]);
				std::cout << "indexPage[" << i << "]: " << indexPage[i] << std::endl;
				if (idx.is_open())
				{
					entityFile = root + "/" + indexPage[i];
					std::cout << "indexPage[" << i << "] is open" << std::endl;
					break;
				}
				std::cout << "indexPage[" << i << "] is fail" << std::endl;
			}
		}
		else if (route->getCommonDirective().autoindex)
		{
			entityFile = root + "/autoindex.html";
		}
	}
	else
	{
		std::ifstream is(root + file);

		if (!is.fail())
		{
			entityFile = root + file;
		}
		else if (route->getCommonDirective().autoindex)
		{
			entityFile = root + "/autoindex.html";
		}
	}
}

void Response::makeGetResponse()
{
	makeEntity(settingRoute());
	makeHeader();
	makeStartLine();
}

void Response::makeDeleteResponse()
{
	makeEntity(settingRoute());
	makeHeader();
	makeStartLine();
}

void Response::makeErrorResponse(std::string error_num)
{
	this->status = error_num;
	makeEntity(settingRoute());
	makeHeader();
	makeStartLine();
}

void Response::setStatusCode()
{
	status_code["100"] = "Continue";
	status_code["101"] = "Switching Protocols";
	status_code["200"] = "OK";
	status_code["201"] = "Created";
	status_code["202"] = "Accepted";
	status_code["203"] = "Non-Authoritative";
	status_code["204"] = "No Content";
	status_code["205"] = "Reset Content";
	status_code["206"] = "Partial Content";
	status_code["300"] = "Multiple Choices";
	status_code["301"] = "Moved Permanently";
	status_code["302"] = "Found";
	status_code["303"] = "See Other";
	status_code["304"] = "Not Modified";
	status_code["305"] = "Use Proxy";
	status_code["307"] = "Temporary Redirect";
	status_code["400"] = "Bad Request";
	status_code["401"] = "Unauthorized";
	status_code["403"] = "Forbidden";
	status_code["404"] = "Not Found";
	status_code["405"] = "Method Not Allowed";
	status_code["406"] = "Not Acceptable";
	status_code["407"] = "Proxy Authentication";
	status_code["408"] = "Request Timeout";
	status_code["409"] = "Conflict";
	status_code["410"] = "Gone";
	status_code["411"] = "Length Required";
	status_code["412"] = "Precondition Failed";
	status_code["413"] = "Request Entity Too Large";
	status_code["414"] = "Request URI Too Long";
	status_code["415"] = "Unsupported Media Type";
	status_code["416"] = "Requested Range Not Satisfiable";
	status_code["500"] = "Expectation Failed";
	status_code["501"] = "Internal Server Error";
	status_code["502"] = "Not Implemented";
	status_code["503"] = "Bad Gateway";
	status_code["504"] = "Gateway Timeout";
	status_code["505"] = "HTTP Version Not Supported";
}
