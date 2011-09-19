#include "log.class.h"

myLog::myLog(std::string setPath, int setLogLevel)
{
	path=setPath;
	logLevel=setLogLevel;

	if(pthread_mutex_init(&_mutex, 0)!=0)
	{
		std::cout<<"ERROR. Initializing myLog::_mutex."<<std::endl;
		exit(1);
	}

	file.open(path.c_str(), std::ios::app);
	if(!file)
	{
		file.close();
		std::cout<<"Помилка при відкритті файлу логів:"<<path<<std::endl;
		exit(1);
	}
}
myLog::~myLog()
{
	pthread_mutex_destroy(&_mutex);
    file.close();
}
void myLog::add(std::string message, int level)
{
	pthread_mutex_lock(&_mutex);
    if(level<=logLevel)
    	file<<get_datetime("%d.%m.%Y %H:%M:%S")<<"    "<<message<<std::endl;
    pthread_mutex_unlock(&_mutex);
}
