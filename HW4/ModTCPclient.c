#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<unistd.h>
#define MAXMESSAGE 30
#define PORT_N 502
#define BYTESIZE 8
#define HEAD_LEN1 12
#define HEAD_LEN2 14

void error_handling(char * message)
{
  fputs(message, stderr);
  fputs("\n",stderr);
  exit(1);
}
void FillForeHeadBytes(unsigned char *message, int req, int size_msg)
{
  message[0] = 0x00;
  message[1] = 0x00;
  message[2] = 0x00;
  message[3] = 0x00;
  message[4] = 0x00;
  message[5] = size_msg-1-5;
  message[6] = 0x01;
}
void FillDataBytes(unsigned char* message
                  ,unsigned char fun_code
                  ,unsigned char start_addr_hi,unsigned char start_addr_lo
                  ,unsigned char quantity_hi, unsigned char quantity_lo)
{
  message[7] = fun_code;
  message[8] = start_addr_hi;
  message[9] = start_addr_lo;
  message[10] = quantity_hi;
  message[11] = quantity_lo;
}
void DecTobin(int number, int* bin_container, int len)
{
  int i;
  for(i = len-1; 0 <= i ; i--)
  {
    if( number < 2)
    {
      bin_container[i] = number;
      break;
    }
    bin_container[i] = number%2;
    number/=2;
  }
}
void ReadCoilInfo(int* bin_coil, int* ct, int len)
{
  int i;
  for(i = 0; i < len ; i++)
  {
    if(bin_coil[i] == 1) (*ct)++;
  }
}
void decoding(unsigned char *rcv_message,int str_len)
{
  int i, sp = 9,ct=0;
  switch(rcv_message[7]){
    case 0x01:
    {
      for(i = sp; i < str_len ; i++)
      {
         int coil_written = rcv_message[i];
         int bin_coil[8] = {0,};
         DecTobin(coil_written,bin_coil,BYTESIZE);
         ReadCoilInfo(bin_coil,&ct,BYTESIZE);
      }
      printf("::Recved length is %d BYTES\n", str_len);
      printf("::Function - Read Coils\n");
      printf("::%d coil(s) - \'True\' state from server!",ct);
      break;
    }
    case 0x0F:
    {
      int sa = 10*(int)rcv_message[8]+(int)rcv_message[9];
      int coils = 10*(int)rcv_message[10]+(int)rcv_message[11];

      printf("::Recved length is %d BYTES\n", str_len);
      printf("::Function - Write Multiple Coils\n");
      printf("::Starting Address is %d!\n",sa);
      printf("::The number of controlled Coils in server is %d", coils);
      break;
    }
    case 0x03:
    {
      int registers = rcv_message[8]/2;
      int value;

      printf("::Recved length is %d BYTES\n", str_len);
      printf("::Function - Read holding Registers\n");
      printf("::The # of written on registers is %d in server\n", registers);
      printf("::Value of Registers got from server are following\n");
      for(i = sp; i < str_len; i+=2)
      {
        value = 10*(int)rcv_message[i]+(int)rcv_message[i+1];
        printf("Value of %d(th) register from starting address = %d\n", (i-sp)/2, value);
      }
      break;
    }
    case 0x10:
    {
      int sa = 10*(int)rcv_message[8]+(int)rcv_message[9];
      int registers = 10*(int)rcv_message[10]+(int)rcv_message[11];

      printf("::Recved length is %d BYTES\n", str_len);
      printf("::Function - Write Multiple Registers\n");
      printf("::Starting Address is %d!\n",sa);
      printf("::The number of controlled Registers in server is %d", registers);
      break;
    }
  }
}
int main(int argc, char**argv)
{
  int sock;
  int str_len;
  struct sockaddr_in serv_addr;

  if(argc!=2)
  {
    printf("Usage: %s <IP> \n",argv[0]);
    exit(1);
  }

  sock = socket(PF_INET, SOCK_STREAM, 0);
  if(sock == -1)
    error_handling("socket() error");

  memset(&serv_addr, 0 , sizeof(serv_addr));
  serv_addr.sin_family=AF_INET;
  serv_addr.sin_addr.s_addr = inet_addr(argv[1]);
  serv_addr.sin_port=htons(PORT_N);

  if(connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr))==-1)
    error_handling("connect() error!");
  // 메시지 입력,전송
  while(1)
  {
    int req;
    int start_addr, quantity, message_len, start_addr2;
    unsigned char *message, rcv_message[MAXMESSAGE];
    unsigned char fun_code, start_addr_hi, start_addr_lo, quantity_hi, quantity_lo;

    fputs("=============================\n",stdout);
    fputs("What function do you want?\n",stdout);
    fputs("(1) : Read Coils\n",stdout);
    fputs("(2) : Write multiple Coils\n",stdout);
    fputs("(3) : Read Holding Registers\n",stdout);
    fputs("(4) : Write multiple (Holding) Registers\n",stdout);
    fputs("(0) : Quit\n",stdout);
    fputs("=============================\n",stdout);

    fputs("Select a function [1, 2, 3, 4, 0] :",stdout);
    scanf("%d", &req);

    if( req == 1 || req == 3)
    {
      fputs("Enter the Start Address :",stdout);
      scanf("%d", &start_addr);
      start_addr_lo = (unsigned char)start_addr%(16*16);
      start_addr2 = start_addr/(16*16);
      start_addr_hi = (unsigned char)start_addr2;

      message_len = HEAD_LEN1;
      if(req == 1)
      {
         fputs("Enter the number of Coils to Read :",stdout);
         fun_code = 0x01;
         scanf("%d", &quantity);
      }
      else{
         fputs("Enter the number of Registers to Read (AWARE!available range from 1 to 8) :",stdout);
         fun_code = 0x03;

         scanf("%d", &quantity);
         if( BYTESIZE < quantity){
           printf("ERROR! trying to access unavailable range!\n");
           break;
         }
      }

      message = (unsigned char *)malloc(sizeof(unsigned char)*message_len);
      memset(message, 0 , sizeof(unsigned char));

      quantity_lo = (unsigned char)quantity%(16*16);
      quantity/=(16*16);
      quantity_hi = (unsigned char)quantity;

      FillForeHeadBytes(message,req,message_len);
      FillDataBytes(message, fun_code,start_addr_hi,start_addr_lo,quantity_hi,quantity_lo);
    }
    else if(req == 2) //write multiple coils
    {
      fputs("Enter the Start Address :",stdout);
      scanf("%d", &start_addr);
      start_addr_lo = (unsigned char)start_addr%(16*16);
      start_addr2 = start_addr/(16*16);
      start_addr_hi = (unsigned char)start_addr2;

      fputs("\nEnter the number of Coils in you want to write on(N of coils at most 8) :",stdout);
      scanf("%d", &quantity);

      message_len = HEAD_LEN2;
      message = (unsigned char *)malloc(sizeof(unsigned char)*message_len);
      memset(message, 0 , sizeof(unsigned char));
      message[12] = (quantity + BYTESIZE-1) / BYTESIZE;
      //두배
      int * coils_flag = (int *)malloc(sizeof(int)*message[12]*BYTESIZE);
      memset(coils_flag, 0, BYTESIZE);
      int i,onCoil,binary=1,ov=0;


      for(i = 0; i < quantity; i++)
      {
        printf("Enter the Coil address #%d to #%d you want switch ON, otherwise left will be OFF :",start_addr,start_addr+BYTESIZE-1);
        scanf("%d",&onCoil);
        coils_flag[BYTESIZE-1-(onCoil-start_addr)] = 1;
      }
      //flag를 하나의 character 즉, 16진수로 보내기
      for(i = BYTESIZE - 1 ; 0 <= i ; i--)
      {
        ov += coils_flag[i]*binary;
        binary *= 2;
      }
      printf("%d",ov);
      message[13] = (unsigned char)ov;
      quantity_lo = (unsigned char)quantity%(16*16);
      quantity/=(16*16);
      quantity_hi = (unsigned char)quantity;

      fun_code = 0x0F;

      FillForeHeadBytes(message,req,message_len);
      FillDataBytes(message,fun_code,start_addr_hi,start_addr_lo,quantity_hi,quantity_lo);
    }
    else if(req == 4)
    {
      fputs("Enter the Start Address :",stdout);
      scanf("%d", &start_addr);
      start_addr_lo = (unsigned char)start_addr%(16*16);
      start_addr2 = start_addr/(16*16);
      start_addr_hi = (unsigned char)start_addr2;

      fputs("\nEnter the number of Registers in you want to write on:",stdout);
      scanf("%d", &quantity);

      message_len = HEAD_LEN2-1 + 2*quantity;
      message = (unsigned char *)malloc(sizeof(unsigned char)*message_len);
      memset(message, 0 , sizeof(unsigned char));
      message[12] = quantity * 2;
      int i,regValue,binary=1,ov=0, target_address;
      unsigned char regValue_hi,regValue_lo;

      for(i = 0; i < quantity;i++ )
      {
        target_address = start_addr + i;
        printf("Write the %d(th) register value :",target_address);
        scanf("%d", &regValue);
        regValue_lo = (unsigned char)regValue%(16*16);
        regValue/=(16*16);
        regValue_hi = (unsigned char)regValue;
        int pos_lo = HEAD_LEN2 + (target_address-start_addr)*2;
        message[pos_lo] = regValue_lo;
        message[pos_lo-1] = regValue_hi;
      }

      quantity_lo = (unsigned char)quantity%(16*16);
      quantity/=(16*16);
      quantity_hi = (unsigned char)quantity;

      fun_code = 0x10;

      FillForeHeadBytes(message,req,message_len);
      FillDataBytes(message,fun_code,start_addr_hi,start_addr_lo,quantity_hi,quantity_lo);
    }
    else break;

    send(sock, message,message_len, 0);
    str_len = recv(sock, rcv_message, MAXMESSAGE, 0);
    printf("\n-----<RESPONSE>-----\n");
    decoding(rcv_message, str_len);
    printf("\n--------------------\n\n");

    free(message);
  }
  close(sock);
  return 0;
}
