#ifndef __IMAGE_SERVER_CLASS_CONFIG
#define __IMAGE_SERVER_CLASS_CONFIG

#include "functions.h"

class config
{
    std::map<std::string, std::string> _data;
  public:
    config();
    ~config();
    std::string get(std::string param);
};

#endif
