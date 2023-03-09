// cs_c.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//
#pragma warning(disable:4996)
#include <iostream>
#include "TopServer.h"
//DWORD WINAPI ThreadProc(LPVOID lp) {
//    SOCKET sClient = *(SOCKET*)lp;
//    printf("与客户端建立链接\n");
//    while (1)
//    {
//        char  buf[1024] = { 0 };
//        int res = recv(sClient, buf, 1024, 0);
//        if (res > 0) {
//            printf("接收到的数据:%s\n", buf);
//        }
//        else {
//            printf("服务端断开链接\n");
//            break;
//        }
//    }
//    closesocket(sClient);
//    return 0;
//}
int main()
{
    //MesTalk test;
    //scanf("%s", test.getbuf());
    //printf("MesTalk datalen:%d MesHead datalen:%d\n", test.datalen, ((MesHead)test).datalen);
    TopServer topServer;
    topServer.Bind(82);
    topServer.Listen();
    topServer.startThread(4);
    while (true)
    {
        topServer.Onlisten();

    }
    getchar();

    return 0;
}