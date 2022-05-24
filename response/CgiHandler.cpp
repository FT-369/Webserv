#include "CgiHandler.hpp"
#include "Server.hpp"
CgiHandler::CgiHandler(){}
CgiHandler::CgiHandler(ConfigLocation *location_info, ClientSocket *cs) 
: _location_info(location_info), _client_socket(cs)
{
	cgiInitEnv();
}

CgiHandler::~CgiHandler()
{
}

std::string getIp(int fd)
{
	struct sockaddr_in client_addr;
	socklen_t addr_len = sizeof(struct sockaddr_in);
	char ip[16];

	getsockname(fd, (struct sockaddr *)&client_addr, &addr_len);
	strncpy(ip, inet_ntoa(client_addr.sin_addr), 16);
	return(ip);
}

void CgiHandler::cgiInitEnv()
{
	_cgi_env["SERVER_PROTOCOL"] = "HTTP/1.1";
	_cgi_env["GATEWAY_INTERFACE"] = "CGI/1.1";
	_cgi_env["SERVER_SOFTWARE"] = "nginx server designed by sma";
	_cgi_env["SERVER_PORT"] = _client_socket->getRequest()->getPort();
	_cgi_env["SERVER_NAME"] = _client_socket->getRequest()->getServerName();
	_cgi_env["PATH_INFO"] = _client_socket->getRequest()->getPath();
	_cgi_env["PATH_TRANSLATED"] = _location_info->getCommonDirective()._root + _client_socket->getRequest()->getPath(); // cgi_path가 아닐까?
	_cgi_env["SCRIPT_NAME"] = _client_socket->getRequest()->getPath();
	_cgi_env["QUERY_STRING"] = _client_socket->getRequest()->getQuery();
	_cgi_env["REMOTE_ADDR"] = getIp(_client_socket->getRequest()->getSocketFD());
	_cgi_env["REDIRECT_STATUS"] = "200";
	_cgi_env["PHP_SELF"] = _client_socket->getRequest()->getPath();
	// _cgi_env["REMOTE_USER"]
	// _cgi_env["REMOTE_HOST"]
	_cgi_env["CONTENT_LENGTH"] = _client_socket->getRequest()->getRequestHeader()["content_length"];
	_cgi_env["CONTENT_TYPE"] = _client_socket->getRequest()->getContentType();
	_cgi_env["REQUEST_METHOD"] = _client_socket->getRequest()->getMethod();
	_cgi_env["REQUEST_URI"] = _client_socket->getRequest()->getPath() + "?" + _client_socket->getRequest()->getQuery();
}

char **CgiHandler::convertEnv()
{
	char **env;

	if (!(env = (char**)malloc(sizeof(char*) * (_cgi_env.size() + 1))))
	{
		return NULL;
	}

	int i = 0;
	for (std::map<std::string, std::string>::iterator it = _cgi_env.begin(); it != _cgi_env.end(); it++)
	{
		env[i] = strdup((it->first + "=" + it->second).c_str());
		i++;
	}
	env[i] = NULL;
	return env;
}

int CgiHandler::executeCgi()
{
	int read_fd[2];
	int write_fd[2];

	if (pipe(read_fd) < 0)
		return 500;
	if (pipe(write_fd) < 0)
	{
		close(read_fd[0]);
		close(read_fd[1]);
		return 500;
	}
	pid_t pid = fork();
	int status;

	if (pid < 0) 
	{
		throw 500;
	}
	else if (pid == 0)	// 자식이면 cgi 실행
	{
		std::string extension = _client_socket->getRequest()->getPath().substr(_client_socket->getRequest()->getPath().rfind(".") + 1);  // .php <-
		std::string cgi_file = _location_info->getCommonDirective()._cgi_path[extension];
		char *argv[4];
		int result = -1;
		char **envp = convertEnv();
		argv[0] = const_cast<char*>(cgi_file.c_str());
		argv[1] = const_cast<char*>(_client_socket->getRequest()->getPath().c_str());
		argv[2] = 0;
		if (_client_socket->getRequest()->getQuery() != "")
		{
			argv[2] = const_cast<char*>(_client_socket->getRequest()->getQuery().c_str());
		}
		argv[3] = 0;

		if (envp == NULL)
			return 500;
		dup2(write_fd[0],STDIN_FILENO);
		dup2(read_fd[1], STDOUT_FILENO);
		close(write_fd[1]);
		close(read_fd[0]);
		if ((result = execve(argv[0], argv, envp)) == -1)
		{
			exit(result);
		}
		exit(result);
	}
	else
	{
		close(write_fd[0]);
		close(read_fd[1]);
		_client_socket->getResource()->setReadFd(read_fd[0]);
		_client_socket->getResource()->setWriteFd(write_fd[1]);
		std::cerr << "response read_fd: " << read_fd[0] << std::endl;
		std::cerr << "response write_fd: " << write_fd[1] << std::endl;
	}
	return status;
}
