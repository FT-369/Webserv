#ifndef CLIENTSOCKET_HPP
#define CLIENTSOCKET_HPP
class ClientSocket;
#include "Socket.hpp"
#include "Response.hpp"
#include "Request.hpp"

enum Stage
{
	GET_REQUEST, // 리퀘스트 파싱 시작
	END_OF_REQUEST, //리퀘스터 파싱 완료
	SET_RESOURCE, // 리소스 세팅 시작
	SEND_RESPONSE, // 세팅된 리소스로 응답 보내줌
	MAKE_AUTOINDEX,
	CGI_WRITE,
	CGI_READ,
	FILE_WRITE,
	FILE_READ,
	CLOSE,
};

class ClientSocket : public Socket
{
private:
	Request *_request;
	Response *_response;
	Resource *_resource;
	Stage _stage;
	ConfigServer _server_info;
	ConfigLocation *_route;
	std::string _file; // 라우팅 경로를 제외한 파일  // _route->getUrl() + _file = _path;
	bool _request_parse_error;

private:
	ClientSocket();

public:
	ClientSocket(int fd, ConfigServer server_info);
	~ClientSocket();
	ConfigServer getConnectServerInfo();
	Request *getRequest() const;
	Resource *getResource() const;
	Response *getResponse() const;
	ConfigLocation *getRoute() const;
	std::string getFile() const;

	
	void readRequest();
	void makeResponse();
	void sendResponse();
	Stage getStage();
	void setStage(Stage stage);

	std::string getErrorPage(std::string error_num);
	int isErrorRequest();
	bool isCGI(const std::string &path);
	void setResourceFd();
	void setGetFd();
	void setPostFd();
	void setDeleteFd();
	void setErrorResource(std::string error);
	void parsingCGIResponse();
	void setRoute();
	bool getRequestParseError() const;
	void setRequestParseError(bool);
};

#endif
