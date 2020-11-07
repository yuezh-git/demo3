#include<stdio.h>
#include<unistd.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<string.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<sys/wait.h>

#define SERVE_IP     "127.0.0.1"
#define SERVE_PORT   (8888)
#define NET_TYPE     AF_INET
#define LISTEN_MAX   (5)
#define TCP          (0)
#if(TCP)
int main(int argc, char **argv[])
{
   pid_t pid;
   struct sockaddr_in cliaddr[LISTEN_MAX];
   int clisock[LISTEN_MAX];
   int clicnt=0;
   // 收发缓存
   unsigned char buf[256];
   char buf2[20]="client ";
	  // 父进程作为服务器
	int ser_socket=socket(NET_TYPE,SOCK_STREAM,0);
	if(ser_socket==-1)
		  perror("creat socket fail ");
	// 为服务器设定属性
	struct sockaddr_in seraddr;
	seraddr.sin_family=NET_TYPE;
	seraddr.sin_port=SERVE_PORT;
	seraddr.sin_addr.s_addr=inet_addr(SERVE_IP);
	socklen_t addrlen=sizeof(struct sockaddr);
   
   if(pid=fork())
   {
	   // 服务器属性绑定到描述符
	   int res=bind(ser_socket,(struct sockaddr *)(&seraddr),addrlen);
	   if(res==-1){
           perror("bind addr fail");
	   }
	   //监听
	   listen(ser_socket,LISTEN_MAX);   
	   // 服务器等待客户端连接
	   clisock[clicnt]=accept(ser_socket,(struct sockaddr *)(&cliaddr),&addrlen);
	   printf("client family %d  port %d  addr %d \n",	seraddr.sin_family,seraddr.sin_port,seraddr.sin_addr.s_addr);

	   for(;;){
	       send(clisock[clicnt],buf2,strlen(buf2),0);
	   	   memset(buf,0,256);
	   	   recv(clisock[clicnt],buf,256,0);
	       printf("serve recv %s \n",(char *)(buf));
	
		   if(strncmp(buf,"quit",4)==0){
		  	   close(clisock[clicnt]);
			   printf("cli close \n");
		       while(waitpid(-1,NULL,WNOHANG)>0){
			   	     printf("child close\n");
			   }
			   _exit(0);
		   }
	   }
	   
   }
   // 子进程
   else
   {
       // 开启新进程，所用资源都会cp到子进程、描述符号也会被+1
       // 此处关闭子进程引用服务器描述符号
        close(ser_socket);
   	   int cli=socket(NET_TYPE,SOCK_STREAM,0);
	   if(cli==-1)
	   	  perror("cli creat fail");

       // 请求连接
       int res=connect(cli,(struct sockaddr *)(&seraddr),addrlen);
       if(res==-1)
	   	   perror("cli connect fail");

	   for(;;){
	   	
          recv(cli,buf,256,0);	  
		  printf("%s:",buf);
		  memset(buf,0,128);
		  scanf("%s",buf);
	      send(cli,buf,strlen(buf),0); 
		  if(strncmp(buf,"quit",4)==0){
		  	   close(cli);
			   printf("cli close \n");
			   _exit(0);

		  }		  	

	   }
   }


   

}
#else
int main(int argc,char *argcv[])
{
	pid_t pid;
	struct sockaddr_in cliaddr[LISTEN_MAX];
	int clisock[LISTEN_MAX];
	int clicnt=0;
	// 收发缓存
	unsigned char buf[256];

	 // 父进程作为服务器
	int ser_socket=socket(NET_TYPE,SOCK_DGRAM,0);
	if(ser_socket==-1)
		 perror("creat socket fail ");
	// 为服务器设定属性
	struct sockaddr_in seraddr;
	seraddr.sin_family=NET_TYPE;
	seraddr.sin_port=SERVE_PORT;
	seraddr.sin_addr.s_addr=inet_addr(SERVE_IP);
	socklen_t addrlen=sizeof(struct sockaddr);

	if(pid=fork())
	{
	    // 父进程作为服务器
	    // 描述符绑定属性
	    int res=bind(ser_socket,(struct sockaddr *)&seraddr,addrlen);
		if(res==-1)
			perror("serve bind err");
		for(;;){
			memset(buf,0,sizeof(buf));
		    recvfrom(ser_socket,buf,sizeof(buf),0,(struct sockaddr *)&cliaddr[clicnt],&addrlen);
		    sendto(ser_socket,buf,strlen(buf),0,(struct sockaddr *)&cliaddr[clicnt],addrlen);
			 if (strncmp(buf,"quit",4)==0){
		  	    close(clisock[clicnt]);
			    printf("cli close \n");
		        while(waitpid(-1,NULL,WNOHANG)>0){
			   	     printf("child close\n");
			    }
			   _exit(0);
			 }
		}


	}
	else
	{
	    // 关闭引用描述符号
	    close(ser_socket);
		clisock[clicnt]=socket(NET_TYPE,SOCK_DGRAM,0);
		for(;;){
			printf("client:");
			scanf("%s",buf);		
			sendto(clisock[clicnt],buf,strlen(buf),0,(struct sockaddr *)&seraddr,addrlen);
			memset(buf,0,sizeof(buf));
		    recvfrom(clisock[clicnt],buf,sizeof(buf),0,(struct sockaddr *)&seraddr,&addrlen);
			printf("clint recv %s \n",buf);
			if(strncmp(buf,"quit",4)==0){
			  	   close(clisock[clicnt]);
				   printf("cli close \n");
				   _exit(0);

			  }	
		}

	}


}
#endif

