#ifndef REQUEST_LIB_H
#define REQUEST_LIB_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/time.h>
#define PATH_MAX 4096
struct request_file
{
  FILE *fp;
  char *file_name;
  char *header;
  char *request;
  unsigned long header_size_sended;
  unsigned long file_size, sended_size;
  int socket_id, status_request;
  struct request_file *prev, *next;
};
struct request_file* add_request(const int socket_id, 
  struct request_file **head);
struct request_file *rm_request(const int socket_id, struct request_file **head);
struct request_file *search_request(const int socket_id, 
  struct request_file **head);
void free_request_list(struct request_file **head);
void print_request_list(struct request_file **head);
#endif /* REQUEST_LIB_H */
