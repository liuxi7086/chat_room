#pragma once
#ifndef _SOCKET_INIT_H_
#define _SOCKET_INIT_H_
#include<WinSock2.h>
#include<stdio.h>
#pragma comment(lib,"ws2_32.lib")
class SocketInit {
public :
	SocketInit() {
		WORD socketVersion = MAKEWORD(2, 2);//(a,b)һ��16λ�ֽڣ���8λ��b���Ͱ�λ��a������������514=512+2
		WSADATA wasData;
		if (WSAStartup(socketVersion, &wasData)!=0) {//����ʧ��
			printf("��̬���ӿ����ʧ��");
		}
	}
	~SocketInit() {
		WSACleanup();
	}
};

#endif // 
