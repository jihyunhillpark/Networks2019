#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<unistd.h>
#define BUFFSIZE 1024

void error_handling(char *message)
{
  fputs(message, stderr);
  fputs("\n",stderr);
  exit(1);
}

int main(int argc, char**argv)
{
  int serv_sock;
  int clnt_sock;
  int str_len;
  char message[BUFFSIZE];
  struct sockaddr_in serv_addr;
  struct sockaddr_in clnt_addr;
  int clnt_addr_size;
  FILE *fp;

  if(argc != 2)
  {
    printf("Usage : %s <port>\n", argv[0]);
    exit(1);
  }

  serv_sock = socket(PF_INET,SOCK_STREAM, 0);
  if(serv_sock == -1)
    error_handling("socket() error");

  memset(&serv_addr, 0, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr=htonl(INADDR_ANY);
  serv_addr.sin_port = htons(atoi(argv[1]));

  if(bind(serv_sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr))==-1)
    error_handling("bind() error");

  if(listen(serv_sock, 5)== -1)
    error_handling("listen() error");

  clnt_addr_size = sizeof(clnt_addr);

  clnt_sock = accept(serv_sock,(struct sockaddr*)&clnt_addr,&clnt_addr_size);
  if(clnt_sock == -1)
    error_handling("accept() error");

  recv(clnt_sock,message,sizeof(int),0);
  int file_length = atoi (message);
  recv(clnt_sock,message,file_length,0);
  message[file_length] = 0;
  fp = fopen(message,"wb");

  /*데이터 수신 및 전송*/
  while((str_len=recv(clnt_sock,message,BUFFSIZE,0))!= 0){
    message[str_len]=0;
    fputs(message,fp);
  }
  fclose(fp);
  close(clnt_sock);
  return 0;

}
