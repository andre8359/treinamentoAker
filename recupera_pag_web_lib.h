#ifndef RECUPERA_PAG_WEB_LIB_H
#define RECUPERA_PAG_WEB_LIB_H

int params_is_valid(int n_args, char **args_list);
void show_error_message(int error);
int download_file(char ** args_list);
#endif /* RECUPERA_PAG_WEB_LIB_H */
