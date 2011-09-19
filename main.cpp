/*
 * main.cpp
 *
 *  Created on: 13 лип. 2011
 *      Author: kufd
 */

#include "main.h"

//global variables
class config config;
class myLog myLog(config.get("log"), strToInt(config.get("logLevel")));

void *serveSocket(void *ptr)
{
	int socket = (int) ptr;
	char buf[1025];
	int bytesRead;
	std::string requestString="";
	class imageProcessor imageProcessor(&config, &myLog);

	// Поступили данные от клиента, читаем их
	while((bytesRead = recv(socket, buf, 1024, 0))>0)
	{
		buf[bytesRead]='\0';
		requestString+=buf;
	}

	//create request object
	class request request(requestString);

	imageProcessor.setRequest(&request);

	//send answer into socket
	imageProcessor.writeIntoSocket(socket);

	close(socket);
}


/**
 * There is in thread clearing cacheList
 */
void *clearCacheList(void *ptr)
{
	while(true)
	{
		//clear cacheList
		class cacheList cacheList(&config, &myLog);
		cacheList.clear();

		//sleep for a day
		exactSleep(24*60*60);
	}
}



int main()
{
	int listener;
	struct sockaddr_in addr;
	std::set<int> clients;

	//go to daemon mode
	if(config.get("daemon")=="yes")
	{
		if(daemon(0,0)==-1)
		{
			myLog.add("ERROR. Can't set daemon mode.", 0);
			exit(1);
		}
	}

	myLog.add("Starting server.", 0);

	//starting clearing cacheList thread
	pthread_t threadClearingCacheList;
	pthread_create(&threadClearingCacheList, NULL, clearCacheList, NULL);

	listener = socket(AF_INET, SOCK_STREAM, 0);
	if(listener < 0)
	{
		perror("socket");
		exit(1);
	}

	fcntl(listener, F_SETFL, O_NONBLOCK);

	addr.sin_family = AF_INET;
	addr.sin_port = htons(strToInt(config.get("port")));
	addr.sin_addr.s_addr = INADDR_ANY;
	if(bind(listener, (struct sockaddr *)&addr, sizeof(addr)) < 0)
	{
		perror("bind");
		exit(2);
	}

	listen(listener, 2);

	clients.clear();

	while(1)
	{
		// Заполняем множество сокетов
		fd_set readset;
		FD_ZERO(&readset);
		FD_SET(listener, &readset);

		for(std::set<int>::iterator it = clients.begin(); it != clients.end(); it++)
			FD_SET(*it, &readset);

		// Задаём таймаут
		timeval timeout;
		timeout.tv_sec = 15;
		timeout.tv_usec = 0;

		// Ждём события в одном из сокетов
		int mx = std::max(listener, *max_element(clients.begin(), clients.end()));
		if(select(mx+1, &readset, NULL, NULL, &timeout) <= 0)
		{
			continue;
			//perror("select");
			//exit(3);
		}

		// Определяем тип события и выполняем соответствующие действия
		if(FD_ISSET(listener, &readset))
		{
			// Поступил новый запрос на соединение, используем accept
			int sock = accept(listener, NULL, NULL);
			if(sock < 0)
			{
				perror("accept");
				exit(3);
			}

			fcntl(sock, F_SETFL, O_NONBLOCK);

			clients.insert(sock);
		}

		for(std::set<int>::iterator it = clients.begin(); it != clients.end(); it++)
		{
			if(FD_ISSET(*it, &readset))
			{
				//Видаляю сокет зі списку
				clients.erase(it);

				//Викликаємо в окремому потоці функцію обробки з’єднання
				pthread_t thread;
				pthread_create(&thread, NULL, serveSocket, (void*) *it);
				//pthread_join(thread, NULL);
			}
		}

		//std::cout<<"clients: "<<clients.size()<<std::endl;
	}

	//wait finish of clearing cache thread
	pthread_join(threadClearingCacheList, NULL);

	myLog.add("Stopping server.", 0);

	return 0;
}
