#ifndef REQUEST_LIB_H
#define REQUEST_LIB_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/time.h>
#include <time.h>
#define PATH_MAX 4096
#define ERROR -1
#define SUCCESS 0
#define KILOBYTE 1024
#define BUFSIZE 8*KILOBYTE

enum method
{
  GET = 1,
  PUT,
  LAST_METHOD
};

struct request_file
{
  int socket_id;
  int  fd;
  int status;
  int method;
  char *buffer;
  char *file_name;
  char *header;
  char *request;
  long header_size_sended;
  long file_size;
  long transferred_size;
  long transf_last_sec;
  long sended_last_pack;
  struct timeval last_pack;
  struct request_file *prev;
  struct request_file *next;
};

struct request_io
{
  int socket_id;
  int fd;
  int method;
  char *buffer;
  long offset;
  long size;
  struct request_io *next;
};

struct manager_io
{
  int total_request;
  struct request_io *head;
  int local_socket;
  int quit;
};

struct request_file *add_request_file(const int socket_id,
                                      const int buffer_size,
                                      struct request_file **head);
int rm_request_file(const int socket_id, struct request_file **head);
struct request_file *search_request_file(const int socket_id,
                                    struct request_file **head);
void free_request_file_list(struct request_file **head);
void print_request_file_list(struct request_file **head);

struct request_io *enqueue_request_io(struct manager_io **manager,
                                      struct request_io *request);
struct request_io *dequeue_request_io(struct manager_io **maneger);

struct request_io *dequeue_request_io_with_small_offset(const int socket_id,
                                                        struct manager_io
                                                          **manager);

struct request_io *dequeue_request_io_with_socket_id(const int socket_id,
                                                     struct manager_io
                                                     **manager);
void free_request_io(struct request_io **request);
void free_request_io_list(struct manager_io **manager);
void rm_request_io(const int socket_id, struct manager_io **manager);
#endif /* REQUEST_LIB_H */
