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
#include <time.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "request_lib.h"
#include "http_utils.h"
#include "thread_utils.h"
#define ERROR -1
#define SUCCESS 0
#define LOCAL_SOCKET_NAME "local.soket"
#define KILOBYTE 1024
#define MEGABYTE 1024 * KILOBYTE
#define GIGABYTE 1024 * MEGABYTE
#define BUFSIZE 8*KILOBYTE
enum states
{
  READY_TO_RECEIVE = 1,
  READY_TO_SEND,
  ENDED_DOWNLOAD,
  ENDED_UPLOAD
};
int receive_request_from_client(const int socket_id,
                                struct request_file **head,
                                long speed_limit);
int receive_from_client(const int socket_id,
                             struct request_file **head,
                             long speed_limit);
int rename_downloaded_file(struct request_file *request);
int send_to_client(const int socket_id,
                   struct request_file **head,
                   long speed_limit);
int change_root_directory(const char *root_directory);
int check_file_ready_to_receive(struct request_file * request);
int check_file_ready_to_send(struct request_file * request);
void close_std_file_desc();
void open_background_process();
void clean_up();
int max(const int a, const int b);
int min(const int a , const int b);
long params_is_valid(int argc, char *argv[], long *speed_limit);
void calc_if_sec_had_pass(struct request_file **r);
int calc_buf_size(long speed_limit);
#endif /* SERVER_LIB_H */
