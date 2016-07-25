#include "http_utils.h"
const char *status_conection [] = 
  { "HTTP/1.0 200 OK\r\n",
  "HTTP/1.0 400 Bad Request\r\n",
  "HTTP/1.0 401 Unauthorized\r\n",
  "HTTP/1.0 403 Forbidden\r\n",
  "HTTP/1.0 404 Not Found\r\n",
  "HTTP/1.0 413 Request Entity Too Large\r\n",
  "HTTP/1.0 500 Internal Server Error\r\n",
  "HTTP/1.0 503 Service Unavailable\r\n"
  };
int find_end_request(char *request)
{ 
  const int found = 0;
  if ((strstr(request, "\r\n\r\n")) != NULL)
    return found;
  else if ((strstr(request,"\n\n")) != NULL)
    return found;
  else if ((strstr(request,"\n\r\n\r")) != NULL)
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

static char *get_content_length(const char *file_name, unsigned long *file_size)
{
  struct stat st;
  const int length_value = 12; /*Terabytes 2^12*/
  const int content_len_size = strlen( "Content-Length: \r\n") + length_value;
  char *content_length = (char *) malloc (content_len_size * sizeof(char));
  if (content_length == NULL)
    return content_length;
  stat(file_name, &st);
  if (st.st_size < 0)
  {
    snprintf(content_length, content_len_size,"Content-Length: %d\r\n",0);
    *file_size = 0;    
  }
  else
  {
    snprintf(content_length, content_len_size,"Content-Length: %ld\r\n",
      st.st_size);
    *file_size = st.st_size;
  }
  return content_length;
}
static char *get_date()
{
  const char *weekday[] = { "Sun", "Mon", "Tue", "Thu", "Fri", "Sat"};
  const char *months[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul",
    "Aug", "Sep", "Oct", "Nov", "Dec"};
  const int date_length = strlen("Date : Mon, 25 Jul 2016 15:11:59 GMT\r\n"); 
  char *date = (char *) malloc (date_length * sizeof(char));
  struct tm *local;
  time_t t;
  t = time(NULL);
  local = localtime(&t);
  mktime (local); 
  
  snprintf(date, date_length,"Date: %s, %d %s %d %d:%d:%d GMT\r\n", 
    weekday[local->tm_wday], local->tm_mday, months[local->tm_mon],
    local->tm_year+1900, local->tm_hour, local->tm_min, local->tm_sec);
  return date;
}
char *make_header(const char *file_name, const int status, 
  unsigned long *file_size)
{
  const char content_type[] = "Content-Type: application/octet-stream\r\n";
  const char server[] = "Server: Cacique/1.0\r\n";
  const int end_header_len = 2;
  char *content_length = get_content_length(file_name, file_size);
  char *date = get_date();
  int header_size = strlen(status_conection[status]) + strlen(content_type)+1 \
    + strlen(content_length)+1  + strlen(date)+1 + strlen(server)+1 \
    + end_header_len;
 char *header = (char *) malloc (header_size * sizeof(char));
 snprintf(header, header_size, "%s%s%s%s%s\r\n", status_conection[status],
  content_type, content_length, date,server);
  free(content_length);
  free(date);
  return header;
}
