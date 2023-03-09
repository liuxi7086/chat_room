#include "TalkingRoom.h"
TalkingRoom::TalkingRoom(int id) {
	roomid = id;
}
int TalkingRoom::getClientSize() {
	return ClientList.size();
}
void TalkingRoom::addClient(ClientObject* pclient) {
	ClientList.insert(pclient);
	pclient->setroom(roomid);
}
void TalkingRoom::rmClient(ClientObject* pclient) {
	ClientList.erase(pclient);
}
void TalkingRoom::sendToAll(ClientObject* pclient,MesHead* mesHead) {
	for (auto it : ClientList) {
		if (it != pclient) {
			it->sendData(mesHead);
		}
	}
}