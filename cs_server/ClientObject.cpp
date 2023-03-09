#include "ClientObject.h"

char* ClientObject::getbuff() {
    return m_buffer;
}
int ClientObject::getlastpos() {
    return lastpos;
}

void ClientObject::setlastpos(int pos) {
    lastpos = pos;
}
ClientObject::ClientObject() {
    room_now_in = -1;
    lastpos = 0;

}
ClientObject::~ClientObject() {
    closesocket(m_cs);
}
void ClientObject::SetClient(SOCKET sClient) {
    m_cs = sClient;
}
SOCKET ClientObject::getSocket() {
    return m_cs;
}
void ClientObject::setroom(int id) {
    room_now_in = id;
}
void ClientObject::sendData(MesHead* mesHead) {
    printf("已发送数据\n");
    send(m_cs, (const char *)mesHead, mesHead->datalen,0);
}
int ClientObject::getnowRoom() {
    return room_now_in;
}