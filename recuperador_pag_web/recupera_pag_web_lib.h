#ifndef RECUPERA_PAG_WEB_LIB_H
#define RECUPERA_PAG_WEB_LIB_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <limits.h>
#include <ctype.h>
#define ERROR -1
#define SERVER_MOVE_PERM 301
#define SERVER_MOVE_TEMP 302
#define SERVER_BAD_REQUEST 400
#define SERVER_UNAUTHORIZED 401
#define SERVER_FORBIDDEN 403
#define SERVER_NOT_FOUND 404
#define SERVICE_UNAVAILABLE 503
#define SERVER_CONNECTED 200
#define BUFSIZE 256
#define MAX_HEADER_LENGTH 4000
typedef enum
{
  ERROR_INCOMP_COMMAND_LINE = 0,
  ERROR_PARAM_BAD_FORMULATED_URL,
  ERROR_PARAM_BAD_FORMULATED_FLAG,
  ERROR_PARAM_BAD_FORMULATED_FILENAME,
  ERROR_PARAM_BAD_FOUMULATED_FILE_EXIST,
  ERROR_PARAM_BAD_FOUMULATED_FILE_ACCESS_DENIED,
  ERROR_NAME_SERVICE_NOT_KNOW,
  ERROR_SERVER_MOVE_PERM,
  ERROR_SERVER_MOVE_TEMP,
  ERROR_SERVER_BAD_REQUEST,
  ERRROR_SERVER_UNAUTHORIZED,
  ERROR_SERVER_FORBIDDEN,
  ERROR_SERVER_NOT_FOUND,
  ERROR_SERVICE_UNAVAILABLE,
} ERRORS;
int params_is_valid(const char *url, const char *file_name, \
  const int overwrite_flag);
void show_error_message(const int error);
int create_socket(const struct addrinfo *p);
void config_connection(struct addrinfo *hints);
char *get_request(const char *url);
int get_serv_connect_info(const char *url, const struct addrinfo *hints, \
  struct addrinfo **serv_info);
int write_file(const int socket_id,const char *file_name);
int download_file(const char *url,const char *file_name);
#endif /* RECUPERA_PAG_WEB_LIB_H */
