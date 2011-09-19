#include "imageProcessor.class.h"


imageProcessor::imageProcessor(class config *config, class myLog *log)
{
	_config = config;
	_log = log;
}

imageProcessor::~imageProcessor()
{
}

void imageProcessor::setRequest(class request *request)
{
	_request = request;
}

void imageProcessor::getProportionSize(int w, int h, int cw, int ch, int &rh, int &rw)
{
	rh=h;
	rw=w;
	if(w==0)
	{
		rw=(int)floor((cw*h)/ch);
		rh=0;
	}
}

void imageProcessor::writeErrorImgIntoSocket(int socket)
{
    Magick::Image img(Magick::Geometry(1,1), "white");
    img.magick("GIF");
    Magick::Blob blob;
    img.write(&blob);
    std::string headers = "HTTP/1.0 200 OK\r\nServer: imageServer\r\nContent-type: image/gif\r\nContent-length: "+intToStr(blob.length())+"\r\nConnection: close\r\n\r\n";
    sendSocket(socket, headers.c_str(), headers.size(), MSG_NOSIGNAL);
    sendSocket(socket, (char*)blob.data(), blob.length(), MSG_NOSIGNAL);
}

bool imageProcessor::writeFileIntoSocket(int socket, const std::string &path)
{
	std::ifstream file(path.c_str(), std::ios::in | std::ios::binary);
	if(file)
	{
		int bufSize = 1048576; //1 M
		char buffer[bufSize];
		std::string headers;

		headers = "HTTP/1.1 200 OK\r\nServer: imageServer\r\nContent-type: image/"+getFileExtension(path)+"\r\nTransfer-Encoding: chunked\r\nConnection: close\r\n\r\n";
		sendSocket(socket, headers.c_str(), headers.size(), MSG_NOSIGNAL);
		while(!file.eof())
		{
			file.read((char *)&buffer, bufSize);
			int count=file.gcount();
			std::string chunkHeader = intToStr(count, 16)+"\r\n";
			sendSocket(socket, chunkHeader.c_str(), chunkHeader.size(), MSG_NOSIGNAL);
			sendSocket(socket, buffer, count, MSG_NOSIGNAL);
			sendSocket(socket, "\r\n", 2, MSG_NOSIGNAL);
		}
		sendSocket(socket, "0\r\n", 3, MSG_NOSIGNAL);
		file.close();

		return true;
	}
	else
	{
		return false;
	}
}

void imageProcessor::writeIntoSocket(int socket)
{
	try
	{
		_log->add("New request. Parameters: "+_request->data("URL"), 7);

		std::string cacheFileName, extension, cacheFilePath, hash;

		if(_request->params("f")=="" && _request->params("fd")=="")
		{
			_log->add("ПОМИЛКА. Не вказаний файл в параметрах: "+_request->data("URL"), 5);
			writeErrorImgIntoSocket(socket);
			return;
		}

		//get hash of url
		hash = _request->params("delete")=="1" ? md5(strReplace("delete=1&", "", strReplace("&delete=1", "", _request->data("URL")))) : md5(_request->data("URL"));
		extension = getFileExtension(_request->params("f"));
		//if we should convert file to some type
		if(_request->params("convertTo")=="gif" || _request->params("convertTo")=="jpg" || _request->params("convertTo")=="jpeg" || _request->params("convertTo")=="png")
			extension=_request->params("convertTo");
		cacheFileName = (std::string)hash+"."+extension;
		cacheFilePath = _config->get("cacheDir")+"/"+hash[0]+"/"+hash[1]+"/"+hash[2]+"/"+cacheFileName;

		//if there are parameter delete == 1 in parameters then we remove file from cache
		if(_request->params("delete")=="1")
		{
			_log->add("Removing file from cache by parameters. File: "+cacheFilePath+". Parameters: "+_request->data("URL"), 5);
			std::string msg="";
			if(remove(cacheFilePath.c_str())!=0)
			{
				_log->add("ERROR. File can't be removed: "+cacheFilePath, 0);
				msg="ERROR. File can't be removed.";
			}
			else
			{
				msg="File was removed.";
			}
			msg="HTTP/1.0 200 OK\r\nServer: imageServer\r\nContent-type: text/plain\r\nContent-length: "+intToStr(msg.size())+"\r\nConnection: close\r\n\r\n"+msg;
			sendSocket(socket, msg.c_str(), msg.size(), MSG_NOSIGNAL);
			return;
		}

		//try to read cached file if there is parameter action in parameters
		if(_request->params("action")!="" && writeFileIntoSocket(socket, cacheFilePath))
		{
			_log->add("File was read from cache: "+cacheFilePath, 5);
		}
		else
		{
			//cacheFile.close(); test it

			std::string sourceFilePath="";

			//testing, does file and alternative file exists?
			if(sourceFilePath=="" && _request->params("f")!="")
			{
				sourceFilePath=_config->get("imagesDir")+"/"+_request->params("f");
				std::ifstream file(sourceFilePath.c_str());
				if(!file)
				{
					sourceFilePath="";
				}
				file.close();
			}
			if(sourceFilePath=="" && _request->params("fd")!="")
			{
				sourceFilePath=_config->get("imagesDir")+"/"+_request->params("fd");
				std::ifstream file(sourceFilePath.c_str());
				if(!file)
				{
					sourceFilePath="";
				}
				file.close();
			}
			if(sourceFilePath=="")
			{
				_log->add("ПОМИЛКА. Не відкриваються файли малюнків. Параметри: "+_request->data("URL"), 5);
				writeErrorImgIntoSocket(socket);
				return;
			}


			if(_request->params("action")=="")
			{
				//if empty action we show source file
				_log->add("Output source file: "+sourceFilePath, 5);
				if(!writeFileIntoSocket(socket, sourceFilePath))
				{
					_log->add("ПОМИЛКА. Не вдалось відкрити файл: "+sourceFilePath+". Параметри: "+_request->data("URL"), 5);
					writeErrorImgIntoSocket(socket);
					return;
				}
			}
			else
			{
				_log->add("Create cache of file: "+sourceFilePath, 5);

				Magick::Image master;
				master.read(sourceFilePath.c_str());
				master.magick(extension);

				//create cache
				if(_request->params("action")=="cut" || _request->params("action")=="1")
				{
					//get parameters
					int bottom = _request->params("bottom")!="" ? strToInt(_request->params("bottom")) : 0;
					int top = _request->params("top")!="" ? strToInt(_request->params("top")) : 0;
					int right = _request->params("right")!="" ? strToInt(_request->params("right")) : 0;
					int left = _request->params("left")!="" ? strToInt(_request->params("left")) : 0;

					if((bottom-top)<=0 || (right-left)<=0)
					{
						_log->add("ПОМИЛКА. Неправильні параметри: "+_request->data("URL"), 5);
						writeErrorImgIntoSocket(socket);
						return;
					}

					master.crop(Magick::Geometry((right-left),(bottom-top),left,top));

					if(toLowerStrANSI(master.magick())=="gif")
						master.page("0x0+0+0");

					if(toLowerStrANSI(master.magick())=="gif" || toLowerStrANSI(master.magick())=="png")
						master.type(Magick::PaletteMatteType);
					else
						master.type(Magick::TrueColorType);

					if(toLowerStrANSI(master.magick())=="gif" || toLowerStrANSI(master.magick())=="png")
						master.backgroundColor("transparent");
				}
				else if(_request->params("action")=="resize" || _request->params("action")=="2")
				{
					//Не працює на даний час
				}
				else if(_request->params("action")=="resize_proportion" || _request->params("action")=="3")
				{
					int w = _request->params("w")!="" ? strToInt(_request->params("w")) : 0;
					int h = _request->params("h")!="" ? strToInt(_request->params("h")) : 0;
					int bs = _request->params("bs")!="" ? strToInt(_request->params("bs")) : 0;
					int sm = _request->params("sm")!="" ? strToInt(_request->params("sm")) : 1;
					std::string align = trim(_request->params("align"))!="" && (trim(_request->params("align"))=="left" || trim(_request->params("align"))=="right" || trim(_request->params("align"))=="center") ? trim(_request->params("align")) : "center";
					std::string valign = trim(_request->params("valign"))!="" && (trim(_request->params("valign"))=="top" || trim(_request->params("valign"))=="bottom" || trim(_request->params("valign"))=="middle") ? trim(_request->params("valign")) : "middle";
					int rw=0, rh=0, left=0, top=0;

					if((w==0 && h==0 && bs==0) || w<0 || h<0 || bs<0)
					{
						_log->add("ПОМИЛКА. Неправильні параметри: "+_request->data("URL"), 5);
						writeErrorImgIntoSocket(socket);
						return;
					}

					if(bs!=0)
					{
						if(master.rows()>master.columns())
						{
							h=bs;
							w=0;
						}
						else
						{
							w=bs;
							h=0;
						}
					}

					if(sm || !sm && (w<(int)master.columns() && w>0  || h<(int)master.rows() && h>0))
					{
						getProportionSize(w, h, (int)master.columns(), (int)master.rows(), rh, rw);
						master.scale(Magick::Geometry(rw,rh));
						if(w>0 && h>0)
						{
							Magick::Image second(Magick::Geometry(w,h), "white");
							if(toLowerStrANSI(master.magick())=="gif" || toLowerStrANSI(master.magick())=="png")
							{
								second.type(Magick::PaletteMatteType);
								second.backgroundColor("transparent");
							}
							else
								second.type(Magick::TrueColorType);

							if(align=="left")
								left=0;
							else if(align=="right")
								left=w-(int)master.columns();
							else if(align=="center")
								left=(int)floor(abs((w-(int)master.columns())/2));

							if(valign=="top")
								top=0;
							else if(valign=="bottom")
								top=h-(int)master.rows();
							else if(valign=="middle")
								top=(int)floor(abs((h-(int)master.rows())/2));


							second.composite(master, left, top);
							master=second;
						}
					}
				}
				else if(_request->params("action")=="cut_resize_proportion" || _request->params("action")=="4")
				{
					int w = _request->params("w")!="" ? strToInt(_request->params("w")) : 0;
					int h = _request->params("h")!="" ? strToInt(_request->params("h")) : 0;
					std::string autocrop = _request->params("autocrop")!="" ? trim(_request->params("autocrop")) : "center";
					int croph=0, cropw=0, cropleft=0, croptop=0, rw=0, rh=0;

					if((w==0 && h==0) || w<0 || h<0 || autocrop!="center")
					{
						_log->add("ПОМИЛКА. Неправильні параметри: "+_request->data("URL"), 5);
						writeErrorImgIntoSocket(socket);
						return;
					}


					if(master.rows()>(int)floor((master.columns()*h)/w))
					{
						cropw=master.columns();
						croph=(int)floor((master.columns()*h)/w);
					}
					else
					{
						cropw=(int)floor((master.rows()*w)/h);
						croph=master.rows();
					}

					if(autocrop=="center")
					{
						cropleft=(int)floor((int)(master.columns()-cropw)/2);
						croptop=(int)floor((int)(master.rows()-croph)/2);
					}

					master.crop(Magick::Geometry(cropw, croph, cropleft, croptop));
					if(toLowerStrANSI(master.magick())=="gif")
						master.page("0x0+0+0");
					getProportionSize(w, h, (int)master.columns(), (int)master.rows(), rh, rw);
					master.scale(Magick::Geometry(rw,rh));
					if(toLowerStrANSI(master.magick())=="gif" || toLowerStrANSI(master.magick())=="png")
					{
						master.type(Magick::PaletteMatteType);
						master.backgroundColor("transparent");
					}
					else
						master.type(Magick::TrueColorType);
				}

				//Роблю світлу рамку по краях малюнка
				int wlb = _request->params("wlb")!="" ? strToInt(_request->params("wlb")) : 0;
				if(wlb>0)
				{
					Magick::Image second(Magick::Geometry(master.columns(),master.rows()), "white");
					if(toLowerStrANSI(master.magick())=="gif" || toLowerStrANSI(master.magick())=="png")
						second.type(Magick::PaletteMatteType);
					else
						second.type(Magick::TrueColorType);
					std::list<Magick::Drawable> drawList;
					Magick::ColorRGB color("#252525");
					drawList.push_back(Magick::DrawableStrokeColor(color)); // Outline color
					drawList.push_back(Magick::DrawableStrokeWidth(wlb)); // Stroke width
					drawList.push_back(Magick::DrawableFillColor("black")); // Fill color
					drawList.push_back(Magick::DrawableRectangle(0,0, master.columns(),master.rows()));
					second.draw(drawList);
					second.composite(master,0,0,Magick::PlusCompositeOp);
					master=second;
					master.fileName(cacheFileName);
				}


				//Накладаю на основний малюнок додаткові
				for(int i=1; true; i++)
				{
					std::string fc = _request->params("fc"+intToStr(i))!="" ? trim(_request->params("fc"+intToStr(i))) : "";
					std::string fcType = _request->params("fcType"+intToStr(i))!="" ? trim(_request->params("fcType"+intToStr(i))) : "plus";
					int fcLeft = _request->params("fcLeft"+intToStr(i))!="" ? strToInt(_request->params("fcLeft"+intToStr(i))) : -1000000;
					int fcTop = _request->params("fcTop"+intToStr(i))!="" ? strToInt(_request->params("fcTop"+intToStr(i))) : -1000000;
					int fcRight = _request->params("fcRight"+intToStr(i))!="" ? strToInt(_request->params("fcRight"+intToStr(i))) : -1000000;
					int fcBottom = _request->params("fcBottom"+intToStr(i))!="" ? strToInt(_request->params("fcBottom"+intToStr(i))) : -1000000;

					if(fc!="")
					{
						Magick::Image second;
						second.read("../"+fc);
						if(fcLeft==-1000000 && fcRight!=-1000000)
							fcLeft=master.columns()-second.columns()-fcRight;
						if(fcTop==-1000000 && fcBottom!=-1000000)
							fcTop=master.rows()-second.rows()-fcBottom;
						if(fcLeft==-1000000)
							fcLeft=0;
						if(fcTop==-1000000)
							fcTop=0;
						if(fcType=="plus")
							master.composite(second, fcLeft ,fcTop, Magick::PlusCompositeOp);
						else if(fcType=="copy")
							master.composite(second, fcLeft ,fcTop, Magick::CopyCompositeOp);
					}
					else
					{
						break;
					}
				}

				int ad = _request->params("ad")!="" ? strToInt(_request->params("ad")) : 15;
				if(ad>0)
				{
					master.write(cacheFilePath);
					class cacheList cacheList(_config, _log);
					cacheList.add(cacheFileName, ad);
				}

				//write file into socket
				Magick::Blob blob;
				master.write(&blob);
				std::string headers = "HTTP/1.0 200 OK\r\nServer: imageServer\r\nContent-type: image/"+extension+"\r\nContent-length: "+intToStr(blob.length())+"\r\nConnection: close\r\n\r\n";
				sendSocket(socket, headers.c_str(), headers.size(), MSG_NOSIGNAL);
				sendSocket(socket, (char*)blob.data(), blob.length(), MSG_NOSIGNAL);
			}
		}
	}
	catch(...)
	{
		_log->add("ПОМИЛКА. Невідома помилка часу виконання. Параметри: "+_request->data("URL"), 5);
		writeErrorImgIntoSocket(socket);
		return;
	}
}
