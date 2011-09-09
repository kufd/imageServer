#include "config.class.h"

config::config()
{
	//встановлюю значення за замовчуванням
	_data["log"]="/var/log/imageServer.log";
	_data["logLevel"]="9";
	_data["port"]="1111";
	_data["imagesDir"]="/var/images";
	_data["cacheDir"]="/var/cache";

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
}

config::~config()
{
}

std::string config::get(std::string param)
{
	return _data[param];
}


