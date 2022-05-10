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

std::map<std::string, std::string>	Response::getMimeType() { return mime_types; }

std::string Response::getContentType(std::string file)
{
	// std::string content_type = config.getDefaultType();
	std::string content_type = "text/html";

	size_t rpos = file.rfind(".");
	std::string extension;

	if (rpos == std::string::npos) {
		return content_type;
	}
	extension = file.substr(rpos + 1);

	// std::cout << "file: " << file << ", extension" <<  extension << std::endl;
	if (mime_types.find(extension) != mime_types.end())
		content_type = mime_types[extension];
	return content_type;
}

void	Response::makeStartLine()
{
	start_line = request->protocol + " " + status + " " + status_code[status] + "\r\n";
}

void	Response::makeHeader()
{
	std::cout << "getpath = " << request->getPath();
	header["Content-type"] = getContentType(request->getPath());
	std::cout << "!!!!!!" << header["Content-type"] << std::endl;
	header["Content-length"] = std::to_string(entity.length());
	header["Server"] = "Mac Web Server";
}

void	Response::makeEntity()
{
	std::string buffer;
	std::ifstream is("./static_file/index.html");

	if (is.fail())
	{
		status = "404";
		return; //throw error
	}
	while (std::getline(is, buffer)) {
		entity += buffer + "\n";
	}
	status = "200";
}

void Response::mappingPath() {
	std::string path = request->getPath();
	int path_len = path.size();

	for (int i = path_len - 1; i >= 0; i++)
	{
		if (i == path_len - 1 || path[i] == '/') {
			for (int j = 0; j < locations.size(); j++) {
				if (path.substr(0, i) == locations[j].getUrl()) {
					route = new ConfigLocation(locations[j].getUrl(), locations[j].getCommonDirective());
				}
			}
		}
	}
}

std::string	Response::makeGetResponse() {
	std::string send_data;
	std::map<std::string, std::string>::iterator it;

	/*
	std::string filename;

	if (파일명이 없고) { <- route의 url == request path 완벽히 일치
		if (index가 있으면) { <- route의 멤버변수 확인
			filename = IndexPage
		} else (autoindex on) { <- route의 멤버변수 확인
			filename = ListingPage
		} else {
			filename  = 403Page
		}
	} else if (파일명이 있지만 리소스가 없을 때) {	<- 함수 이용해서, 디렉토리 및 파일이 있는지 확인
		if (autoindex on) { <- route의 멤버변수 확인
			filename = ListingPage
		} else {
			filename = 403Page
		}
	} else {	// <- 파일명이 있고, 리소스가 있을 때
		filename = StaticFile
	}
	
	makeEntity(filename);
	*/

	mappingPath();
	makeEntity( );
	makeHeader();
	makeStartLine();

	send_data += start_line;
	for (it = header.begin(); it != header.end(); it++) {
		send_data += it->first + ": " + it->second + "\r\n";
	}
	send_data += "\r\n" + entity + "\r\n";
	return send_data;
}

void	Response::setStatusCode()
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