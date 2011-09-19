#ifndef __IMAGE_SERVER_CLASS_LOGS
#define __IMAGE_SERVER_CLASS_LOGS

#include "functions.h"

class myLog
{
    std::string path;
    int logLevel;
    std::ofstream file;
    pthread_mutex_t _mutex;
  public:
    myLog(std::string setPath, int setLogLevel);
    ~myLog();
    void add(std::string message, int level);
};

#endif
