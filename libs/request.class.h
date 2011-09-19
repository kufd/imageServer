#ifndef __IMAGE_SERVER_CLASS_REQUEST
#define __IMAGE_SERVER_CLASS_REQUEST

#include "functions.h"

class request
{
	std::string _request;
	std::map<std::string, std::string> _data;
	std::map<std::string, std::string> _params;

	void parseRequest();
	void parseGetParams();
  public:
	request(std::string request);
    ~request();
    std::map<std::string, std::string> data();
    std::string data(std::string key);
    std::map<std::string, std::string> params();
    std::string params(std::string key);
    static std::string urlDecode(const std::string & s);
};

#endif
