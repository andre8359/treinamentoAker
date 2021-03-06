#ifndef HTTP_UTILS_H
#define HTTP_UTILS_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>
#include <limits.h>
#include "request_lib.h"
#include "server_lib.h"
#include <errno.h>
#define MAX_HEADER_SIZE 4000
enum status_conection
{
  OK = 1,
  CREATED,
  BAD_REQUEST,
  UNAUTHORIZED,
  FORBIDDEN,
  NOT_FOUND,
  CONFLICT,
  INTERNAL_ERROR,
  SERVICE_UNAVAILABLE,
  LAST_STATUS
};
char *find_end_request(char *header);
void check_request_info(struct request_file *request);
char *str_dup(const char *str);
char *make_header(struct request_file *request);
int create_default_response_files();
int set_std_response(struct request_file *r);
#endif /* SERVER_LIB_H */
