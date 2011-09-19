#include "functions.h"

std::string get_datetime(std::string format, time_t setTime)
{
    time_t ttime=!setTime ? time(NULL) : setTime;
    time_t* pttime=&ttime;
    struct tm* datetime=localtime(pttime);
    char char_datetime[20];
    strftime(char_datetime, 20, format.c_str(), datetime);
    return char_datetime;
}

std::string intToStr(int number, int base)
{
    std::stringstream str;
    str.imbue(loc_c);
    if(base==16)
    	str<<std::hex<<number;
    else
    	str<<std::dec<<number;
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

std::string getFileExtension(const std::string &filename)
{
	int loc1=filename.rfind(".");
	if(loc1!= std::string::npos)
	{
		return toLowerStrANSI(filename.substr(loc1+1));
	}
	return "";
}

std::string toLowerStrANSI(std::string s)
{
    transform(s.begin(), s.end(), s.begin(), tolower);
    return s;
}

std::string md5(const std::string& text)
{
	unsigned char hash[16];
	char hashHex[32];
	MD5((unsigned char*)text.c_str(), text.length(), hash);
	sprintf(hashHex, "%.2x%.2x%.2x%.2x%.2x%.2x%.2x%.2x%.2x%.2x%.2x%.2x%.2x%.2x%.2x%.2x",
			hash[0], hash[1], hash[2], hash[3], hash[4], hash[5], hash[6], hash[7],
			hash[8], hash[9], hash[10], hash[11], hash[12], hash[13], hash[14], hash[15]);

	return hashHex;
}

void exactSleep(int sec)
{
	int sec1=sec;
	while(sec>0)
	{
		sec-=sec1-sleep(sec1);
	}
}

int sendSocket(int s, const char *buf, int len, int flags)
{
    int n, attempt=0, total = 0;

    while(total < len)
    {
    	attempt=0;
        while((n=send(s, buf+total, len-total, flags))==-1)
        {
        	usleep(50000);
        	//exactSleep(20);
        	if(++attempt==5)
        	{
        		break;
        	}
        }
        if(n == -1)
        {
        	break;
        }
        total += n;
    }

    return (n==-1 ? -1 : total);
}

std::string strReplace(std::string from, std::string to, std::string s)
{
	int pos1;
	while((pos1 = s.find(from))!=std::string::npos)
	{
		s.replace(pos1, from.size(), to);
	}
	return s;
}
