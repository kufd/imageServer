#ifndef __IMAGE_SERVER_CLASS_LOGS
#define __IMAGE_SERVER_CLASS_LOGS

#include "functions.h"

class log
{
    std::string path;
    int logLevel;
    std::ofstream file;
  public:
    log(std::string setPath, int setLogLevel);
    ~log();
    void add(std::string message, int level);
};

#endif
