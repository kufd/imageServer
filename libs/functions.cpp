#include "functions.h"

std::string get_datetime(std::string format)
{
    time_t ttime=time(NULL);
    time_t* pttime=&ttime;
    struct tm* datetime=localtime(pttime);
    char char_datetime[20];
    strftime(char_datetime, 20, format.c_str(), datetime);
    return char_datetime;
}

std::string intToStr(int number)
{
    std::stringstream str;
    str.imbue(loc_c);
    str<<number;
    return str.str();
}

std::string trim(std::string s)
{
    int end_index;
    while(s[0]=='\n' || s[0]=='\r' || s[0]==' ' || s[0]=='\t')
       s.erase(0,1);
    end_index=s.length()-1;
    while(s[end_index]=='\n' || s[end_index]=='\r' || s[end_index]==' ' || s[end_index]=='\t')
    {
       s.erase(end_index,1);
       end_index=s.length()-1;
    }
    return s;
}

int strToInt(std::string s)
{
	return atoi(s.c_str());
}
