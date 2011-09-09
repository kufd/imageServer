#include "imageProcessor.class.h"


imageProcessor::imageProcessor(const config &config)
{
	config=_config;
}

imageProcessor::~imageProcessor()
{

}

void imageProcessor::setRequest(const request &request)
{
	_request = request;
}

void imageProcessor::writeIntoSocket(int socket)
{
	char hash[16];
	unsigned char *MD5(const unsigned char *d, unsigned long n, hash);
}
