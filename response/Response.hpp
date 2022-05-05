#ifndef RESPONSE_HPP
#define RESPONSE_HPP

class Response
{
private:
    std::string start_line;
    std::string header;
    std::string entity;

    /* data */
public:
    Response(/* args */);
    ~Response();
};

Response::Response(/* args */)
{
}

Response::~Response()
{
}

#endif
