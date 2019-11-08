#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<unistd.h>

void error_handling(char * message)
{
  fputs(message, stderr);
  fputs("\n",stderr);
  exit(1);
}

int main(int argc, char**argv)
{
  int sock;
  char *buffer;
  char message[1024];
  int str_len;
  struct sockaddr_in serv_addr;
  FILE *fp;
  int size;

  if(argc!=4)
  {
    printf("Usage: %s <IP> <PORT> <FILE_NAME> \n",argv[0]);
    exit(1);
  }

  sock = socket(PF_INET, SOCK_STREAM, 0);
  if(sock == -1)
    error_handling("socket() error");

  memset(&serv_addr, 0 , sizeof(serv_addr));
  serv_addr.sin_family=AF_INET;
  serv_addr.sin_addr.s_addr = inet_addr(argv[1]);
  serv_addr.sin_port=htons(atoi(argv[2]));

  if(connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr))==-1)
    error_handling("connect() error!");


  //파일길이와 길이 전송
  if((fp = fopen(argv[3], "rb")) != NULL)
  {
    sprintf(message, "%d",strlen(argv[3]));
    printf("buffer size %s\n", message);
    send(sock, message, sizeof(int) ,0 );
    send(sock, argv[3], strlen(argv[3]), 0);
  }
  else
    printf("파일이 존재하지 않습니다!\n");

  //파일 읽어들이
  fseek(fp, 0,SEEK_END);
  size = ftell(fp);

  buffer = malloc(size + 1);
  memset(buffer,0,size + 1);

  fseek(fp, 0 ,SEEK_SET);
  fread(buffer, size, 1 ,fp);

  //파일 전송하기
  send(sock,buffer,strlen(buffer),0);

  close(sock);
  return 0;
}
