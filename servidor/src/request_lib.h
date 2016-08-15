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

enum method
{
  GET = 1,
  PUT,
  LAST_METHOD
};

struct request_file
{
  int  fd;
  char *file_name;
  char *header;
  char *request;
  long header_size_sended;
  long file_size;
  long transferred_size;
  long transf_last_sec;
  int socket_id;
  int status;
  int method;
  struct timeval last_pack;
  struct request_file *prev;
  struct request_file *next;
};
struct request_file* add_request(const int socket_id,
                                 struct request_file **head);
int rm_request(const int socket_id, struct request_file **head);
struct request_file *search_request(const int socket_id,
                                    struct request_file **head);
void free_request_list(struct request_file **head);
void print_request_list(struct request_file **head);
#endif /* REQUEST_LIB_H */
