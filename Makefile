CC = gcc
tcp_exe : echo_client.o echo_server.o
	gcc -o tcp_exe echo_client.o echo_server.o

echo_client.o : echo_client.c
	gcc -c -o echo_client.o echo_client.c

echo_server.o : echo_server.c
	gcc -c -o echo_server.o echo_server.c

clean :
	rm *.o 
