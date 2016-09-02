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
#include <errno.h>
#define ERROR -1
#define SUCCESS 0
#define CLIENT_LOCAL_SOCKET_NAME "/tmp/client-socket"
#define SERVER_LOCAL_SOCKET_NAME "/tmp/server-socket"
#define BACKLOG FD_SETSIZE
int create_socket(int family, int type);
void config_connection(const long port, struct sockaddr_in *serv_info);
int make_listening_socket(const long port);
int accept_new_connection(const int socket_id);
int make_local_socket(char *socket_name);
#endif
