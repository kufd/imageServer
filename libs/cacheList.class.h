#ifndef __IMAGE_SERVER_CLASS_CACHE_LIST
#define __IMAGE_SERVER_CLASS_CACHE_LIST

#include "functions.h"
#include "config.class.h"
#include "log.class.h"

class cacheList
{
	class config *_config;
	class myLog *_log;
	class myLog *_logError;
	pthread_mutex_t _mutex;
  public:
    cacheList(class config *config, class myLog *log, class myLog *logError);
    ~cacheList();
    void add(const std::string &filename, int ad);
    void clear();
};

#endif
