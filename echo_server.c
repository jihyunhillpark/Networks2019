#include<stdio.h>
#include<string.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#define BUFSIZE 100

void error_handling(char * message)
{
  fputs(message, stderr);
  fputc("\n",stderr);
  exit(1);
}

int main(int argc, char**argv)
{
  int serv_sock;
  int clnt_sock;
  char message[BUFSIZE]
  int str_len;

  struct sockaddr_in serv_addr;
  struct sockaddr_in clnt_addr;
  int clnt_addr_size;

  if(argc != 2)
  {
    printf("Usage : %s <port>\n", argv[0]);
    eit(1);
  }
  serv_sock = socket(PF_INET,SOCK_STREAM, 0);
  if(serc_sock == -1)
    error_handling("socket() error");

  memset(&serv_addr, 0, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr=htonl(INADDR_ANY);
  serv_addr.sin_port = htons(argv[1]);

  if(bind(serv_sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr))==-1)
    error_handling("bind() error");

  if(listen(serc_sock, 5)== -1)
    error_handling("listen() error");

  clnt_addr_size = sizeof(clnt_addr);
  clnt_sock = accept(serc_sock,(struct sockaddr*)&clnt_addr,&clnt_addr_size);
  if(clnt_sock == -1)
    error_handling("accept() error");

  /*데이터 수신 및 전송*/
  while(str_len==read(clnt_sock,message,BUFSIZE))!=0{
    write(clnt_sock,message, str_len);
    write(1, message, str_len);
  }
  close(clnt_sock);
  return 0;

}
