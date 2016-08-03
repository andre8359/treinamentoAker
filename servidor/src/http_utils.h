#ifndef HTTP_UTILS_H
#define HTTP_UTILS_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>
#include <limits.h>
#include "request_lib.h"
#include <errno.h>
#define MAX_HEADER_SIZE 4000
enum status_conection {
  OK = 1,
  CREATED,
  BAD_REQUEST,
  UNAUTHORIZED,
  FORBIDDEN,
  NOT_FOUND,
  INTERNAL_ERROR,
  SERVICE_UNAVAILABLE
};
int find_end_request(char *header);
int get_resquest_info(struct request_file *request);
char *make_header(const char *file_name, const int status,
                  long *file_size);
int create_default_response_files();
int set_std_response(struct request_file *r);
#endif /* SERVER_LIB_H */
