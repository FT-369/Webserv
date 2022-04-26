#ifndef CONFIGSERVER_HPP
#define CONFIGSERVER_HPP

#include <iostream>
#include <map>
#include <string>
#include <vector>
#include "ConfigLocation.hpp"

class ConfigServer
{
private:
    std::vector<ConfigLocation> locations;
    std::map<std::string, std::string> simple_directive;
    /* data */
public:
    ConfigServer(/* args */);
    ~ConfigServer();
};

ConfigServer::ConfigServer(/* args */)
{
}

ConfigServer::~ConfigServer()
{
}

#endif
