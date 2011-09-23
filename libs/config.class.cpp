#include "config.class.h"

config::config()
{
	_log = NULL;
	_logError = NULL;

	//встановлюю значення за замовчуванням
	_data["log"]="/var/log/imageServer.log";
	_data["logError"]="/var/log/imageServer-error.log";
	_data["logLevel"]="9";
	_data["port"]="1111";
	_data["imagesDir"]="/var/images";
	_data["cacheDir"]="/var/cache";
	_data["daemon"]="no";
	_data["uid"]="0";
	_data["gid"]="0";
	_data["pidFile"]="/run/imageServer/imageServer.pid";

	//відкриття файлу налаштуваннь
	std::ifstream file("conf/imageServer.conf", std::ios::in | std::ios::binary);
	if(!file)
	{
		file.close();
		file.open("/etc/imageServer/imageServer.conf", std::ios::in | std::ios::binary);
	}

	if(file)
	{
		//Читання файлу налаштувань
		std::string config="";
		char buf[1025];
		while(!file.eof())
		{
			file.read((char *)&buf,1024);
			buf[file.gcount()]='\0';
			config+=buf;
		}
		config+="\n";
		file.close();

		//парсинг файлу
		int len = config.size();
		int i=0;
		std::string contentType="key", key="", value="";
		for(i=0; i<len; i++)
		{
			char ch = config.at(i);

			if(ch=='\r' || ch=='\n') //key
			{
				contentType="key";
				if(key!="")
				{
					_data[trim(key)]=trim(value);
					key=value="";
				}
			}
			else if(ch=='=') //value
			{
				if(contentType!="comment")
					contentType="value";
			}
			else if(ch=='#') //comment
			{
				contentType="comment";
			}
			else
			{
				if(contentType=="key")
				{
					key+=ch;
				}
				else if(contentType=="value")
				{
					value+=ch;
				}
			}
		}
	}

	_data["cacheListDir"]=_data["cacheDir"]+"/list";


	//Set gid and uid for server
	if(_data["gid"]!="0")
	{
		if(setegid(strToInt(_data["gid"])))
		{
			std::cout<<"Error setting gid."<<std::endl;
			exit(1);
		}
	}
	if(_data["uid"]!="0")
	{
		if(seteuid(strToInt(_data["uid"])))
		{
			std::cout<<"Error setting uid."<<std::endl;
			exit(1);
		}
	}


	//create pid file
	for(int i=1;i<=30;i++)
	{
		std::ifstream pidFileTest(_data["pidFile"].c_str(), std::ios::in);
		if(pidFileTest)
		{
			pidFileTest.close();
			if(i==30)
			{
				std::cout<<"Pid-file exists: "<<_data["pidFile"]<<std::endl;
				exit(1);
			}
			exactSleep(2);
		}
		else
			break;
	}
	std::ofstream pidFile(_data["pidFile"].c_str(), std::ios::out);
    if(pidFile)
    {
        pidFile<<(getpid());
        pidFile.close();
    }
    else
    {
        std::cout<<"Can't create pid-file: "<<_data["pidFile"]<<std::endl;
        exit(1);
    }

	//Тут просто створюю директорії без перевірки їх існування
	//якщо вони вже є то просто функція створення директорії поверне помилку
	mkdir(_data["cacheDir"].c_str(), 0755);
	mkdir(_data["cacheListDir"].c_str(), 0755);

	//Створюю піддиректорії кешу
	for(int i1=48; i1<=112; i1++)
	{
		if(i1==58)
			i1=97;
		mkdir((_data["cacheDir"]+"/"+(char)i1).c_str(), 0755);
		for(int i2=48; i2<=112; i2++)
		{
			if(i2==58)
				i2=97;
			mkdir((_data["cacheDir"]+"/"+(char)i1+"/"+(char)i2).c_str(), 0755);
			for(int i3=48; i3<=112; i3++)
			{
				if(i3==58)
					i3=97;
				mkdir((_data["cacheDir"]+"/"+(char)i1+"/"+(char)i2+"/"+(char)i3).c_str(), 0755);
			}
		}
	}
}

config::~config()
{
	//remove pid file
	if(remove(_data["pidFile"].c_str())!=0)
	{
		if(_logError)
		{
			_logError->add("ERROR. Pid file can't be removed: "+_data["pidFile"], 0);
		}
	}
}

std::string config::get(std::string param)
{
	return _data[param];
}

void config::setLog(class myLog *log, class myLog *logError)
{
	_log=log;
	_logError=logError;
}


