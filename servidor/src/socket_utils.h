#ifndef SOCKET_UTILS_H
#define SOCKET_UTILS_HH
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/un.h>
#include <stddef.h>
#include <unistd.h>
#define ERROR -1
#define SUCCESS 0
#define SERVER_IP "127.0.0.1"
#define BACKLOG FD_SETSIZE
#define KILOBYTE 1024
#define MEGABYTE 1024 * KILOBYTE
#define GIGABYTE 1024 * MEGABYTE
#define BUFSIZE 8*KILOBYTE
int create_socket(const struct sockaddr_in *p);
void config_connection(const long port, struct sockaddr_in *serv_info);
int make_connection(const long port);
int accept_new_connection(const int socket_id);
int make_named_socket (const char *file_name);
#endif
