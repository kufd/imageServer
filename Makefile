main: main.o
	g++ -o main main.o libs/functions.o libs/log.class.o libs/request.class.o libs/config.class.o -lpthread

main.o: main.cpp
	g++ -c main.cpp

