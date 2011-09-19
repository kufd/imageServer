#include "cacheList.class.h"

cacheList::cacheList(class config *config, class myLog *log)
{
	_config = config;
	_log = log;

	if(pthread_mutex_init(&_mutex, 0)!=0)
	{
		_log->add("ERROR. Initializing cacheList::_mutex.", 0);
		exit(1);
	}
}

cacheList::~cacheList()
{
	pthread_mutex_destroy(&_mutex);
}

void cacheList::add(const std::string &filename, int ad)
{
	pthread_mutex_lock(&_mutex);
	std::string path = _config->get("cacheListDir")+"/"+get_datetime("%d.%m.%Y", time(NULL)+ad*24*60*60)+".list";
	int file=open(path.c_str(), O_CREAT|O_APPEND|O_WRONLY, 0644);
	if(file!=-1)
	{
	      std::string str=filename+"\n";
	      write(file,(const void *)str.c_str(),(size_t)str.size());
	}
	else
	{
		_log->add("ПОМИЛКА. Помилка при відкритті файлу: "+path, 0);
	}
	close(file);
	pthread_mutex_unlock(&_mutex);
}

/**
 * method remove outdated files
 */
void cacheList::clear()
{
	std::ifstream file;
	std::string cacheListPath, data="";
	char buf[1025];
	int loc1, i=0;

	_log->add("Starting process of clearing cacheList", 0);

	cacheListPath = _config->get("cacheListDir")+"/"+get_datetime("%d.%m.%Y", time(NULL)-(++i)*24*60*60)+".list";
	file.open(cacheListPath.c_str(), std::ios::in | std::ios::binary);

	while(file)
	{
		//read file
		while(!file.eof())
		{
			file.read((char *)&buf,1024);
			buf[file.gcount()]='\0';
			data+=buf;
		}
		file.close();

		//remove files
		while((loc1=data.find("\n",0))!= std::string::npos)
		{
			std::string cacheFileName=data.substr(0,loc1);
			std::string cacheFilePath=_config->get("cacheDir")+"/"+cacheFileName[0]+"/"+cacheFileName[1]+"/"+cacheFileName[2]+"/"+cacheFileName;
			data=data.substr(loc1+1);

			if(remove(cacheFilePath.c_str())!=0)
				_log->add("ERROR. File can't be removed: "+cacheFilePath, 0);
			else
				_log->add("File was removed: "+cacheFilePath, 0);

			//sleep
			usleep(1000);
		}

		//remove cache list file
		if(remove(cacheListPath.c_str())!=0)
			_log->add("ERROR. File can't be removed: "+cacheListPath, 0);
		else
			_log->add("File was removed: "+cacheListPath, 0);

		//open new file
		cacheListPath = _config->get("cacheListDir")+"/"+get_datetime("%d.%m.%Y", time(NULL)-(++i)*24*60*60)+".list";
		file.open(cacheListPath.c_str(), std::ios::in | std::ios::binary);
	}

	_log->add("Stopping process of clearing cacheList", 0);
}

