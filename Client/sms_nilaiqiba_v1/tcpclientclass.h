/***********************************************************
文件名：tcp_client.h
功能：客户端数据传输类 实现连接的初始化，接收，发送和关闭
***********************************************************/

#ifndef _TCPCLIENT_H_
#define _TCPCLIENT_H_
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fstream>
#include <sstream>
#include <iostream>
#include <string>
using namespace std;

#define R_ERROR -1

#ifndef MAXLEN
#define MAXLEN 1000
#endif

class TcpClientClass{

public:
    TcpClientClass();
    TcpClientClass(const TcpClientClass&);
    ~TcpClientClass();

    bool tcp_client_init(char *addr, int port, string &telNum);//addr ip地址字符串头 || port 端口号

    bool tcp_client_send(char *buff, int blen);//buff 数据 || blen 长度

    bool tcp_client_recv(char *buff, int blen);//buff 缓冲区 || 长度

    bool tcp_client_close();//关闭连接

    bool reconnect();//重新连接服务器

    int sockfd;

    string telNum;

private:
    struct sockaddr_in serv;
    char serv_ip[20];


};
#endif
