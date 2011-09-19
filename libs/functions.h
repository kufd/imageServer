#ifndef __IMAGE_SERVER_FUNCTIONS
#define __IMAGE_SERVER_FUNCTIONS

#include <string>
#include <sstream>
#include <fstream>
#include <iostream>
#include <cstdlib>
#include <map>
#include <algorithm>
#include <sys/types.h>
#include <sys/stat.h>
#include <cmath>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <openssl/md5.h>
#include <pthread.h>

const std::locale loc_utf8("uk_UA.utf8");
const std::locale loc_c("C");

std::string get_datetime(std::string format, time_t setTime=0);
std::string intToStr(int number, int base=10);
int strToInt(std::string s);
std::string trim(std::string s);
std::string getFileExtension(const std::string &filename);
std::string toLowerStrANSI(std::string s);
std::string md5(const std::string& text);
void exactSleep(int sec);
int sendSocket(int s, const char *buf, int len, int flags);
std::string strReplace(std::string from, std::string to, std::string s);

#endif
