/*!
 * \file   server_lib.c
 * \brief  Arquivo de implementação das funcoes para o recupe
 * \date 18/07/2016
 * \author Andre Dantas <andre.dantas@aker.com.br>
 */
#include "http_utils.h"

/* Resposta padrao */
const char *status_conection[] =
{
  "HTTP/1.0 200 OK\r\n",
  "HTTP/1.0 201 Created\r\n",
  "HTTP/1.0 400 Bad Request\r\n",
  "HTTP/1.0 401 Unauthorized\r\n",
  "HTTP/1.0 403 Forbidden\r\n",
  "HTTP/1.0 404 Not Found\r\n",
  "HTTP/1.0 409 Conflict\r\n",
  "HTTP/1.0 500 Internal Server Error\r\n",
  "HTTP/1.0 503 Service Unavailable\r\n"
};

/* Mensagens de respota padrao*/
const char *messages_status[] =
{
  "<h1> 200 - OK </h1>",
  "<html>\n"
    " <body>\n"
    "  <h1>Created</h1>\n"
    "  <p>O arquivo foi criado com sucesso!</p>\n"
    " </body>\n"
    "</html>\n",
  "<html>\n"
    " <body>\n"
    "  <h1>Bad Request</h1>\n"
    "  <p>Este servidor nao entendeu sua requisicao.</p>\n"
    " </body>\n"
    "</html>\n",
  "<h1> 401 - Unauthorized</h1>\n",
  "<h1> 403 - Forbidden</h1>\n",
  "<html>\n"
    " <body>\n"
    "  <h1>404 - Not Found</h1>\n"
    "  <p>A URL requisitada nao pode ser encontrada nesse servidor.</p>\n"
    " </body>\n"
    "</html>\n",
  "<h1> 409 - Conflict</h1>\n",
  "<h1> 500 - Internal Server Error</h1>\n",
  "<h1> 503 - Service Unavailable</h1>\n"
};

/*Nomes do arquivos com html das respostas padao.*/
const char  *std_response_file_names[] =
{
  "ok.html",
  "created.html",
  "bad_request.html",
  "unauthorized.html",
  "forbidden.html",
  "not_found.html",
  "conflict.html",
  "internal_error.html",
  "service_unavailable.html",
};
/* Headers das funcoes estaticas */
static long get_content_length(struct request_file *request);
static int get_file_name(char *input_path);
static void get_request_info(struct request_file *request);
static char *set_content_type(const char *file_name);
static char *set_content_length(const char *file_name, long *file_size);
static char *get_date();
/*!
 * \brief Encontra fim de requisicao.
 * \param[in] request String que representa a requisicao.
 *  do socket.
 * \return Retona o descritor do socket em caso de sucesso ou ERROR em caso de
 *  falha.
 */
char *find_end_request(char *request)
{
  char *ch = NULL;

  if (request == NULL)
    return NULL;

  if ((ch = strstr(request, "\r\n\r\n")) != NULL)
    return ch + 4;
  else if ((ch = strstr(request,"\n\n")) != NULL)
    return ch + 2;
  else if ((ch = strstr(request,"\n\r\n\r")) != NULL)
    return ch + 4;

  return NULL;
}

void check_request_info(struct request_file *request)
{
  get_request_info(request);

  if (request->method < GET && request->method > LAST_METHOD)
    goto on_error;

  if (request->file_name == NULL)
    goto on_error;

  if (request->file_size < 0 
      || (request->method == PUT && request->file_size == 0))
    goto on_error;

  request->status = OK;
  return;

on_error:
  request->status = BAD_REQUEST;
  set_std_response(request);
}
/*!
 * \brief Le as informacoes da requisicao (GET -> nome do arquivo requisitado,
 * PUT -> nome e tamanho do arquivo requisitado.
 * \param p Estrutura que contem as informacoes sobre a requisicao.
 * \return Retona 0 em caso de suceso ou -1 em caso de falha.
 */
static void get_request_info(struct request_file *request)
{
  char file_path[PATH_MAX];
  const int command_size = 5, http_version_size = 10;
  char command[command_size], http_version[http_version_size];
  char real_path_file_name[PATH_MAX];

  memset(file_path, 0, PATH_MAX);
  memset(command, 0, command_size);
  memset(http_version, 0, http_version_size);

  sscanf(request->request,"%4s %s %9s\r\n\r\n %*[^|]",command, file_path,
         http_version);

  if (!strncmp(command, "GET",4))
    request->method  = GET;
  else if (!strncmp(command,"PUT",4))
  {
    request->file_size = get_content_length(request);
    request->method  = PUT;
  }

  if (get_file_name(file_path))
    request->file_name = NULL;
  else
  {
    realpath(file_path + 1, real_path_file_name);
    request->file_name = str_dup(real_path_file_name);
  }
}

/*!
 * \brief Le o valor do content length enviado em uma requisicao.
 * \param[in]  p Estrutura que contem as informacoes sobre a requisicao.
 * \return Retona o valor encontrado.
 * \return ERROR  Em caso de falha.
 */
static long get_content_length(struct request_file *request)
{
  char *ch = NULL , *end = NULL;
  const int base = 10;
  long file_length = 0;
  ch = strstr(request->request, "Content-Length:");
  if (ch == NULL)
    return ERROR;
  ch += strlen("Content-Length: ");

  errno = 0;

  file_length = strtol(ch, &end, base);

  if (errno == ERANGE && (file_length == LONG_MAX || file_length == LONG_MIN
                          || file_length <= 0))
    return ERROR;
  return file_length;
}
static int get_file_name(char *input_path)
{
  if (!strncmp(input_path,"..",3))
    return ERROR;
  else if (*input_path != '/')
    return ERROR;
  if (!strncmp(input_path, "/", 2) || !strncmp(input_path,"/.",3))
  {
    strncpy(input_path, "index.html", strlen("index.html") + 1);
    return SUCCESS;
  }
  else
    return SUCCESS; 
}
char *str_dup(const char *str)
{
  char *dup = calloc(strlen(str) + 1, sizeof(char));

  if (dup != NULL)
    strcpy(dup, str);

  return dup;
}
/*!
 * \brief Seta informacoes da resposta a requisicao como uma das padroes (Bad
 *  Request, Not Foud, etc).
 * \param p Estrutura que contem as informacoes sobre a requisicao.
 * \return Retona 0 em caso de suceso ou -1 em caso de falha.
 */
int set_std_response(struct request_file *r)
{
  struct stat st;
  free(r->file_name);
  r->file_name = NULL;

  if (r->fd > 0)
    close(r->fd);

  r->method = GET;
  r->file_name = str_dup(std_response_file_names[r->status - 1]);

  stat(r->file_name, &st);
  if (st.st_size > 0)
    r->file_size = st.st_size;
  else
  {
    r->file_size = 0;
    return ERROR;
  }
  return SUCCESS;
}
char *make_header(struct request_file *request)
{
  const char server[] = "Server: Cacique/0.0.1\r\n";
  const int end_header_len = 2;
  char *content_length = set_content_length(request->file_name,
                                            &(request->file_size));
  char *date = get_date();
  char *content_type = set_content_type(request->file_name);
  char connection_status[] = "Connection: Close\r\n";
  int header_size = strlen(status_conection[request->status])
    + strlen(content_type) + 1
    + strlen(content_length) + 1
    + strlen(date) + 1
    + strlen(server) + 1
    + end_header_len
    + strlen(connection_status);

  char *header = (char *) calloc (header_size + 1, sizeof(char));

  snprintf(header, header_size, "%s%s%s%s%s%s\r\n",
           status_conection[request->status-1],
           content_type, content_length,connection_status,date,server);

  free(content_length);
  free(date);
  free(content_type);

  return header;
}
static char *set_content_length(const char *file_name, long *file_size)
{
  struct stat st;
  const int length_value = 12; /*Terabytes 2^12*/
  const int content_len_size = strlen( "Content-Length: \r\n") + length_value;
  char *content_length = (char *) calloc (content_len_size, sizeof(char));

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
static char *set_content_type(const char *file_name)
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
static char *get_date()
{
  const char *weekday[] = { "Sun", "Mon", "Tue", "Thu", "Fri", "Sat"};
  const char *months[] = { "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul",
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
int create_default_response_files()
{
  FILE *fp = NULL;
  int i;
  for (i = OK; i < LAST_STATUS; i++)
  {
    fp = fopen(std_response_file_names[i-1], "w");
    if (fp == NULL)
      return ERROR;

    fwrite(messages_status[i-1],sizeof(char),strlen(messages_status[i-1]),fp);
    fclose(fp);
  }

  return SUCCESS;
}
