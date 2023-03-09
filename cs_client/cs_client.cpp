// cs_c.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include "SocketInit.h"
#include "MesHead.h"
#include "TopClient.h"
#pragma warning(disable:4996)
//exit talk join leave create show
DWORD WINAPI ThreadProc(LPVOID lp) {
    TopClient *sClient=(TopClient*)lp;
    printf("show talk join exit leave create\n");
    while (1)
    {
        char buf[1024];
        gets_s(buf, 1024);
        if (strcmp(buf, "show") == 0) {
            //显示服务器下有哪些聊天室
            MesShow messhow = MesShow();
            sClient->sendmes(&messhow);
        }
        else if (strncmp(buf, "join", strlen("join")) == 0) {
            //想加入哪个聊天室
            int r = atoi(buf + strlen("join"));
            printf("加入聊天室%d\n", r);
            MesJoin mesjoin(r);
            sClient->sendmes(&mesjoin);
        }
        else if (strcmp(buf, "talk") == 0) {
            //聊天室谈话
            printf("开始聊天\n");
            while (1) {
                MesTalk mestalk;
                gets_s(mestalk.getbuf(),1024);
                if (strcmp(mestalk.getbuf(), "leave") == 0) {
                    printf("结束聊天\n");
                    MesLeave mesleave;
                    sClient->sendmes(&mesleave);
                    break;
                }
               // printf("mestalk->type:%d\n", mestalk.MesType);
               // printf("%s\n", mestalk.getbuf());
                strcpy(mestalk.getbuf() + strlen(mestalk.getbuf()), "\n");
                sClient->sendmes(&mestalk);
            }
        }
        else if (strcmp(buf, "create") == 0) {
            MesCreate mesCreate;
            sClient->sendmes(&mesCreate);
        }
        else if (strcmp(buf, "exit") == 0) {
            exit(0);
        }
    }
}
int main()
{
    TopClient topClient;
    topClient.ConnetSever("172.18.250.235", 82);
    CreateThread(NULL, NULL, ThreadProc, &topClient, NULL, NULL);
    while (1)
    {
        topClient.recvdata();
    }
    getchar();
    return 0;
}