#ifndef RESOURCE_HPP
#define RESOURCE_HPP

#include "webserv.hpp"
#include "Request.hpp"
#include "Socket.hpp"

class Resource : public Socket // cgi가 실행이 됐는지를 waitpid로 확인 후 실행이 됐다면 write_fd[1]로 body를 보내준다.
{              // read_fd[0]을 kqueue에 read_event로 등록하고 이 후 cgi 실행이 완료 된 후
private:       // read_fd[1]로 cgi가 데이터를 보내주면 read_fd[0]에서 read_event가 발생한다. 읽어와서 보내주면 끝..
	int _write_fd;
	int _read_fd;
	pid_t _pid;
	std::string _content;
	std::string _extension;
	std::string _content_type;

public:
	Resource();
	~Resource();
	int getWriteFd();
	int getReadFd();
	pid_t getPid();
	std::string &getContent();
	std::string const &getContent() const;
	std::string getSrcExtension() const;
	std::string getSrcContentType() const;
	void setContent(std::string const &content);
	void setRequest(Request *req);
	void setWriteFd(int fd);
	void setReadFd(int fd);
	void setPid(pid_t pid);
	void setExtension(std::string const &extension);
	void setContentType(std::string const &content_type);
	void makeAutoIndex(std::string root, std::string directory);
};

#endif
