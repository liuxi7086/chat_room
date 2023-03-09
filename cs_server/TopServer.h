#pragma once

#include "iostream"
#include<vector>
#include <map>
#include "TalkingRoom.h"
#include "ResMge.h"
class TopServer;
class ThreadRecv {//线程类，目的是多分几个线程来处理全部的客户端，减少主线程压力
private:
	HANDLE m_h;
	std::vector<ClientObject*>m_vecClientBuffer;//阻塞队列，将服务器给这个线程的客户端先放进来
	std::map<SOCKET, ClientObject*>m_mapClient;
	int ClientChange;
	fd_set m_fd_rev;
	TopServer* topserver;
public:
	ThreadRecv();
	~ThreadRecv();
	static DWORD WINAPI Threadpro(LPVOID lp);//线程函数必须是静态函数
	void start(TopServer* p);//启动线程函数
	int getmapsize();
	void addtobuff(ClientObject* p);
};
class TopServer
{
	SOCKET slisten;
	SocketInit sockInit;
	FD_SET fd_read;
	std::vector<ClientObject*>vecClientObject;
	int ThreadCount;
	ThreadRecv* arrayThread;
	std::map<int,TalkingRoom*>RoomList;
public:
	TopServer();
	void showRoom(ClientObject* sClient);
	virtual ~TopServer();
	void Bind(unsigned short port);//绑定端口号
	void Listen();
	void Onlisten();
	void startThread(int ncount);
	void dealWithData(ClientObject* sClient,MesHead* mesHead);
	void TalkRoom(ClientObject* p, MesHead* mesHead);
	void joinRoom(ClientObject* p, int roomid);
	void sendMes(const char* buf, ClientObject* p);
	void createRoom(ClientObject* t);
	void leaveRoom(ClientObject* p);
	//void sendData(ClientObject* sClient, MesHead* mesHead);
};

