imageServer: imageServer.o
	g++ -o imageServer imageServer.o libs/functions.o libs/log.class.o libs/request.class.o libs/config.class.o libs/imageProcessor.class.o libs/cacheList.class.o `Magick++-config --cppflags --cxxflags --ldflags --libs` -lpthread -lcrypto

imageServer.o: imageServer.cpp
	g++ -c imageServer.cpp -I/usr/include/ImageMagick
