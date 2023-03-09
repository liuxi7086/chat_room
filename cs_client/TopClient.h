#pragma once

#include "SocketInit.h"
#include "MesHead.h"
class TopClient
{
	SocketInit sockinit;
	SOCKET sClient;
public:
	TopClient();
	virtual ~TopClient();//多态，根据运行时候的对象判断调用子类还是父类函数
	void ConnetSever(const char* ip, unsigned short port);//链接的时候需要ip地址和端口号
	void sendmes(MesHead* pmesHead);
	void recvdata();
	SOCKET getClient();
	void start();
	void dealWithData(MesHead* mesHead);
};

