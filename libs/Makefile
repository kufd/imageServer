all: functions.o log.class.o request.class.o config.class.o imageProcessor.class.o cacheList.class.o

functions.o: functions.cpp
	g++ -c functions.cpp
log.class.o: log.class.cpp
	g++ -c log.class.cpp
request.class.o: request.class.cpp
	g++ -c request.class.cpp
config.class.o: config.class.cpp
	g++ -c config.class.cpp
imageProcessor.class.o: imageProcessor.class.cpp
	g++ -c imageProcessor.class.cpp -I/usr/include/ImageMagick
cacheList.class.o: cacheList.class.cpp
	g++ -c cacheList.class.cpp
