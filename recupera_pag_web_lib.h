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
#define ERROR_INCOMP_COMMAND_LINE -1 
#define ERROR_PARAM_BAD_FORMULATED_URL -2
#define ERROR_PARAM_BAD_FORMULATED_FLAG -3
#define ERROR_PARAM_BAD_FORMULATED_FILENAME -4
#define ERROR_PARAM_BAD_FOUMUALTED_FILE_EXIST -5
#define ERROR_PARAM_BAD_FOUMUALTED_FILE_ACCESS_DENIED -6
#define ERROR_NAME_SERVICE_NOT_KNOW -7
#define ERROR -10
#define ERROR_SERVER_MOVE_PERM 301
#define ERROR_SERVER_MOVE_TEMP 302
#define ERROR_SERVER_BAD_REQUEST 400
#define ERRROR_SERVER_UNAUTHORIZED 401
#define ERROR_SERVER_FORBIDDEN 403
#define ERROR_SERVER_NOT_FOUND 404
#define BUFSIZE 16
int params_is_valid(char *url, char *file_name, int overwrite_flag);
void show_error_message(int error);
int create_socket(struct addrinfo *p);
void config_connection(struct addrinfo *hints);
char *get_request(char *url);
int get_serv_connect_info(char *url, struct addrinfo *hints, \
  struct addrinfo **serv_info);
int write_file(int socket_id, char *file_name);
int download_file(char *url, char *file_name);
#endif /* RECUPERA_PAG_WEB_LIB_H */
