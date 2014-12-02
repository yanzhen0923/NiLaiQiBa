#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "tcp_client.h"

int main(int argc, char **argv)
{
	char buf[1010];

	strcpy(buf, "127.0.0.1");
	tcp_client_init(buf, 4217);
	tcp_client_recv(buf, 1009);

	strcpy(buf, "quit");
	printf("close server...", buf);
	tcp_client_send(buf, 1009);
	tcp_client_recv(buf, 1009);
	printf("%s\n", buf);
	tcp_client_close();
	return 0;
}
