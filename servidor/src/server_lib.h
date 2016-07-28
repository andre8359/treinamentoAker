#ifndef SERVER_LIB_H
#define SERVER_LIB_H
#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/time.h>
#include <signal.h>
#include <errno.h>
#include <limits.h>
#include "request_lib.h"
#include "http_utils.h"
#define ERROR -1
#define SUCCESS 0
#define SERVER_IP "127.0.0.1"
#define BACKLOG 5
#define KILOBYTE 1024
#define MEGABYTE 1024 * KILOBYTE
#define GIGABYTE 1024 * MEGABYTE
#define BUFSIZE  1
int create_socket(const struct sockaddr_in *p);
void config_connection(const long port, struct sockaddr_in *serv_info);
int make_connection(const long port);
int accept_new_connection(const int socket_id);
void vector_cpy (char *dst, const char *src, const int begin, const int length);
int receive_request_from_client(const int socket_id,
  struct request_file **head, long buf_size);
int write_to_client (const int socket_id, struct request_file **head, 
  long buf_size);
int change_root_directory(const char *root_directory);
int check_file_ready_to_send(struct request_file * request);
void close_std_file_desc();
void open_background_process();
void clean_up();
int max(const int a, const int b);
int min(int a , int b);
long params_is_valid(int argc, char *argv[], long *speed_limit);
int calc_if_seg_had_pass(struct request_file *r);
#endif /* SERVER_LIB_H */
