#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<unistd.h>

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

  struct sockaddr_in serv_addr;
  struct sockaddr_in clnt_addr;
  int clnt_addr_size;

  if(argc != 3)
  {
    printf("Usage : %s <port> <buffer_size>\n", argv[0]);
    exit(1);
  }
  int buffer_size = atoi(argv[2]);
  char message[buffer_size];
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

  sleep(5);
  /*데이터 수신 및 전송*/
  while((str_len=recv(clnt_sock,message,buffer_size,0))!= 0){
    message[str_len]=0;
    printf("수신된 메시지 : %s \n",message);
    send(clnt_sock,message, str_len,0);
    //send(1, message, str_len,0);
    //memset(&message, 0, sizeof(message));
  }
  close(clnt_sock);
  return 0;

}
