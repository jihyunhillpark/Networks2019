#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<unistd.h>
#define BUFSIZE 100
#define MESSAGES 3

void error_handling(char * message)
{
  fputs(message, stderr);
  fputs("\n",stderr);
  exit(1);
}

int main(int argc, char**argv)
{
  int sock;
  char message[BUFSIZE];
  char message1[BUFSIZE] = "1234567890";
  char message2[BUFSIZE] = "ABCDEFGHIJ";
  char message3[BUFSIZE] = "KLMNOPQRST";
  int str_len;
  struct sockaddr_in serv_addr;

  if(argc!=3)
  {
    printf("Usage: %s <IP> <PROT> \n",argv[0]);
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

  send(sock, message1, strlen(message1), 0);
  send(sock, message2, strlen(message2), 0);
  send(sock, message3, strlen(message3), 0);
  // 메시지 입력,전송
  while(1)
  {

    fputs("전송할 메시지를 입력하세요(q to quit) : ",stdout);
    fgets(message,BUFSIZE, stdin);
    if(!strcmp(message,"q\n")) break;
    send(sock, message, strlen(message), 0);

    str_len = recv(sock, message, BUFSIZE, 0);
    message[str_len] = 0;
  }

  shutdown(sock,1);
  close(sock);
  return 0;
}
