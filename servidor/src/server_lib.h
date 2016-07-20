#ifndef SERVER_LIB_H
#define SERVER_LIB_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <limits.h>
#include <ctype.h>
#include <syslog.h>
#define ERROR -1
int create_socket(const struct addrinfo *p);
void config_connection(struct addrinfo *hints);
void close_std_file_desc();
void open_background_process();
#endif /* SERVER_LIB_H */
