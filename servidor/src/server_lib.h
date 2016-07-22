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
#include <sys/time.h>
#include "request_lib.h"
#include "http_utils.h"
#define ERROR -1
#define SERVER_IP "127.0.0.1"
#define BACKLOG 5
#define BUFSIZE 16
int create_socket(const struct sockaddr_in *p);
void config_connection(const char *port, struct sockaddr_in *serv_info);
int make_connection(const char *port);
int accept_new_connection(const int socket_id);
void vector_cpy (char *dst, char *src, int begin, int length);
char* receive_request_from_client(const int socket_id);
void write_to_client (struct request_file *r);
void close_std_file_desc();
void open_background_process();
#endif /* SERVER_LIB_H */
