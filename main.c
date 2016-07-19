/*!
 * \file   main.c
 * \brief  arquivo de implementação do recuperador de paginas WEB
 * \date 11/07/2016
 * \author Andre Dantas <andre.dantas@aker.com.br>
 */
#include <stdio.h>
#include <stdlib.h>
#include "recupera_pag_web_lib.h"

int main(int argc, char *argv[])
{
  int ret;
  char *url = NULL, *file_name = NULL;
  char overwrite_flag = 0; 
  int c;
  opterr = 0;

  while ((c = getopt(argc, argv, "ou:f:")) != -1)
  {
    switch (c)
    {
    case 'o':
      overwrite_flag = 1;
      break;
    case 'f':
      file_name = optarg;
      break;
    case 'u':
      url = optarg;
      break;
    case '?':
      if (optopt == 'f')
        ret = ERROR_INCOMP_COMMAND_LINE;
      else if (optopt == 'u')
        ret = ERROR_INCOMP_COMMAND_LINE;
      else if (isprint(optopt))
      {
        printf("Opcao desconhecida '-%c'.", optopt);
        ret = ERROR;
      }
      break;
    default:
      ret = ERROR_INCOMP_COMMAND_LINE;
    }
   } 
  if (ret < 0)
  { 
    show_error_message(ret);
    return 0;    
  }
  ret = params_is_valid(url, file_name, overwrite_flag);
  if (ret < 0)
  { 
    show_error_message(ret);
    return 0;    
  }
  ret = download_file(url, file_name);
  if (ret != 0)
  {
    show_error_message(ret);
    return 0;
  }
  printf("FINALIZADO!\n");
  return 0;
}
