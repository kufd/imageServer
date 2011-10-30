all: functions.o log.class.o request.class.o config.class.o imageProcessor.class.o cacheList.class.o imageServer.o imageServer

imageServer: imageServer.o
	g++ -o imageServer imageServer.o libs/functions.o libs/log.class.o libs/request.class.o libs/config.class.o libs/imageProcessor.class.o libs/cacheList.class.o `Magick++-config --cppflags --cxxflags --ldflags --libs` -lpthread -lcrypto -O3

imageServer.o: imageServer.cpp
	g++ -c imageServer.cpp -I/usr/include/ImageMagick
	
functions.o: libs/functions.cpp
	g++ -o libs/functions.o -c libs/functions.cpp
log.class.o: libs/log.class.cpp
	g++ -o libs/log.class.o -c libs/log.class.cpp
request.class.o: libs/request.class.cpp
	g++ -o libs/request.class.o -c libs/request.class.cpp
config.class.o: libs/config.class.cpp
	g++ -o libs/config.class.o -c libs/config.class.cpp
imageProcessor.class.o: libs/imageProcessor.class.cpp
	g++ -o libs/imageProcessor.class.o -c libs/imageProcessor.class.cpp -I/usr/include/ImageMagick
cacheList.class.o: libs/cacheList.class.cpp
	g++ -o libs/cacheList.class.o -c libs/cacheList.class.cpp
