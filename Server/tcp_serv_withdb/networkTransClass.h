/**
 **	文件名：networkTransClass.h
 **	功能：服务端的网络通信类,用于接收客户端传送过来的包并进行解析，将其发送到目标客户端
 **
 **/

#ifndef _NETWORK_TRANSCLASS_H_
#define _NETWORK_TRANSCLASS_H_
#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string>
#include <map>
#include <vector>
#include <iostream>
#include <queue>

#include "DatabaseServer.h"
#include "Sms.h"
#include "Sms_fileServer.h"

#define R_ERROR -1

#ifndef MAXLEN
#define MAXLEN 1000
#endif
#define MaxConnect 100

class NetworkTransClass{
public:
	NetworkTransClass();
	NetworkTransClass(const NetworkTransClass&);
	~NetworkTransClass();

	bool establishConnection();	//初始化连接

	bool run();	//等待连接 - 建立连接 - 收发数据

	bool closeConnection();			//关闭连接
	
	string get_system_time();		//获取系统时间

private:
	struct sockaddr_in serv; //服务端本地地址
	int sockfd;		//服务器socket
	int sockcli;		//客户端socket

    	DatabaseServer dbs;

	int conn_amount ;//总连接数量
/*************************************************************/
	struct Connect{
		int fd;
		int port;
		int tel;
		string number;
		char cli_info[MAXLEN];
	}fd_A[MaxConnect+1];//单个连接属性 想用联系人代替它
/*************************************************************/

	int tcp_serv_init(int port, int backlog);//tcp连接初始化

	int tcp_serv_send(char *buff, int blen);//发送数据

	int tcp_serv_recv(char *buff, int blen);//接收数据

	int tcp_serv_accept();//接收新的连接

	int tcp_serv_close();//关闭连接

    void showclient();//显示当前连接情况

    int get_fd(char* str);//根据接受到的短信获取目标socket

    string creatTel();//注册新号码

    int sendMessage(char *buff);//发送短信 buff：用来保存短信的缓冲区
};

#endif
