#ifndef HTTP_UTILS_H
#define HTTP_UTILS_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MAX_HEADER_SIZE 4000
int find_end_request(char *header);
char *get_resquest_info(char *request);
#endif /* SERVER_LIB_H */
