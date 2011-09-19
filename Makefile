main: main.o
	g++ -o main main.o libs/functions.o libs/log.class.o libs/request.class.o libs/config.class.o libs/imageProcessor.class.o libs/cacheList.class.o `Magick++-config --cppflags --cxxflags --ldflags --libs` -lpthread -lcrypto

main.o: main.cpp
	g++ -c main.cpp -I/usr/include/ImageMagick
