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
struct dtgram
{
  int seq;
  char data[1025];
};
int main(int argc, char**argv)
{
  int serv_sock;
  int str_len, num = 0;
  struct dtgram message;
  char buffer[1025];

  struct sockaddr_in serv_addr;
  struct sockaddr_in clnt_addr;
  int clnt_addr_size;
  FILE* fp;

  if(argc!=2)
  {
    printf("Usage: %s <port> \n",argv[0]);
    exit(1);
  }
  serv_sock=socket(PF_INET, SOCK_DGRAM, 0);
  if(serv_sock == -1)
    error_handling("UDP소켓 생성 오류\n");

  memset(&serv_addr, 0, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr=htonl(INADDR_ANY);
  serv_addr.sin_port = htons(atoi(argv[1]));

  if(bind(serv_sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr))==-1)
    error_handling("bind() error");


  while(1)
  {
    clnt_addr_size = sizeof(clnt_addr);
    memset((char *)&message, 0, sizeof(struct dtgram));
    str_len = recvfrom(serv_sock ,(char *)&message, sizeof(struct dtgram), 0
              ,(struct sockaddr*)&clnt_addr, &clnt_addr_size);
    sendto(serv_sock, (char *)&message, str_len, 0 ,(struct sockaddr*)&clnt_addr, sizeof(clnt_addr));

    if(message.seq == 0)
    {
        int title_length = strlen(message.data);
        char title[title_length + 1];
        strncpy(title, message.data, title_length);
        title[title_length] = 0;
        fp = fopen(title,"wb");
	break;
    }
  }

  while(1)
  {
    clnt_addr_size = sizeof(clnt_addr);
    memset(buffer, 0, sizeof(buffer));
    str_len = recvfrom(serv_sock ,buffer, sizeof(buffer), 0
              ,(struct sockaddr*)&clnt_addr, &clnt_addr_size);
    if(str_len == 0) break;
    fprintf(fp,"%s",buffer);
  }
  fclose(fp);
  close(serv_sock);
  return 0;
}
