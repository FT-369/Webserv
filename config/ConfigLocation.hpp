#ifndef CONFIGLOCATION_HPP
#define CONFIGLOCATION_HPP

#include <iostream>
#include <map>
#include <string>
#include <vector>

class ConfigLocation
{
private:
    std::string url;
    short       url_prefix;
    std::map<std::string, std::vector<std::string> > simple_directive;
    /* data */
public:
    ConfigLocation(/* args */);
    ~ConfigLocation();
};

#endif