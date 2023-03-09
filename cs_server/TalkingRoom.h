#pragma once
#include "ClientObject.h"
#include <set>
class TalkingRoom
{
private:
	int roomid;
	std::set<ClientObject*>ClientList;
	std::set<int>s;
public:
	TalkingRoom(int id);
	int getClientSize();
	void addClient(ClientObject* pclient);
	void rmClient(ClientObject* pclient);
	void sendToAll(ClientObject* pclient,MesHead* mesHead);
};

