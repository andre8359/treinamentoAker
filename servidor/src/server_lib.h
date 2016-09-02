#ifndef SERVER_LIB_H
#define SERVER_LIB_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <ctype.h>
#include <signal.h>
#include <errno.h>
#include <limits.h>
#include <time.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "request_lib.h"
#include "http_utils.h"
#include "thread_utils.h"
#include "socket_utils.h"
#define ERROR -1
#define SUCCESS 0
#define LOCAL_SOCKET_NAME "/tmp/cacique-local-socket"
#define CONFIG_FILE_PATH "/tmp/cacique.conf"
#define PID_FILE_PATH "/tmp/cacique.pid"

enum states
{
  READY_TO_RECEIVE = 1,
  READY_TO_SEND,
  ENDED_DOWNLOAD,
  ENDED_UPLOAD,
  ENDED_DOWNLOAD_UNCOMPLETED,
  ENDED_UPLOAD_UNCOMPLETED
};

int check_speed_limit(struct request_file* request, long speed_limit);
int receive_request_from_client(const int socket_id,
                                struct request_file **head,
                                struct manager_io **manager,
                                long speed_limit);
int receive_from_client(const int socket_id,
                        struct request_file **head,
                        struct manager_io **manager,
                        long speed_limit);
int request_read(const int socket_id,
                 struct request_file **head,
                 struct manager_io **manager,
                 long speed_limit);
int handle_thread_answer(const int local_socket,
                         struct request_file **head,
                         struct manager_io **manager_client);
int send_to_client(const int socket_id,
                   const long speed_limit,
                   struct manager_io **manager_client,
                   struct request_file **head);
int handle_uncompleted_transf(const int socket_id, struct request_file **head);
int handle_end_upload(const int socket_id, struct request_file **head);
int handle_server_error(const int socket_id, struct request_file **head);
int check_if_is_directory(const char *path);
int change_root_directory(const char *root_directory);
int check_file_ready_to_receive(struct request_file * request);
int check_file_ready_to_send(struct request_file * request);
void close_std_file_desc();
void open_background_process();
int max(const int a, const int b);
int min(const int a , const int b);
long params_is_valid(int argc, char *argv[], long *speed_limit);
int check_if_valid_port(int port);
void calc_if_sec_had_pass(struct request_file **r);
int calc_buf_size(long speed_limit);
int diff_time (struct timeval *result, struct timeval *x, struct timeval *y);
int check_config_params(char *root_directory, long port, long speed_limit);
int read_config_file(char *root_dir, long *port, long *speed_limit);
int write_pid_file();
int write_config_file(long port, long speed_limit);
int change_listen_socket(long *port, long new_port, int *server_socket);
#endif /* SERVER_LIB_H */
