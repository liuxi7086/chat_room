#include "TopClient.h"
#include "iostream"
#pragma warning(disable:4996)
TopClient::TopClient() {
    //�����ͻ����׽���
    sClient = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sClient == SOCKET_ERROR) {
        printf("�����׽��ִ���ʧ��\n");
        return;
    }
}
TopClient::~TopClient() {
    closesocket(sClient);
}
void TopClient::ConnetSever(const char* ip, unsigned short port) {
    //���׽���
    sockaddr_in sock_in;
    sock_in.sin_family = AF_INET;
    sock_in.sin_port = htons(port);//��С���ֽ���ĳɴ���ֽ���
    sock_in.sin_addr.S_un.S_addr = inet_addr(ip);//�ͻ���ȥ�ҷ����
    //printf("%s %d\n", ip, port);
    if (connect(sClient, (const sockaddr*)&sock_in, sizeof(sock_in)) == SOCKET_ERROR) {
        printf("���ӷ�����ʧ��\n");
        return;
    }
    printf("�ɹ����ӷ�����\n");
}
void TopClient::dealWithData(MesHead* mesHead) {
    switch (mesHead->MesType)
    {
    case MES_SHOW_REPLY: {
        MesShowReply* mesShowReply = (MesShowReply*)mesHead;
        printf("---------������Ϣ---------\n");
        printf("����%d������\n", mesShowReply->RoomCount);
        for (int i = 0; i < mesShowReply->RoomCount; i++) {
            printf("����%d   Ŀǰ������%d/%d\n", mesShowReply->roomlist[i].room_id, mesShowReply->roomlist[i].room_count, mesShowReply->roomlist[i].room_max);
        }
        printf("---------��ʾ����---------\n");
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
            printf("��������ʧ��!\n");
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
//            //��ʾ������������Щ������
//            MesShow messhow = MesShow();
//            sendmes(&messhow);
//            char buf[1024];
//            int res = recv(sClient, buf, 1024, 0);
//            if (res > 0) {
//
//            }
//        }
//        else if (strncmp(buf, "join", strlen("join")) == 0) {
//            //������ĸ�������
//            int r = atoi(buf + strlen("join"));
//            printf("����������%d\n", r);
//            MesJoin mesjoin(r);
//            sendmes(&mesjoin);
//        }
//        else if (strcmp(buf, "talk") == 0) {
//            //������̸��
//            printf("��ʼ����\n");
//            while (1) {
//                MesTalk mestalk;
//                gets_s(mestalk.getbuf(), 1024);
//                if (strcmp(mestalk.getbuf(), "leave") == 0) {
//                    printf("��������\n");
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