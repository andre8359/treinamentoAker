#!/usr/bin/env python
import sys
import os

file_urls = ['http://lovelace.aker.com.br/Arquivos/ProjetoNovoDPIeIPS.pdf',
            'http://lovelace.aker.com.br/Arquivos/acc_fw67_fw65_auth.patch',
            'http://lovelace.aker.com.br/Arquivos/planilha_bugs.ods',
            'http://lovelace.aker.com.br/ISOs/ubuntu-12.04-server-i386.iso',
            'http://lovelace.aker.com.br/ISOs/Fedora-16-i386-DVD.iso']

for i in xrange(len(file_urls)):
  #print("-----------------------------------------------------")
  #os.system ('wget ' + file_urls[i] + ' arquivos_originais/' + 
  #  file_urls[i][file_urls[i].rfind('/')+1:])
  print("-----------------------------------------------------")
  os.system ('./prog -o -u '+ file_urls[i] + ' -f arquivos_teste/' + 
    file_urls[i][file_urls[i].rfind('/')+1:])
print("-----------------------------------------------------")

for i in xrange(len(file_urls)):
  print("*******************************************************")
  os.system("md5sum arquivos_originais/" + file_urls[i][file_urls[i].rfind('/')+1:]
    + " arquivos_teste/" + file_urls[i][file_urls[i].rfind('/')+1:])
print("*******************************************************")

