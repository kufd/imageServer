#ifndef __IMAGE_SERVER_FUNCTIONS
#define __IMAGE_SERVER_FUNCTIONS

#include <string>
#include <sstream>
#include <fstream>
#include <iostream>
#include <cstdlib>
#include <map>

const std::locale loc_utf8("uk_UA.utf8");
const std::locale loc_c("C");

std::string get_datetime(std::string format);
std::string intToStr(int number);
int strToInt(std::string s);
std::string trim(std::string s);

#endif
