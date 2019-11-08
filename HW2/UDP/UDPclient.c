#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<unistd.h>
#define BUFSIZE 1024

void error_handling(char * message)
{
  fputs(message, stderr);
  fputs("\n",stderr);
  exit(1);
}
struct dtgram
{
  int seq;
  char data[1024];
};
int main(int argc, char**argv)
{
  int sock;
  struct dtgram message;
  int str_len, addr_size, i;
  FILE* fp;
  int count = 1;

  struct sockaddr_in serv_addr;
  struct sockaddr_in from_addr;

  if(argc != 4)
  {
    printf("Usage : %s <IP> <port> <file name>\n", argv[0]);
    exit(1);
  }
  sock = socket(PF_INET,SOCK_DGRAM, 0);
  if(sock == -1)
      error_handling("UDP소켓 생성 오류");

  memset(&serv_addr, 0 , sizeof(serv_addr));
  serv_addr.sin_family=AF_INET;
  serv_addr.sin_addr.s_addr = inet_addr(argv[1]);
  serv_addr.sin_port=htons(atoi(argv[2]));

  /*새롭게 추가된 부분*/
  if(connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr))==-1)
     error_handling("connect() error!");

  if( (fp = fopen(argv[3],"rb"))!= NULL )
  {
    memset(message.data,0,sizeof(message.data));
    strncpy(message.data,argv[3],strlen(argv[3]));
    message.seq = 0;
    while(1)
    {
       send(sock, (char *)&message, sizeof(struct dtgram), 0);
       recv(sock, (char *)&message, sizeof(struct dtgram), 0);
       if( message.seq == 0 ) break;
    }
  }
  else
    printf("There is no such file name\n");

  while(fgets(message.data, sizeof(struct dtgram) - sizeof(int),fp))
  {
    message.seq = count;
    send(sock, (char *)&message, sizeof(struct dtgram), 0);
    count++;
  }
  send(sock, (char *)&message,fclose(fp), 0);
  close(sock);
  return 0;
}
