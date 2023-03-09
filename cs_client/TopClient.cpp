#include "TopClient.h"
#include "iostream"
#pragma warning(disable:4996)
TopClient::TopClient() {
    //创建客户端套接字
    sClient = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sClient == SOCKET_ERROR) {
        printf("监听套接字创建失败\n");
        return;
    }
}
TopClient::~TopClient() {
    closesocket(sClient);
}
void TopClient::ConnetSever(const char* ip, unsigned short port) {
    //绑定套接字
    sockaddr_in sock_in;
    sock_in.sin_family = AF_INET;
    sock_in.sin_port = htons(port);//将小端字节序改成大端字节序
    sock_in.sin_addr.S_un.S_addr = inet_addr(ip);//客户端去找服务端
    //printf("%s %d\n", ip, port);
    if (connect(sClient, (const sockaddr*)&sock_in, sizeof(sock_in)) == SOCKET_ERROR) {
        printf("连接服务器失败\n");
        return;
    }
    printf("成功链接服务器\n");
}
void TopClient::dealWithData(MesHead* mesHead) {
    switch (mesHead->MesType)
    {
    case MES_SHOW_REPLY: {
        MesShowReply* mesShowReply = (MesShowReply*)mesHead;
        printf("---------房间信息---------\n");
        printf("共有%d个房间\n", mesShowReply->RoomCount);
        for (int i = 0; i < mesShowReply->RoomCount; i++) {
            printf("房间%d   目前人数：%d/%d\n", mesShowReply->roomlist[i].room_id, mesShowReply->roomlist[i].room_count, mesShowReply->roomlist[i].room_max);
        }
        printf("---------显示结束---------\n");
        break;
    }
    case MES_TALK: {
        MesTalk* mesTalk = (MesTalk*)mesHead;
        printf("%s", mesTalk->getbuf());
        break;
    }
    default:
        printf("case error\n");
        break;
    }
}
void TopClient::recvdata() {
    while (1) {
        char buf[1024] = { 0 };
        int res = recv(sClient, buf, 1024, 0);
        if (res > 0) {
            dealWithData((MesHead*)buf);
        }
        else {
            printf("接收数据失败!\n");
            Sleep(1000);
        }
    }
}

void TopClient::sendmes(MesHead* pmesHead) {
    send(sClient, (const char*)pmesHead, pmesHead->datalen, 0);
}
//void TopClient::start() {
//    printf("show talk join exit leave create\n");
//    while (1)
//    {
//        char buf[1024];
//        gets_s(buf, 1024);
//        if (strcmp(buf, "show") == 0) {
//            //显示服务器下有哪些聊天室
//            MesShow messhow = MesShow();
//            sendmes(&messhow);
//            char buf[1024];
//            int res = recv(sClient, buf, 1024, 0);
//            if (res > 0) {
//
//            }
//        }
//        else if (strncmp(buf, "join", strlen("join")) == 0) {
//            //想加入哪个聊天室
//            int r = atoi(buf + strlen("join"));
//            printf("加入聊天室%d\n", r);
//            MesJoin mesjoin(r);
//            sendmes(&mesjoin);
//        }
//        else if (strcmp(buf, "talk") == 0) {
//            //聊天室谈话
//            printf("开始聊天\n");
//            while (1) {
//                MesTalk mestalk;
//                gets_s(mestalk.getbuf(), 1024);
//                if (strcmp(mestalk.getbuf(), "leave") == 0) {
//                    printf("结束聊天\n");
//                    MesLeave mesleave;
//                    sendmes(&mesleave);
//                    break;
//                }
//                // printf("mestalk->type:%d\n", mestalk.MesType);
//                printf("%s\n", mestalk.getbuf());
//                sendmes(&mestalk);
//            }
//        }
//    }
//}