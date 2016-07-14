#ifndef RECUPERA_PAG_WEB_LIB_H
#define RECUPERA_PAG_WEB_LIB_H
int params_is_valid(int n_args, char **args_list);
void show_error_message(int error);
int create_socket(struct addrinfo *p);
void config_connection(struct addrinfo *hints);
char *get_request(char *url);
int get_serv_connect_info(char *url, struct addrinfo *hints, \
  struct addrinfo **serv_info);
int get_header(char *file_name);
int write_file(int socket_id, char *file_name);
int download_file(char **args_list);
#endif /* RECUPERA_PAG_WEB_LIB_H */
