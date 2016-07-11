/*i
 * \file   main.c
 * \brief  arquivo de implementação do recuperador de paginas WEB
 * \data 11/07/2016
 * \author Andre Dantas <andre.dantas@aker.com.br>
 */


#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "recupera_pag_web_lib.h"
#include <netdb.h>
#include <string.h>

#define PORT 8080

int main(int argc, char *argv[])
{
  int ret;
  int socket_id = 0;
  struct hostent *h;
  struct  addrinfo hints, *servinfo, *p;

 /* ret = params_is_valid(argc, argv);

  if (ret < 0)
  { 
    show_error_message(ret);
    exit(1);    
  }*/
  
  memset(&hints, 0, sizeof(hints)); 
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_PASSIVE;
  
  if ((ret = getaddrinfo("www.example.com", "http", \
    &hints, &servinfo)) != 0) 
  {
    return 0;
  }
   
  for(p = servinfo; p != NULL; p = p->ai_next) 
  {
    if ((socket_id = socket(p->ai_family, p->ai_socktype,
            p->ai_protocol)) == -1) 
    {
      continue;
    }

    if (connect(socket_id, p->ai_addr, p->ai_addrlen) == -1) 
    {
        close(socket_id);
        continue;
    }
  
   break; 
  }
  if (p == NULL)
  {
    printf("Nao foi possivel conectar.\n"); 
    return 0;
  } 
  
  char request[] = "GET sonar.html  HTTP/1.0\r\n\r\n";
  send(socket_id, &request,strlen(request),0);
  char bufin[200];
  printf("%ld",recv(socket_id, &bufin, sizeof(bufin),0));  
  printf("%s",bufin);
}
