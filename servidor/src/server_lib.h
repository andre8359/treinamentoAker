#ifndef SERVER_LIB_H
#define SERVER_LIB_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <ctype.h>
#define ERROR -1
#define SERVER_IP "127.0.0.1"
#define BACKLOG 5
#define BUFSIZE 256
int create_socket(const struct sockaddr_in *p);
void config_connection(const char *port, struct sockaddr_in *serv_info);
int wait_connection(const char *port);
void close_std_file_desc();
void open_background_process();
#endif /* SERVER_LIB_H */
