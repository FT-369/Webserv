#include "Resource.hpp"


Resource::Resource() : _write_fd(-1), _read_fd(-1), _pid(-1)
{

}

Resource::~Resource()
{

}

int Resource::getWriteFd()
{
	return _write_fd;
}

int Resource::getReadFd()
{
	return _read_fd;
}

pid_t Resource::getPid()
{
	return _pid;
}

std::string &Resource::getContent()
{
	return (content);
}

std::string const &Resource::getContent() const
{
	return (content);
}

void Resource::setRequest(Request *req)
{
	_req = req;
}

void Resource::setWriteFd(int fd)
{
	_write_fd = fd;
}

void Resource::setReadFd(int fd)
{
	_read_fd = fd;
}
void Resource::getPid(pid_t pid)
{
	_pid = pid;
}
