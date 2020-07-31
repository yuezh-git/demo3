#include<stdio.h>
#include<unistd.h>
#include<string.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<sys/socket.h>
#include"sockType.h"
int main(){
	
#ifdef TCP
    int cli_socket=socket(AF_INET, SOCK_STREAM, 0);
#else
	int cli_socket=socket(AF_INET, SOCK_DGRAM, 0);
#endif
	if(cli_socket==-1) perror("socket faile \n");	
	struct sockaddr_in	addr_ser;
	addr_ser.sin_family=AF_INET;
	addr_ser.sin_port =htons(1234);
	addr_ser.sin_addr.s_addr=inet_addr(IP);

	socklen_t addrlen = sizeof(struct sockaddr);

     char sendbuf[256];	  //申请一个发送数据缓存区
     char recvbuf[256];	  //申请一个接收数据缓存区
     
#ifndef TCP
	while(1){
		//向客户端发送数据
		printf("Cli:>");
		scanf("%s",sendbuf);
		sendto(cli_socket, sendbuf, strlen(sendbuf)+1, 0, (struct sockaddr*)&addr_ser, addrlen);   
		//接收来自客户端的数据
		recvfrom(cli_socket, recvbuf, 100, 0, (struct sockaddr*)&addr_ser, &addrlen);
		printf("Ser:>%s\n", recvbuf);
	
	}
#else
	int conet =connect(cli_socket,(struct sockaddr*)&addr_ser,addrlen);
	if(conet==-1) perror("connect fail");
	int recvlen;
	while(1)
	{
		printf("send:");
		scanf("%s",sendbuf);
		if(strncmp(sendbuf,"quit",4)==0){
			shutdown(cli_socket,SHUT_RDWR);
			break;
		}
		else{
			write(cli_socket,sendbuf,strlen(sendbuf)+1);
			recvlen= read(cli_socket,recvbuf,200);
			printf(" len =%d %s \n",recvlen, recvbuf);
		}

	}

#endif	
return 0;
}
