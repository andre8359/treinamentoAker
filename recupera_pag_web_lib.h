#ifndef RECUPERA_PAG_WEB_LIB_H
#define RECUPERA_PAG_WEB_LIB_H
int params_is_valid(int n_args, char **args_list);
void show_error_message(int error);
int create_socket(struct addrinfo *p);
void config_connection(struct addrinfo *hints);
void get_request(char *url, char *request, char *root_directory);
int write_file(int socket_id, char *file_name, char *file_param);
int download_file(char **args_list, int num_args);
#endif /* RECUPERA_PAG_WEB_LIB_H */
