#ifndef __IMAGE_SERVER_CLASS_IMAGE_PROCESSOR
#define __IMAGE_SERVER_CLASS_IMAGE_PROCESSOR

#include "functions.h"
#include "libs/request.class.h"
#include "libs/config.class.h"
#include <openssl/md5.h>

class imageProcessor
{
	const request _request;
	const config _config;
  public:
    imageProcessor(const config &config);
    ~imageProcessor();
    void setRequest(const request &request);
    void writeIntoSocket(int socket);
};

#endif
