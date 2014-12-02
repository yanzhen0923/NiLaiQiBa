#include <iostream>
#include <string.h>
#include <QDebug>
#include <sys/ioctl.h>
#include "tcpclientclass.h"
using namespace std;

TcpClientClass::TcpClientClass(){

    sockfd = -1;
    telNum = "empty";
    memset(serv_ip, 0, sizeof(serv_ip));
}

TcpClientClass::TcpClientClass(const TcpClientClass&){
}

TcpClientClass::~TcpClientClass(){
}

bool TcpClientClass::reconnect(){

    qDebug() << "reconnect ... ...\n";

    for(int i = 0; i < 10; i ++)
    {
        if(tcp_client_init(serv_ip, 4217, telNum)){
            return true;
        }
    }

    return false;
}

bool TcpClientClass::tcp_client_init(char *addr, int port,string &tn)
{
    strcpy(serv_ip, addr);
    cout << serv_ip << endl;
    char buf[MAXLEN + 9];
    if (NULL == addr)
    {
        return false;
    }
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        qDebug() << ("socket error!\n");
        return false;
    }
    memset((char *)&serv, 0, sizeof(serv));
    serv.sin_family = AF_INET;
    serv.sin_addr.s_addr = inet_addr(addr);
    serv.sin_port = htons(port);
    /***************************************connect 服务端***************************/
    int error = -1, len;
    len = sizeof(int);
    timeval tm;
    fd_set set;
    unsigned long ul = 1;
    ioctl(sockfd, FIONBIO, &ul);//设置为非阻塞模式
    bool ret = false;

    if (connect(sockfd, (struct sockaddr *)&serv, sizeof(serv)) < 0)
    {
        tm.tv_sec = 2;
        tm.tv_usec = 0;
        FD_ZERO(&set);
        FD_SET(sockfd, &set);
        if(select(sockfd+1, NULL, &set, NULL, &tm) > 0)
        {
            getsockopt(sockfd, SOL_SOCKET, SO_ERROR, &error, (socklen_t*)&len);
            if(error == 0)
                ret = true;
            else
                ret = false;
        }
        else
            ret = false;
    }
    else
        ret = true;
    ul = 0;
    ioctl(sockfd, FIONBIO, &ul);
    if(!ret)
    {
        fprintf(stderr, "Cannot Connect the server\n");
        close(sockfd);
        return false;
    }
    fprintf(stderr, "Connected\n");
    /*******************************************************************************/

    if(tcp_client_recv(buf, MAXLEN) < 0)
    {
        qDebug() << ("get server message failed\n");
        return false;
    }
    int cnt;

    cout << "old Tel:" << tn << endl;

    stringstream ss;
    ss << tn;
    ss >> buf;
    cnt = send(sockfd, buf, MAXLEN, 0);
    if (cnt == -1)
    {
        qDebug() << ("send tel number error!\n");
        return false;
    }

    cnt = recv(sockfd, buf, MAXLEN, 0);
    if (cnt == -1)
    {
        qDebug() << ("receive tel number error!\n");
        return false;
    }
    tn.clear();
    stringstream ss2;

    ss2 << buf;
    ss2 >> tn;
    cout << "Tel:" << tn << endl;
    telNum = tn;

    return true;
}

bool TcpClientClass::tcp_client_send(char *buff, int blen)
{
    int cnt;
    qDebug() << ("sending...\n");
        if (NULL == buff || blen <= 0)
    {
        return false;
    }

    cnt = send(sockfd, buff, blen, 0);

    if (cnt == -1)
    {
        qDebug() << ("tcp_client_send data error!\n");
        return false;
    }

    qDebug() << ("success\n");

    return true;
}

bool TcpClientClass::tcp_client_recv(char *buff, int blen)
{
    int cnt;


    if (NULL == buff || blen <= 0)
    {
        return false;
    }
    memset(buff, 0, blen);

    cnt = recv(sockfd, buff, blen, 0);

    if (cnt <= 0)
    {
        return false;
    }

    return true;
}

bool TcpClientClass::tcp_client_close()
{
    if(sockfd > 0)
        close(sockfd);

    return true;
}
