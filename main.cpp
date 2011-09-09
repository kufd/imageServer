/*
 * main.cpp
 *
 *  Created on: 13 лип. 2011
 *      Author: kufd
 */

#include "main.h"

//global variables
config config;
log log(config.get("log"), strToInt(config.get("logLevel")));

void *serveSocket(void *ptr)
{
	int socket = (int) ptr;
	char buf[1025];
	int bytesRead;
	std::string requestString="";

	log.add("start: "+intToStr(socket), 0);

	// Поступили данные от клиента, читаем их
	while((bytesRead = recv(socket, buf, 1024, 0))>0)
	{
		buf[bytesRead]='\0';
		requestString+=buf;
	}

	//create request object
	request request(requestString);

	/*
	std::map<std::string, std::string> d = request.params();
	requestString += "\n\n\n\n\n\n\n\n";
	for(std::map<std::string, std::string>::iterator it = d.begin(); it != d.end(); it++)
	{
		requestString += it->first+" = "+it->second+"\n\n\n";
	}


	//Отправляем данные обратно клиенту
	send(socket, requestString.c_str(), requestString.size(), 0);
	 */

	close(socket);

	log.add("stop: "+intToStr(socket), 0);
}



int main()
{
	int listener;
	struct sockaddr_in addr;
	std::set<int> clients;

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
			perror("select");
			exit(3);
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
			}
		}

		//std::cout<<"clients: "<<clients.size()<<std::endl;
	}

	return 0;
}
