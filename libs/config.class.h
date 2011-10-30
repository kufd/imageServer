#ifndef __IMAGE_SERVER_CLASS_CONFIG
#define __IMAGE_SERVER_CLASS_CONFIG

#include "functions.h"
#include "log.class.h"

class config
{
    std::map<std::string, std::string> _data;
    class myLog *_log;
    class myLog *_logError;
  public:
    config();
    ~config();
    std::string get(std::string param);
    void setLog(class myLog *log, class myLog *logError);
    void testPidFIle();
    void createPidFIle();
};

#endif
