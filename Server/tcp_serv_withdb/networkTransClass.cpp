/**

**	文件名：NetworkTransClass.cpp
**	功能：服务端的网络通信类,用于接收客户端传送过来的包并进行解析，将其发送到目标客户端
**
**/
#include "networkTransClass.h"
#include <pthread.h>
using namespace std;
#ifndef TELEN
#define TELEN 11
#endif

NetworkTransClass::NetworkTransClass()
{
    printf("初始化参数\n");
    sockfd = -1;		//服务器socket
    sockcli = -1;		//客户端socket

    conn_amount = 0 ;//总连接数量

    for(int i = 0; i <= MaxConnect; i ++)
    {
        fd_A[i].fd = -1;
        fd_A[i].port = -1;

        fd_A[i].number = "188105774";
	fd_A[i].number.append(1, i / 10 + '0');
        fd_A[i].number.append(1, i % 10 + '0');

        fd_A[i].tel = i;
        fd_A[i].cli_info[0] = 0;
    }
}

NetworkTransClass::NetworkTransClass(const NetworkTransClass& other)
{
}

NetworkTransClass::~NetworkTransClass()
{
}

//建立连接
bool NetworkTransClass::establishConnection()
{
    tcp_serv_init(4217, MaxConnect);
    return true;
}

//显示当前连接
void NetworkTransClass::showclient()
{
    int i;
    printf("----------------show socket----------------\n");
    printf("\tsocket amount: %d\n", conn_amount);
    printf("\tserver:%d  \n", fd_A[0].fd);
    for (i = 1; i <= MaxConnect ; i++)
    {
	if(fd_A[i].fd > 0)
            printf("\tclient[%d]:%d \n", i, fd_A[i].fd);
    }
    printf("-------------------------------------------\n");
    printf("\n\n");
}

int get_num(string tel)
{
	int num = (tel[tel.length() - 2] - '0')* 10 + (tel[tel.length() - 1] - '0');
	return num;
}

int NetworkTransClass::get_fd(char* str)
{

    stringstream ss;
    ss << str;
    Sms newmessage;
    ss >> newmessage;

    string tel= newmessage.get_to();
    int to = get_num(tel);

    if(to >= 0 && to < MAXLEN)
        return fd_A[to].fd;
    else
        return -1;
}



// 建立连接，接收指令，传输数据
bool NetworkTransClass::run()
{
    priority_queue<Sms> que;        //用于处理延时短信的优先队列
    while(!que.empty()) que.pop();

    fd_set fdsr;
    int maxsock;
    struct   timeval   timeout;
    char buf[(MAXLEN+9)];

    memset(buf, '0', sizeof(buf));

    maxsock = sockfd;
    conn_amount = 0;
    fd_A[0].fd = sockfd;

    //接受客户端请求
    while(1)
    {

        FD_ZERO(&fdsr);

        for (int i = 0; i <= MaxConnect ; i++)
        {
            if(fd_A[i].fd > 0)
            {
                FD_SET(fd_A[i].fd, &fdsr);
            }
        }

        //处理定时发送的信息
        string sys_time = get_system_time();
        while(!que.empty()) {
            Sms s = que.top();
            if(s.get_time() > sys_time) {
                break;
            }
            que.pop();
            s.sms_to_cstr(buf);
            sendMessage(buf);
            printf("sending delayed message to: %d  \n", sockcli);
            usleep(100000);
        }

        timeout.tv_sec = 0;
        timeout.tv_usec = 100000;

        int ret = select(maxsock + 1, &fdsr, NULL, NULL, &timeout);

        if( ret < 0 )
        {
            perror("select");
            break;
        }

        if(ret == 0)
        {
            //printf("time out\n");
            continue;
        }

        if(FD_ISSET(sockfd,&fdsr))//服务器接收到新的连接
        {
            if(tcp_serv_accept() == 3)
                break;


            if(sockcli > maxsock)
            {
                maxsock = sockcli;
            }

            continue;
        }

        for(int i=1; i<= MaxConnect; i++) //接收到客户端发送过来的数据
        {
            if(fd_A[i].fd < 0)
                continue;

            if(FD_ISSET(fd_A[i].fd,&fdsr))
            {
                sockcli = fd_A[i].fd;

                if(tcp_serv_recv(buf, MAXLEN) <= 0 )
                {
                    printf("与%s的连接关闭%s:%d\n",fd_A[i].number.c_str(), fd_A[i].cli_info, fd_A[i].port);
                    close(fd_A[i].fd);
                    FD_CLR(fd_A[i].fd,&fdsr);
                    conn_amount--;
                    fd_A[i].fd = -1;
                }
                else
                {
                    tcp_serv_send(buf, MAXLEN);

                    /*********显示短信信息************/
                    Sms rec;
                    rec.cstr_to_sms(buf);
                    rec.set_cont(rec.get_from());
                    rec.sms_to_cstr(buf);
                    //std::cout << rec << std::endl;

                    printf("from: %s(%d)  ||  ", rec.get_from().c_str(), sockcli);
                    /*******************************/

                    /*********解析并发送短信**********/
                    if(rec.get_time() > get_system_time()) {
                        que.push(rec);
                        printf("to: %s(%d)  delayed\n", rec.get_to().c_str(), sockcli);
                        printf("que.size(): %u %s\n", que.size(), ((Sms)(que.top())).get_time().c_str());
                    } else {
			printf("to: %s(%d)\n", rec.get_to().c_str(), get_fd(buf));
                        sendMessage(buf);
                    }
                    /*******************************/

                }
            }
        }
    }
    return false;
}

string NetworkTransClass::creatTel()
{
    string ret = "188105774";
    int num = ++dbs.nexttelnum;
    if(num >= 100)
    {
        printf("Can not creat more client tel\n");
        ret.clear();
        return ret;
    }
    if(num <= 0)
    {
        num = 1;
	dbs.nexttelnum = 2;
    }
    ret.append(1, num / 10 + '0');
    ret.append(1, num % 10 + '0');
    return ret;
}
// 关闭连接
bool NetworkTransClass::closeConnection()
{
    tcp_serv_close();
    return true;
}

/**********************************************************
 **socket 底层
 **********************************************************/
int NetworkTransClass::tcp_serv_init(int port, int backlog)
{
    //检查初始化参数是否合法
    if (port <= 1024 || backlog < 1)
    {
        return -1;
    }

    //创建socket
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("socket");
        return -1;
    }

    // 开启地址复用
    int on = 1;
    if(setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) < 0)
    {
        perror("setsockopt");
        return false;
    }

    //绑定IP
    memset((char *)&serv, 0, sizeof(serv));
    serv.sin_family = AF_INET;
    serv.sin_addr.s_addr = htonl(INADDR_ANY);
    serv.sin_port = htons(port);

    if (bind(sockfd, (struct sockaddr *)&serv, sizeof(serv)) < 0)
    {
        perror("bind");
        return -1;
    }



    // 输出绑定地址信息
    printf("\n绑定IP地址: %s: %d\n\n", inet_ntoa(serv.sin_addr),ntohs(serv.sin_port));


    sockcli = -1;

    //监听
    if (listen(sockfd, backlog) < 0)
    {
        perror("listen");
        return -1;
    }
    return 1;
}

int NetworkTransClass::tcp_serv_accept()
{
    /**********************************************************
    **接受新连接
    **********************************************************/
    static struct sockaddr_in client;

    socklen_t clen;

    clen = sizeof(struct sockaddr);

    sockcli =  accept(sockfd, (struct sockaddr *)&client, &clen);

    if(MaxConnect < conn_amount - 1)
    {
        printf("max connection arrive, refuse connection\n");
        close(sockcli);
        sockcli = -1;
        return -1;
    }


    char buf[(MAXLEN+9)];

    conn_amount ++;
    strcpy(buf, "connect server success");
    tcp_serv_send(buf, MAXLEN);

    /**********************************************************
    **处理客户端电话号码
    **********************************************************/
    tcp_serv_recv(buf, MAXLEN);//接收客户端的电话号码或者是退出命令
    if(strcmp(buf, "quit") == 0)//服务器受控退出
    {
	printf("%s\n", buf);
        tcp_serv_send(buf, MAXLEN);
        return 3;
    }

    //提取接收到的消息
    stringstream ss;
    string telNum;
    ss << buf;
    ss >> telNum;

    if(strcmp(telNum.c_str(), "empty") == 0)  //若客户端需要注册电话号码
    {
        telNum = creatTel();

        if(telNum.length() == 0)
        {
            return -1;
        }
        stringstream ss2;

        ss2 << telNum;
        ss2 >> buf;
    }

    //cout << telNum << endl;//输出电话号码

    int num = get_num(telNum);

    fd_A[num].fd = sockcli;
    fd_A[num].port = ntohs(client.sin_port);
    strcpy(fd_A[num].cli_info, inet_ntoa(client.sin_addr));

    printf("new connection\ttel[%s]%s:%d",fd_A[num].number.c_str(), fd_A[num].cli_info, fd_A[num].port);
    printf("||sockcli: %d\n\n", fd_A[num].fd);
    showclient();
    tcp_serv_send(buf, MAXLEN);

    /**********************************************************
    **检索表单把之前没发出去的短信发出去
    **********************************************************/
    if(dbs.ip_contact.find(telNum) == dbs.ip_contact.end())
    {
        return -1;
    }
	cout << "><" << endl;
    map<string, int> ssms = dbs.get_sms_list(telNum);
    map<string, int>::iterator it;
    Sms msg;

    for(it = ssms.begin(); it != ssms.end(); ++it)
    {
        msg = dbs.get_sms(it->second);
        msg.sms_to_cstr(buf);
        int rt = sendMessage(buf);
        usleep(100000);
        printf("send failed message:%s\n", buf);
        if(rt == -1)
        {
            break;
        }
    }
    cout << "11111" << endl;
    dbs.Delete(telNum);
    return sockcli;
}

int NetworkTransClass::sendMessage(char *buff)//发送短信 buff：用来保存短信的缓冲区
{
    sockcli = get_fd(buff);
    //printf("to: %d\n", sockcli);

    if(sockcli < 0)
    {
        /***************输出失败 把短信存下来***********************/
        Sms unsendsms;
        unsendsms.cstr_to_sms(buff);
        dbs.Add(unsendsms);
        printf("....not online\n");
        return -1;
    }
    if(tcp_serv_send(buff, MAXLEN) <= 0)
    {
        /***************输出失败 把短信存下来***********************/
        Sms unsendsms;
        unsendsms.cstr_to_sms(buff);
        dbs.Add(unsendsms);
        printf("....send failed\n");
        return -1;
    }
    return 1;
}

int NetworkTransClass::tcp_serv_send(char *buff, int blen)
{
    int cnt;

    if (NULL == buff || blen <= 0)
    {
        return -1;
    }
    cnt = send(sockcli, buff, blen, 0);

    if (cnt < 0)
    {
        printf("tcp_serv_send data error[%d]!\n", errno);
        return -1;
    }

    return cnt;
}

int NetworkTransClass::tcp_serv_recv(char *buff, int blen)
{
    int cnt;

    if (NULL == buff || blen <= 0)
    {
        return -1;
    }

    memset(buff, 0, blen);
    cnt = recv(sockcli, buff, blen, 0);

    if (cnt < 0)
    {
        printf("tcp_serv_recv data error!\n");
    }

    return cnt;
}

int NetworkTransClass::tcp_serv_close()
{
    close(sockfd);
    close(sockcli);
    dbs.shut_down();
    return 1;
}

string NetworkTransClass::get_system_time()
{
    string ret("");
    time_t timer;
    struct tm* t_tm;
    timeval tBegin;
    time(&timer);
    t_tm = localtime(&timer);
    gettimeofday(&tBegin, 0);
    ret += m_to_string((unsigned int)t_tm->tm_year + 1900, 4);
    ret += m_to_string((unsigned int)t_tm->tm_mon + 1, 2);
    ret += m_to_string((unsigned int)t_tm->tm_mday, 2);
    ret += m_to_string((unsigned int)t_tm->tm_hour, 2);
    ret += m_to_string((unsigned int)t_tm->tm_min, 2);
    ret += m_to_string((unsigned int)t_tm->tm_sec, 2);
    ret += m_to_string((unsigned int)tBegin.tv_usec, 8);
    return ret;
}
