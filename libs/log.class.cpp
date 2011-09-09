#include "log.class.h"

log::log(std::string setPath, int setLogLevel)
{
  path=setPath;
  logLevel=setLogLevel;

  file.open(path.c_str(), std::ios::app);
  if(!file)
  {
      file.close();
      std::cout<<"Помилка при відкритті файлу логів:"<<path<<std::endl;
      exit(1);
  }
}
log::~log()
{
    file.close();
}
void log::add(std::string message, int level)
{
    if(level<=logLevel)
      file<<get_datetime("%d.%m.%Y %H:%M:%S")<<"    "<<message<<std::endl;
}
