#include "TopServer.h"
#define MAX_CLIENT 10
TopServer::TopServer() {
    //��������׽���Э����/����ipv4��Ȼ���ʽ����ʽ(��ʽ����tcp)��Ȼ����tcpЭ��
    slisten = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (slisten == SOCKET_ERROR) {
        printf("�����׽��ִ���ʧ��\n");
        return;
    }
    FD_ZERO(&fd_read);//��ʼ��fd_read
}
TopServer::~TopServer() {
    closesocket(slisten);
    for (auto x : vecClientObject) {
        delete x;
        x = NULL;
    }
    delete arrayThread;
    vecClientObject.clear();
}
void TopServer::Bind(unsigned short port) {
    //���׽���
    sockaddr_in sock_in;
    sock_in.sin_family = AF_INET;
    sock_in.sin_port = htons(port);//��С���ֽ���ĳɴ���ֽ���
    sock_in.sin_addr.S_un.S_addr = INADDR_ANY;//ֱ�ӻ�ȡ������IP��ַ
    int ret = bind(slisten, (const sockaddr*)&sock_in, sizeof(sockaddr_in));
    if (ret == SOCKET_ERROR) {
        printf("���׽���ʧ��");
        return;
    }
    FD_SET(slisten, &fd_read);
    //printf("�󶨳ɹ�\n");
}
void TopServer::Onlisten() {
        FD_SET fd_tmp = fd_read;
        const timeval tv = { 0,1 };
        int rev = select(NULL, &fd_tmp, NULL, NULL, &tv);//��һ��ʱ���ڰ��������¼����׽��ֶ�ɸѡ����
        for (int i = 0; i < fd_tmp.fd_count; i++) {
            //printf("%d\n", rev);
            if (fd_tmp.fd_array[i] == slisten) {
                sockaddr_in clientAddr;//���յ�ַ
                int nlen = sizeof(sockaddr);
                SOCKET sClient = accept(slisten, (sockaddr*)&clientAddr, &nlen);//�������˿ͻ���
                if (sClient == SOCKET_ERROR) {
                    printf("���տͻ���ʧ��\n");
                    return;
                }
                printf("��ͻ����ѽ�������:%s\n", inet_ntoa(clientAddr.sin_addr));
                FD_SET(sClient, &fd_read);
                ClientObject* clientObject = new ClientObject();
                clientObject->SetClient(sClient);
                vecClientObject.push_back(clientObject);
                ThreadRecv* tmp = arrayThread;
                for (int i = 1; i < ThreadCount; i++) {
                    if (tmp->getmapsize() > arrayThread[i].getmapsize()) {
                        tmp = &arrayThread[i];
                    }
                }
                tmp->addtobuff(clientObject);
                FD_SET(sClient, &fd_read);
            }
        }
}

void TopServer::Listen() {
    if (listen(slisten, 10) == SOCKET_ERROR) {//backlog �����ӵȴ���������ĳ��ȣ���Ϊ5�����ʾ���5����������������
        printf("����ʧ��");
        return;
    }
}

void TopServer::startThread(int ncount) {
    //new/delete����ù��졢����������malloc/free���ᣬ���������޷����㶯̬�����Ҫ��
    arrayThread = new ThreadRecv[ncount];
    ThreadCount = ncount;
    for (int i = 0; i < ncount; i++) {
        arrayThread[i].start(this);
    }
}

void ThreadRecv::start(TopServer* p) {
    topserver = p;
    CreateThread(NULL, NULL, Threadpro, (LPVOID)this, NULL, NULL);
}

int ThreadRecv::getmapsize() {
    return m_mapClient.size();
}

DWORD WINAPI ThreadRecv::Threadpro(LPVOID lp) {
    ThreadRecv* threadRecv = (ThreadRecv*)lp;
    while (true) {
        //������ת�Ƶ�������
        if (!threadRecv->m_vecClientBuffer.empty()) {
            //����
            for (int i = 0; i < threadRecv->m_vecClientBuffer.size(); i++) {
                threadRecv->m_mapClient[threadRecv->m_vecClientBuffer[i]->getSocket()] =
                    threadRecv->m_vecClientBuffer[i];
           }
            threadRecv->m_vecClientBuffer.clear();
            threadRecv->ClientChange = true;
        }
        //�ͻ��˷����仯
        if (threadRecv->ClientChange) {
            int i = 0;
            for (auto mpx :threadRecv->m_mapClient ) {
                threadRecv->m_fd_rev.fd_array[i] = mpx.first;
                i++;
            }
            threadRecv->m_fd_rev.fd_count = i;
        }
        FD_SET fd_tmp = threadRecv->m_fd_rev;
        const timeval tv = { 0,1 };
        int rev = select(NULL, &fd_tmp, NULL, NULL, &tv);//��һ��ʱ���ڰ��������¼����׽��ֶ�ɸѡ����
        for (int i = 0; i < fd_tmp.fd_count; i++) {
                char  buf[1024] = { 0 };
                auto iter = threadRecv->m_mapClient.find(fd_tmp.fd_array[i]);
                if (iter != threadRecv->m_mapClient.end()) {
                    char* clientBuffer = iter->second->getbuff();
                    int lastpos = iter->second->getlastpos();
                    int res = recv(fd_tmp.fd_array[i], clientBuffer+lastpos, 1024-lastpos, 0);
                    lastpos += res;
                    MesHead* phead =(MesHead*)clientBuffer;
                    if (res > 0) {
                        while (lastpos >= sizeof(MesHead)) {//˵����һ�������İ�����ֻ��Ҫ֪��datalen�ͺ���
                            if (lastpos>=phead->datalen)//���˵Ŀǰ��lastpos�������Ϣ���ֽ������Ļ��Ͱ������Ϣ�����
                            {
                                threadRecv->topserver->dealWithData(iter->second,phead);
                                memcpy(clientBuffer, clientBuffer + phead->datalen, lastpos-phead->datalen);
                                lastpos -= phead->datalen;
                            }
                            else {
                                break;
                            }
                        }
                        iter->second->setlastpos(lastpos);
                    }
                    else {
                        printf("%d\n", res);
                        printf("����˶Ͽ�����\n");
                            threadRecv->m_mapClient.erase(fd_tmp.fd_array[i]);
                        threadRecv->ClientChange = true;
                        break;
                    }
                }
        }
    }
}
ThreadRecv::~ThreadRecv() {
    //CloseHandle(m_h);
    WaitForSingleObject(m_h,INFINITE);
}
ThreadRecv::ThreadRecv() {
    ClientChange = true;

}
void ThreadRecv::addtobuff(ClientObject* p) {
    m_vecClientBuffer.push_back(p);
}
void TopServer::showRoom(ClientObject* sClient) {
    MesShowReply mesShowReply;
    mesShowReply.RoomCount = RoomList.size();
    int i = 0;
    for (auto it : RoomList) {
        mesShowReply.roomlist[i].room_id = it.first;
        mesShowReply.roomlist[i].room_count = it.second->getClientSize();
        mesShowReply.roomlist[i].room_max = MAX_CLIENT;
        i++;
    }
    sClient->sendData(&mesShowReply);
}
void TopServer::dealWithData(ClientObject* sClient, MesHead* mesHead) {
    switch (mesHead->MesType) {//���Է��ʸ����еĶ���
    case MES_SHOW:
        printf("������ʾ������\n");
        showRoom(sClient);
        break;
    case MES_TALK: {
        MesTalk* mesTalk = (MesTalk*)mesHead;
        printf("%s\n", mesTalk->getbuf());
        TalkRoom(sClient, (MesHead*)mesTalk);
        break;
    }
    case MES_JOIN: {
        // printf("mesHead->MesType:%d\n", mesHead->MesType);
        MesJoin* mesjoin = (MesJoin*)mesHead;
        printf("���������� %d\n", mesjoin->roomid);
        joinRoom(sClient, mesjoin->roomid);
        break;
    }
    case MES_CREATE:
        printf("���󴴽�������\n");
        createRoom(sClient);
        break;
    case MES_LEAVE:
        leaveRoom(sClient);
        printf("�뿪������\n");
        break;
    default:
        printf("��Ϣ����ʧ��\n");
        printf("%d %d\n", mesHead->MesType, MES_TALK);
        break;
    }
}
void TopServer::sendMes(const char* buf, ClientObject* p) {
    MesTalk mesTalk;
    strcpy(mesTalk.getbuf(), buf);
    p->sendData((MesHead*)&mesTalk);
}
void TopServer::TalkRoom(ClientObject* p,MesHead* mesHead) {
    int roomid = p->getnowRoom();
    if (roomid == -1) {
        printf("Ŀǰ���û�δ����������\n");
        sendMes("�㻹δ��������������\n", p);
        return;
    }
    else {
        RoomList[roomid]->sendToAll(p, mesHead);
    }
}
void TopServer::joinRoom(ClientObject* p, int roomid){
    if (RoomList.count(roomid) == 0) {
        printf("Ŀǰ���û������˲����ڵ�������\n");
        sendMes("�������Ҳ�����\n", p);
        return;
    }
    if (p->getnowRoom() != -1) {
        RoomList[p->getnowRoom()]->rmClient(p);
        if (RoomList[p->getnowRoom()]->getClientSize() == 0) {
            ResMge::getInstance()->recycle(p->getnowRoom());
            RoomList.erase(p->getnowRoom());
        }
    }
    RoomList[roomid]->addClient(p);
    p->setroom(roomid); 
    char s[1024] = "���������ң�";
    int res = sprintf(s + strlen(s), "%d\n", roomid);
    if (res < 0) {
        printf("�ַ���ƴ��������\n");
    }
    printf(s);
    sendMes(s,p);
}
void TopServer::createRoom(ClientObject* p) {
   // printf("1111\n");
    int id = ResMge::getInstance()->getfreeName();
    //printf("2222\n");
    if (id == -1) {
        printf("���������ұ��\n");
        sendMes("��������������\n",p);
        return;
    }
    TalkingRoom* newRoom=new TalkingRoom(id);
    RoomList[id] = newRoom;
    char s[1024] = "���ѳɹ����������ң�";
    int res=sprintf(s+strlen(s),"%d\n",id);
    if (res < 0) {
        printf("�ַ���ƴ��������\n");
    }
    printf(s);
    sendMes(s,p);
    joinRoom(p, id);
}
void TopServer::leaveRoom(ClientObject* p) {
    if (p->getnowRoom() == -1) {
        printf("�˳����󣬸��û�δ�����κ�������\n");
        return;
    }
    RoomList[p->getnowRoom()]->rmClient(p);
    if (RoomList[p->getnowRoom()]->getClientSize() == 0) {
        ResMge::getInstance()->recycle(p->getnowRoom());
        RoomList.erase(p->getnowRoom());
    }
}