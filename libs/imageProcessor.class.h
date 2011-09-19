#ifndef __IMAGE_SERVER_CLASS_IMAGE_PROCESSOR
#define __IMAGE_SERVER_CLASS_IMAGE_PROCESSOR

#include "functions.h"
#include "request.class.h"
#include "config.class.h"
#include "cacheList.class.h"
#include "log.class.h"
#include <Magick++.h>

class imageProcessor
{
	class request *_request;
	class config *_config;
	class myLog *_log;

	void getProportionSize(int w, int h, int cw, int ch, int &rh, int &rw);
	void writeErrorImgIntoSocket(int socket);
	bool writeFileIntoSocket(int socket, const std::string &path);
  public:
    imageProcessor(class config *config, class myLog *log);
    ~imageProcessor();
    void setRequest(class request *request);
    void writeIntoSocket(int socket);
};

#endif
