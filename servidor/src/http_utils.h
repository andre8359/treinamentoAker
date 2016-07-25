#ifndef HTTP_UTILS_H
#define HTTP_UTILS_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>
#define OK 0
#define BAD_REQUEST 1
#define UNAUTHORIZED 2
#define FORBIDDEN 3
#define NOT_FOUND 4
#define REQUEST_TOO_LARGE 5
#define INTERNAL_ERROR 6
#define SERVICE_UNAVAILABLE 7
#define MAX_HEADER_SIZE 4000
int find_end_request(char *header);
char *get_resquest_info(char *request);
char *make_header(const char *file_name, const int status, 
  unsigned long *file_size);
#endif /* SERVER_LIB_H */
