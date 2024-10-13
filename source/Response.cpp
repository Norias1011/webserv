#include "Response.hpp"

Response::Response() : _request(NULL), 
{
}

Response::Response(Client* client) : _request(client->getRequest())
{
}


Response::~Response()
{
}
