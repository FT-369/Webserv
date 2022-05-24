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
	MAKE_RESPONSE, // 세팅된 리소스로 응답 보내줌
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

private:
	ClientSocket();

public:
	ClientSocket(int fd, ConfigServer server_info);
	ConfigServer getConnectServerInfo();
	~ClientSocket();
	Request *getRequest() const;
	Resource *getResource() const;
	Response *getResponse() const;
	int recieveRequest();
	void sendResponse();
	Stage getStage();
	void setStage(Stage stage);
	std::string getErrorPage(std::string error_num);
	bool isCGI(const std::string &path);
	void setResourceFd();
	void setGetFd();
	void setPostFd();
	void setErrorResource(std::string error);
};

#endif
