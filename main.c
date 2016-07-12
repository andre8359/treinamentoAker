/*i
 * \file   main.c
 * \brief  arquivo de implementação do recuperador de paginas WEB
 * \data 11/07/2016
 * \author Andre Dantas <andre.dantas@aker.com.br>
 */


#include <stdio.h>
#include <stdlib.h>
#include "recupera_pag_web_lib.h"

int main(int argc, char *argv[])
{
  int ret;
  ret = params_is_valid(argc, argv);

  if (ret < 0)
  { 
    show_error_message(ret);
    exit(1);    
  }
 ret = download_file(argv);  
}
