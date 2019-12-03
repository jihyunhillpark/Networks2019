#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<unistd.h>
#define BUFSIZE 1024
#define FIRST_ELEMENTS 3

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
  FILE * fp;
  //char file_contents[BUFSIZE];

  if(argc != 2)
  {
    printf("Usage : %s <port> \n", argv[0]);
    exit(1);
  }
  char message[BUFSIZE];
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

  /*데이터 수신 및 전송*/

  while(1){
    memset(message,0, sizeof(BUFSIZE));
    clnt_addr_size = sizeof(clnt_addr);

    clnt_sock = accept(serv_sock,(struct sockaddr*)&clnt_addr,&clnt_addr_size);
    if(clnt_sock == -1)
      error_handling("accept() error");

    if((str_len=recv(clnt_sock,message,BUFSIZE,0)) == 0) break;

    message[str_len]=0;
    printf("%s\n",message);
    char * token;
    token = strtok(message, " ");
    printf("%s\n",token);
    int get_flag = 0;
    int post_flag = 0;
    char * file_contents;
    char status[30] = "HTTP/1.1 200 OK\n";
    char response[BUFSIZE] = {0};
    /* GET 일 때 */
    if(!strcmp(token,"GET"))
    {
      token = strtok(NULL," ");
      if((!strcmp(token,"/")) || (!strcmp(token,"/index.html")))
      {
        printf("request contetns : %s\n", token);
        fp = fopen("index.html","rb");
        get_flag = 1;
      }
      else if(!strcmp(token,"/query.html"))
      {
        printf("request contetns : %s\n", token);
        fp = fopen("query.html","rb");
        get_flag = 1;
      }
    }
    else if(!strcmp(token,"POST"))
    {
      post_flag = 1;

      while (token != NULL)               // 자른 문자열이 나오지 않을 때까지 반복
      {
          printf("%s\n", token);          // 자른 문자열 출력
          token = strtok(NULL, "\n");      // 다음 문자열을 잘라서 포인터를 반환
          if(!(strncmp(token,"name=",5))) break;
      }
      printf("last token : %s\n", token);
    }

    if(get_flag == 1)
    {
      int count;
      int size;

      fseek(fp, 0, SEEK_END);    // 파일 포인터를 파일의 끝으로 이동시킴
      size = ftell(fp);          // 파일 포인터의 현재 위치를 얻음
      printf("file size %d",size);
      file_contents = malloc(size + 1);    // 파일 크기 + 1바이트(문자열 마지막의 NULL)만큼 동적 메모리 할당
      memset(file_contents, 0, size + 1);  // 파일 크기 + 1바이트만큼 메모리를 0으로 초기화

      fseek(fp, 0, SEEK_SET);                // 파일 포인터를 파일의 처음으로 이동시킴
      count = fread(file_contents, size, 1, fp);    // hello.txt에서 파일 크기만큼 값을 읽음

      printf("%s size: %d, count: %d\n", file_contents, size, count);

      fclose(fp);     // 파일 포인터 닫기

      sprintf(response,"%s%s%d\n%s%s%s%s"
            ,status
            ,"Content-Length: ",strlen(file_contents)
            ,"Content-Type: text/html\n"
            ,"Connection: Closed\n"
            ,"\n"
            ,file_contents);
      free(file_contents);   // 동적 메모리 해제
    }
    else if( post_flag == 1 )
    {
      sprintf(response,"%s%s%d\n%s%s%s%s%s%s"
            ,status
            ,"Content-Length: ",strlen(token)+9
            ,"Content-Type: text/html\n"
            ,"Connection: closed\n"
            ,"\n"
            ,"<h2>"
            ,token
            ,"</h2>");
    }
    else // 에러인 경우
    {
      memset(status,0, sizeof(status));
      strcpy(status, "HTTP/1.1 404 Not Found");
      sprintf(response,"%s\n%s%d\n%s\n%s\n%s"
                      ,status
                      ,"Content-Length:",0
                      ,"Content-Type: text/html"
                      ,"Connection: closed"
                      ,"\n");
      printf("%s",response);
    }
      printf("\n\n!!!!response : %s",response);
      send(clnt_sock,response,strlen(response),0);

  }
  close(clnt_sock);
  return 0;
}
