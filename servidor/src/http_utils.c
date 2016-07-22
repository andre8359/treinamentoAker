#include "http_utils.h"

int find_end_request(char *header)
{ 
  const int found = 0;
  if ((strstr(header, "\r\n\r\n")) != NULL)
    return found;
  else if ((strstr(header,"\n\n")) != NULL)
    return found;
  else if ((strstr(header,"\n\r\n\r")) != NULL)
    return found;
  else
    return -1;
}
static int get_file_name_length(char *request)
{
   char *ch;
   ch = strstr(request,"\n");
   return  (ch - request) - (strlen("GETHTTP/1.1")) - 1;

}
char *get_resquest_info(char *request)
{
  char *file_name = NULL;
  int file_name_length =  get_file_name_length(request);
  if (file_name_length <= 0)
    return NULL;
  file_name = (char *) malloc (file_name_length * sizeof(char));
  sscanf(request,"GET %s HTTP/1.%*[0-9]\r\n\r\n %*[^|]", file_name);
  return file_name;
}
 
