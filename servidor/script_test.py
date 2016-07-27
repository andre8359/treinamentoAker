#!/usr/bin/env python
import sys
import os
from multiprocessing import Process

PORT  = "8080"
IP = "10.4.3.1"
file_urls = [IP +':' + PORT + '/pdfs/evolucaoDosCodecs.pdf',
            IP + ':' + PORT + '/imgs/lena.jpeg',
            IP +':' + PORT + '/imgs/mandrill.png',
            IP +':' + PORT + '/teste_300mb.iso',
            IP +':' + PORT + '/teste']
process_list = []

def download_file(file_url, fileout_name):
  print('Download start --->' + fileout_name)
  os.system('curl -sS --http1.0 --url ' + file_url + ' -o arquivos_download/' 
    + fileout_name)
  print('Download end --->' + fileout_name)
for i in xrange(len(file_urls)):
  print("-----------------------------------------------------")
  p = Process(target=download_file,
args=(file_urls[i],file_urls[i][file_urls[i].rfind('/')+1:], ))
  p.start()
  process_list.append(p)
  print("-----------------------------------------------------")
for p in process_list:
    p.join()
for i in xrange(len(file_urls)):
  print("*******************************************************")
  os.system("md5sum root/" + file_urls[i][file_urls[i].find('/')+1:]
    + " arquivos_download/" + file_urls[i][file_urls[i].rfind('/')+1:])
print("*******************************************************")

