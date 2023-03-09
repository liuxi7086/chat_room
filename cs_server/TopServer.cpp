#include "TopServer.h"
#define MAX_CLIENT 10
TopServer::TopServer() {
    //这个监听套接字协议域/族是ipv4，然后格式是流式(流式面向tcp)，然后是tcp协议
    slisten = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (slisten == SOCKET_ERROR) {
        printf("监听套接字创建失败\n");
        return;
    }
    FD_ZERO(&fd_read);//初始化fd_read
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
    //绑定套接字
    sockaddr_in sock_in;
    sock_in.sin_family = AF_INET;
    sock_in.sin_port = htons(port);//将小端字节序改成大端字节序
    sock_in.sin_addr.S_un.S_addr = INADDR_ANY;//直接获取本机的IP地址
    int ret = bind(slisten, (const sockaddr*)&sock_in, sizeof(sockaddr_in));
    if (ret == SOCKET_ERROR) {
        printf("绑定套接字失败");
        return;
    }
    FD_SET(slisten, &fd_read);
    //printf("绑定成功\n");
}
void TopServer::Onlisten() {
        FD_SET fd_tmp = fd_read;
        const timeval tv = { 0,1 };
        int rev = select(NULL, &fd_tmp, NULL, NULL, &tv);//在一秒时间内把有网络事件的套接字都筛选出来
        for (int i = 0; i < fd_tmp.fd_count; i++) {
            //printf("%d\n", rev);
            if (fd_tmp.fd_array[i] == slisten) {
                sockaddr_in clientAddr;//接收地址
                int nlen = sizeof(sockaddr);
                SOCKET sClient = accept(slisten, (sockaddr*)&clientAddr, &nlen);//监听到了客户端
                if (sClient == SOCKET_ERROR) {
                    printf("接收客户端失败\n");
                    return;
                }
                printf("与客户端已建立连接:%s\n", inet_ntoa(clientAddr.sin_addr));
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
    if (listen(slisten, 10) == SOCKET_ERROR) {//backlog 是连接等待队列请求的长度，若为5，则表示最多5个连接请求进入队列
        printf("监听失败");
        return;
    }
}

void TopServer::startThread(int ncount) {
    //new/delete会调用构造、析构函数，malloc/free不会，所以他们无法满足动态对象的要求。
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
        //生产者转移到消费者
        if (!threadRecv->m_vecClientBuffer.empty()) {
            //加锁
            for (int i = 0; i < threadRecv->m_vecClientBuffer.size(); i++) {
                threadRecv->m_mapClient[threadRecv->m_vecClientBuffer[i]->getSocket()] =
                    threadRecv->m_vecClientBuffer[i];
           }
            threadRecv->m_vecClientBuffer.clear();
            threadRecv->ClientChange = true;
        }
        //客户端发生变化
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
        int rev = select(NULL, &fd_tmp, NULL, NULL, &tv);//在一秒时间内把有网络事件的套接字都筛选出来
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
                        while (lastpos >= sizeof(MesHead)) {//说明是一个完整的包，我只需要知道datalen就好了
                            if (lastpos>=phead->datalen)//如果说目前的lastpos比这个消息的字节数长的话就把这个消息处理掉
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
                        printf("服务端断开链接\n");
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
    switch (mesHead->MesType) {//可以访问父类有的东西
    case MES_SHOW:
        printf("请求显示聊天室\n");
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
        printf("加入聊天室 %d\n", mesjoin->roomid);
        joinRoom(sClient, mesjoin->roomid);
        break;
    }
    case MES_CREATE:
        printf("请求创建聊天室\n");
        createRoom(sClient);
        break;
    case MES_LEAVE:
        leaveRoom(sClient);
        printf("离开聊天室\n");
        break;
    default:
        printf("消息解析失败\n");
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
        printf("目前该用户未加入聊天室\n");
        sendMes("你还未加入任意聊天室\n", p);
        return;
    }
    else {
        RoomList[roomid]->sendToAll(p, mesHead);
    }
}
void TopServer::joinRoom(ClientObject* p, int roomid){
    if (RoomList.count(roomid) == 0) {
        printf("目前该用户加入了不存在的聊天室\n");
        sendMes("该聊天室不存在\n", p);
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
    char s[1024] = "加入聊天室：";
    int res = sprintf(s + strlen(s), "%d\n", roomid);
    if (res < 0) {
        printf("字符串拼接有问题\n");
    }
    printf(s);
    sendMes(s,p);
}
void TopServer::createRoom(ClientObject* p) {
   // printf("1111\n");
    int id = ResMge::getInstance()->getfreeName();
    //printf("2222\n");
    if (id == -1) {
        printf("已无聊天室编号\n");
        sendMes("聊天室数量已满\n",p);
        return;
    }
    TalkingRoom* newRoom=new TalkingRoom(id);
    RoomList[id] = newRoom;
    char s[1024] = "你已成功创建聊天室：";
    int res=sprintf(s+strlen(s),"%d\n",id);
    if (res < 0) {
        printf("字符串拼接有问题\n");
    }
    printf(s);
    sendMes(s,p);
    joinRoom(p, id);
}
void TopServer::leaveRoom(ClientObject* p) {
    if (p->getnowRoom() == -1) {
        printf("退出错误，该用户未加入任何聊天室\n");
        return;
    }
    RoomList[p->getnowRoom()]->rmClient(p);
    if (RoomList[p->getnowRoom()]->getClientSize() == 0) {
        ResMge::getInstance()->recycle(p->getnowRoom());
        RoomList.erase(p->getnowRoom());
    }
}