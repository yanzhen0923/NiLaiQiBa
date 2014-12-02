int tcp_client_init(char *addr, int port);

int tcp_client_send(char *buff, int blen);

int tcp_client_recv(char *buff, int blen);

int tcp_client_close();
