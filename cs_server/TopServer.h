#pragma once

#include "iostream"
#include<vector>
#include <map>
#include "TalkingRoom.h"
#include "ResMge.h"
class TopServer;
class ThreadRecv {//�߳��࣬Ŀ���Ƕ�ּ����߳�������ȫ���Ŀͻ��ˣ��������߳�ѹ��
private:
	HANDLE m_h;
	std::vector<ClientObject*>m_vecClientBuffer;//�������У���������������̵߳Ŀͻ����ȷŽ���
	std::map<SOCKET, ClientObject*>m_mapClient;
	int ClientChange;
	fd_set m_fd_rev;
	TopServer* topserver;
public:
	ThreadRecv();
	~ThreadRecv();
	static DWORD WINAPI Threadpro(LPVOID lp);//�̺߳��������Ǿ�̬����
	void start(TopServer* p);//�����̺߳���
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
	void Bind(unsigned short port);//�󶨶˿ں�
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

