#pragma once

#include "SocketInit.h"
#include "MesHead.h"
class TopClient
{
	SocketInit sockinit;
	SOCKET sClient;
public:
	TopClient();
	virtual ~TopClient();//��̬����������ʱ��Ķ����жϵ������໹�Ǹ��ຯ��
	void ConnetSever(const char* ip, unsigned short port);//���ӵ�ʱ����Ҫip��ַ�Ͷ˿ں�
	void sendmes(MesHead* pmesHead);
	void recvdata();
	SOCKET getClient();
	void start();
	void dealWithData(MesHead* mesHead);
};

