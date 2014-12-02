#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

//#define R_OK	0
#define R_ERROR -1

static struct sockaddr_in serv;
static int sockfd;

int tcp_client_init(char *addr, int port)
{
	if (NULL == addr)
	{
	    return R_ERROR;
	}
	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		printf("socket error!\n");
		return R_ERROR;
	}
	bzero((char *)&serv, sizeof(serv));
	serv.sin_family = AF_INET;
	serv.sin_addr.s_addr = inet_addr(addr);
	serv.sin_port = htons(port);

	if (connect(sockfd, (struct sockaddr *)&serv, sizeof(serv)) < 0)
	{
		printf("connection error!\n");
		return R_ERROR;
	}
	return R_OK;
}

int tcp_client_send(char *buff, int blen)
{
	int cnt;

    if (NULL == buff || blen <= 0)
	{
	    return R_ERROR;
	}
	cnt = send(sockfd, buff, blen, 0);
	
	if (cnt == -1)
	{
		printf("tcp_client_send data error!\n");
		return R_ERROR;
	}

	return cnt;
}

int tcp_client_recv(char *buff, int blen)
{
	int cnt;
	
	if (NULL == buff || blen <= 0)
	{
	    return R_ERROR;
	}
	bzero(buff, blen);
	cnt = recv(sockfd, buff, blen, 0);

	if (cnt < 0)
	{
		printf("tcp_client_recv data error!\n");
	}

	return cnt;
}

int tcp_client_close()
{
	close(sockfd);

	return R_OK;
}
