#pragma once
#include "SocketInit.h"
#include "MesHead.h"
#define BUFFER_MAXSIZE 1024
class ClientObject {
private:
	SOCKET m_cs;
	char m_buffer[BUFFER_MAXSIZE * 2];
	int lastpos;
	int room_now_in;
public:
	ClientObject();
	virtual ~ClientObject();
	void SetClient(SOCKET sClient);
	SOCKET getSocket();
	char* getbuff();
	int getlastpos();
	void setlastpos(int pos);
	void sendData(MesHead* mesHead);
	void setroom(int id);
	int getnowRoom();
};