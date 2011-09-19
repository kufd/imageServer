#include "request.class.h"


request::request(std::string request)
{
	_request=request;
	parseRequest();
	parseGetParams();
}
request::~request()
{
}

//method return data of request
std::map<std::string, std::string> request::data()
{
	return _data;
}

//method returns value from data by key
std::string request::data(std::string key)
{
	return _data[key];
}

//method returns get params of request
std::map<std::string, std::string> request::params()
{
	return _params;
}

//method returns value from params by key
std::string request::params(std::string key)
{
	return _params[key];
}

std::string request::urlDecode(const std::string & s)
{
	std::string buffer = "";
	int len = s.length();

	for (int i = 0; i < len; i++)
	{
	    int j = i ;
	    char ch = s.at(j);
	    if (ch == '%')
	    {
	        char tmpstr[] = "0x0__";
  	        int chnum;
	  	    tmpstr[3] = s.at(j+1);
		    tmpstr[4] = s.at(j+2);
		    chnum = strtol(tmpstr, NULL, 16);
		    buffer += chnum;
		    i += 2;
	    }
        else
	    {
		   buffer += ch;
	    }
	}

	return buffer;
}


void request::parseGetParams()
{
	std::string getData=_data["GET-Params"]+"&";

	int index = 0;
	int len;
	std::string name, value;
	std::map<std::string, std::string> result;

	// This boolean decides between name and value
	// First thing to be parsed is a name
	bool is_name = true;

	len = getData.length();

	while (index < len)
	{
		if ('=' == getData[index])
		{
			// We are parsing a value now
			is_name = false;
		}
		else if ('&' == getData[index])
		{
			// The '&' is a parameter separator, so this is a new one
			result[name]=urlDecode(value);
			name="";
			value="";

			// We are parsing a name now
			is_name = true;
		}
		else
		{
			// This char is part of ...
			if (is_name)
			{
				// ... a name, so append it
				name=name+getData[index];
			}
			else
			{
				// ... a value, so append it
				value=value+getData[index];
			}
		}
		// Move forward in the string str
		index++;
	}
	_params = result;
}

void request::parseRequest()
{
	int i=0, loc1, loc2, loc3;
	std::string s, key, headers;
	std::map<std::string, std::string> result;

	loc1=_request.find("\r\n\r\n",0);
	result["headers"]=_request.substr(0,loc1);
	result["body"]=_request.substr(loc1+4);

	headers=result["headers"]+"\r\n";
	while((loc1=headers.find("\r\n",0))!= std::string::npos)
	{
		i++;
		s=headers.substr(0,loc1);
		headers=headers.substr(loc1+2);

		if(i==1)
		{
			loc1=s.find(" ",0);
			if(loc1!= std::string::npos)
			{
				result["Method"]=trim(s.substr(0,loc1));
				loc2=s.find(" ",loc1+1);
				if(loc1!= std::string::npos)
				{
					result["URL"]=trim(s.substr(loc1+1,loc2-loc1));
					result["HTTP-Version"]=trim(s.substr(loc2+1));
				}
			}
		}
		else
		{
			loc1=s.find(":",0);
			if(loc1!= std::string::npos)
			{
				key=trim(s.substr(0, loc1));
				if(key!="")
				{
					result[key]=trim(s.substr(loc1+1));
				}
			}
		}
	}

	//get getParams
	loc1=result["URL"].find("?",0);
	result["GET-Params"] = (loc1!=std::string::npos) ? result["URL"].substr(loc1+1) : "";

	_data = result;
}
