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
	std::string extension = _client_socket->getRequest()->getPath().substr(_client_socket->getRequest()->getPath().rfind(".") + 1);
	_cgi_env["SERVER_PROTOCOL"] = "HTTP/1.1";
	_cgi_env["GATEWAY_INTERFACE"] = "CGI/1.1";
	_cgi_env["SERVER_SOFTWARE"] = "nginx server designed by sma";
	_cgi_env["REQUEST_METHOD"] = _client_socket->getRequest()->getMethod();
	_cgi_env["REQUEST_SCHEME"] = _client_socket->getRequest()->getMethod();
	_cgi_env["SERVER_PORT"] = _client_socket->getRequest()->getPort();
	_cgi_env["SERVER_NAME"] = _client_socket->getRequest()->getServerName();
	_cgi_env["DOCUMENT_ROOT"] = _location_info->getCommonDirective()._cgi_path[extension];
	_cgi_env["DOCUMENT_URI"] = _client_socket->getRequest()->getPath() + (_client_socket->getRequest()->getQuery().size() > 0 ? ("?" + _client_socket->getRequest()->getQuery()) : ""); // 리퀘스트에 명시된 전체 주소가 들어가야 함
	_cgi_env["REQUEST_URI"] = _client_socket->getRequest()->getPath() + (_client_socket->getRequest()->getQuery().size() > 0 ? ("?" + _client_socket->getRequest()->getQuery()) : ""); // 리퀘스트에 명시된 전체 주소가 들어가야 함
	_cgi_env["SCRIPT_NAME"] = _client_socket->getFile(); // 실행파일 전체 주소가 들어가야함
	_cgi_env["SCRIPT_FILENAME"] = _client_socket->getRoute()->getCommonDirective()._root + _client_socket->getFile();// 실행파일 전체 주소가 들어가야함
	_cgi_env["QUERY_STRING"] = _client_socket->getRequest()->getQuery();
	_cgi_env["REMOTE_ADDR"] = getIp(_client_socket->getSocketFd());
	_cgi_env["REDIRECT_STATUS"] = "200";
	if (_client_socket->getRequest()->getRequestBody().size() > 0)
		_cgi_env["CONTENT_LENGTH"] = std::to_string(_client_socket->getRequest()->getRequestBody().size());
	_cgi_env["CONTENT_TYPE"] = _client_socket->getRequest()->getContentType();
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
	int status = 200;

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
		char **env;

		if (!(env = (char**)malloc(sizeof(char*) * (_cgi_env.size() + 1))))
		{
			return -1;
		}
		size_t i = 0;
		for (std::map<std::string, std::string>::iterator it = _cgi_env.begin(); it != _cgi_env.end(); it++)
		{
			env[i] = strdup((it->first + "=" + it->second).c_str());
			i++;
		}
		env[i] = NULL;
		std::string path_ = _client_socket->getRoute()->getCommonDirective()._root + _client_socket->getRequest()->getPath(); 
		argv[0] = const_cast<char*>(cgi_file.c_str());
		argv[1] = const_cast<char*>(path_.c_str());
		argv[2] = 0;
		if (_client_socket->getRequest()->getQuery() != "")
		{
			argv[2] = const_cast<char*>(_client_socket->getRequest()->getQuery().c_str());
		}
		argv[3] = 0;
		if (env == NULL)
		{
			return 500;
		}
		dup2(write_fd[0],STDIN_FILENO);
		dup2(read_fd[1], STDOUT_FILENO);
		close(write_fd[1]);
		close(read_fd[0]);
		if ((result = execve(argv[0], argv, env)) == -1)
		{
			for (size_t idx = 0; idx < i; idx++)
			{
				free(env[idx]);
				env[idx] = 0;
			}
			free(env);
			env = 0;
			exit(result);
		}
		for (size_t idx = 0; idx < i; idx++)
		{
			free(env[idx]);
			env[idx] = 0;
		}
		free(env);
		env = 0;
		exit(result);
	}
	else
	{
		close(write_fd[0]);
		close(read_fd[1]);

		_client_socket->getResource()->setPid(pid);
		_client_socket->getResource()->setReadFd(read_fd[0]);
		_client_socket->getResource()->setWriteFd(write_fd[1]);
	}
	return status;
}
