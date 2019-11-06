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
  int serv_sock;
  int str_len, num = 0;

  struct sockaddr_in serv_addr;
  struct sockaddr_in clnt_addr;
  int clnt_addr_size;

  if(argc!=3)
  {
    printf("Usage: %s <port> <buffer_size>\n",argv[0]);
    exit(1);
  }
  int buffer_size = atoi(argv[2]);
  int message[buffer_size];
  serv_sock=socket(PF_INET, SOCK_DGRAM, 0);
  if(serv_sock == -1)
    error_handling("UDP소켓 생성 오류\n");

  memset(&serv_addr, 0, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr=htonl(INADDR_ANY);
  serv_addr.sin_port = htons(atoi(argv[1]));

  if(bind(serv_sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr))==-1)
    error_handling("bind() error");

  sleep(5);
  
  while(1)
  {
    clnt_addr_size = sizeof(clnt_addr);
    memset(&message, 0, sizeof(message));
    str_len = recvfrom(serv_sock, message, buffer_size,0
              ,(struct sockaddr*)&clnt_addr, &clnt_addr_size);
    sendto(serv_sock, message, str_len, 0 ,(struct sockaddr*)&clnt_addr, sizeof(clnt_addr));
    /*받은 데이터의 길이가 0 즉, eof일 때 종료한다.*/
    if(str_len == 0) break;
    printf("수신 내용 : %s \n", message);
  }
  close(serv_sock);
  return 0;
}
