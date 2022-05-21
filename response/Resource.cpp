#include "Resource.hpp"


Resource::Resource() : _write_fd(-1), _read_fd(-1), _pid(-1)
{

}

Resource::~Resource()
{

}

int Resource::getWriteFd() { return _write_fd; }
int Resource::getReadFd() { return _read_fd; }
pid_t Resource::getPid() { return _pid; }
std::string &Resource::getContent() { return _content; }
std::string const &Resource::getContent() const { return _content; }
std::string Resource::getSrcExtension() const { return _extension; }
std::string Resource::getSrcContentType() const { return _content_type; }

void Resource::setWriteFd(int fd) { _write_fd = fd; }
void Resource::setReadFd(int fd) { _read_fd = fd; }
void Resource::setPid(pid_t pid) { _pid = pid; }
void Resource::setExtension(std::string const &extension) { _extension = extension; }
void Resource::setContentType(std::string const &content_type) { _content_type = content_type; }

void Resource::makeAutoIndex(std::string root, std::string directory)
{
	std::string host = "http://" + getSocketHost() + ":" + std::to_string(getSocketPort());
	std::string pos = (directory == "" || directory[directory.size() - 1] != '/') ? directory + "/" : directory;
	DIR *dir = NULL;

	_content += "<!DOCTYPE html>\n";
	_content += "<html>\n";
	_content += "<head>\n</head>\n";
	_content += "<body>\n";
	_content += "<h1> Index of "+ pos + "</h1>\n";

	dir = opendir((root + directory).c_str());
	if (dir == NULL)
		return ; // error

	struct dirent *file = NULL;
	while ((file = readdir(dir)) != NULL) {
		std::string d_name = file->d_type == DT_DIR ? std::string(file->d_name) + "/" : std::string(file->d_name);
		_content += "<a href=\"" + host + pos + file->d_name + "\">";
		_content += file->d_name;
		if (file->d_type == DT_DIR)
			_content += + "/";
		_content += "</a><br>\n";
	}
	closedir(dir);

	_content += "</body>\n";
	_content += "</html>\n";
}