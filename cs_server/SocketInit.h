#pragma once
#ifndef _SOCKET_INIT_H_
#define _SOCKET_INIT_H_
#include<WinSock2.h>
#include<stdio.h>
#pragma comment(lib,"ws2_32.lib")
class SocketInit {
public :
	SocketInit() {
		WORD socketVersion = MAKEWORD(2, 2);//(a,b)一个16位字节，高8位是b，低八位是a，所以这里是514=512+2
		WSADATA wasData;
		if (WSAStartup(socketVersion, &wasData)!=0) {//创建失败
			printf("动态链接库加载失败");
		}
	}
	~SocketInit() {
		WSACleanup();
	}
};

#endif // 
