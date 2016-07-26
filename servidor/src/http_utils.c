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
const char *messages_status [] = 
  { "",
    "<html>\n"
    " <body>\n"
    "  <h1>Bad Request</h1>\n"
    "  <p>Este servidor nao entendeu sua requisicao.</p>\n"
    " </body>\n"
    "</html>\n",
    "<h1> 401 - Unauthorized</h1>",
    "<h1> 403 - Forbidden</h1>",
    "<html>\n"
    " <body>\n"
    "  <h1>404 - Not Found</h1>\n"
    "  <p>A URL requisitado nao pode ser encontrada nesse servidor.</p>\n"
    " </body>\n"
    "</html>\n",
    "<h1> 413 - Request Entity Too Large</h1>",
    "<h1> 500 - Internal Server Error</h1>",
    "<h1> 503 - Service Unavailable</h1>"
  };
const char  *std_response_file_names[] =
  { "bad_request.html",
    "unauthorized.html",
    "forbidden.html",
    "not_found.html",
    "request_too_large.html",
    "internal_error.html",
    "service_unavailable.html",
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

char *get_resquest_info(struct request_file *request)
{
  char *file_name = NULL, temp[PATH_MAX];
  char command[5], http_version[10]; 
 
   sscanf(request->request,"%4s %s %9s\r\n\r\n %*[^|]",command, temp,
      http_version);
  
  if (strncmp(command,"GET", 3)) 
    request->status_request = BAD_REQUEST;
  else if ((strncmp(http_version,"HTTP/1.0",8)) 
    && (strncmp(http_version,"HTTP/1.1",8)))
    request->status_request = BAD_REQUEST;
  if (request->status_request == BAD_REQUEST)
  {
    set_std_response(request);
    return NULL;
  }
  if (strlen (temp) == 0)
    file_name = strdup("index.html");
  if (*temp == '/')
   file_name = strdup(temp + 1);
  else
  file_name = strdup(temp);

  return file_name;
}
int set_std_response(struct request_file *r)
{
  if (r->status_request != OK)
  {
    free(r->file_name);
    r->file_name = 
      strdup(std_response_file_names[r->status_request - 1]);
    struct stat st;
    stat(r->file_name, &st);
    if (st.st_size > 0)
      r->file_size = st.st_size;
    else
    {  
      r->file_size = 0;
      return -1;
    }
    return 0;
  }
  return 0;
}
static char *get_content_type(const char *file_name)
{
  const char *type[] =
    { "application/octet-stream",
      "text/html",
      "text/plain",
      "image/jpeg",
      "image/png", 
    }; 
    int pos = 0;
    int content_type_len = 0;
    char * content_type = NULL, *ch = NULL;
    ch = strrchr(file_name,'.');
    if(ch == NULL)
      pos = 0;
    else if (!strncmp(ch,".html",5) || !strncmp(ch,".htm",4) 
      || !strncmp(ch,".shtml",6))
      pos = 1;
    else if (!strncmp(ch,".txt",4) || !strncmp(ch,".asc",4)
      || !strncmp(ch,".text",5) || !strncmp(ch,".pot",4) 
      || !strncmp(ch,".brf",4) || !strncmp(ch,".srt",4))
      pos = 2;
    else if ( !strncmp(ch,".jpeg",5) ||  !strncmp(ch,".jpg",4)
      ||  !strncmp(ch,".jpe",4))
      pos = 3;
    else if (!strncmp(ch,".png",4))
      pos = 4;
    else  
      pos = 0;
    content_type_len = strlen("Content-Type:  \r\n ") + strlen(type[pos]) + 1;
    content_type = (char *) malloc(content_type_len * sizeof(char));
    snprintf(content_type,content_type_len,"Content-Type: %s \r\n", type[pos]); 
  return content_type;
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
  const char server[] = "Server: Cacique/1.0\r\n";
  const int end_header_len = 2;
  char *content_length = get_content_length(file_name, file_size);
  char *date = get_date();
  char *content_type = get_content_type(file_name);
  int header_size = strlen(status_conection[status]) + strlen(content_type)+1 \
    + strlen(content_length)+1  + strlen(date)+1 + strlen(server)+1 \
    + end_header_len;
 char *header = (char *) malloc (header_size * sizeof(char));
 snprintf(header, header_size, "%s%s%s%s%s\r\n", status_conection[status],
  content_type, content_length, date,server);
  free(content_length);
  free(date);
  free(content_type);
  return header;
}
int create_default_response_files()
{
  
  FILE *fp = NULL;
  int i;
  for(i = BAD_REQUEST; i <= SERVICE_UNAVAILABLE; i++)
  {
    fp = fopen(std_response_file_names[i-1], "w");
    if (fp == NULL)
      return -1;
    fwrite(messages_status[i],sizeof(char), strlen(messages_status[i]), fp);
    fclose(fp); 
  }
  return 0;    
}
