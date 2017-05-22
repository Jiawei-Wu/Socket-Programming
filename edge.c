#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define MAXLINES 100
#define LEN 10

int tcp_port = 23575;
int udp_port = 24575;
int or_port = 21575;
int and_port = 22575;

char addr[20] = "127.0.0.1";
char output[MAXLINES][BUFSIZ];
int num;
int and_num;
int or_num;

int sockfd;
struct sockaddr_in servaddr;
int sockfd2;
struct sockaddr_in servaddr2;

void show_output(){
	int i;

	printf("The edge server has received %d lines from the client using TCP over port %d\n",num,tcp_port);

	printf("The edge server has successfully sent %d lines to Backend-Server OR.\n",or_num);
	printf("The edge server has successfully sent %d lines to Backend-Server AND.\n",and_num);

	printf("The edge server start receiving the computation results from Backend-Server OR and Backend-Server AND using UDP port %d.\n",udp_port);

	printf("The computation results are:\n");

	for(i=0;i<num;i++){
		printf("%s\n",output[i]);
	}
	printf("The edge server has successfully finished receiving all computation results from the Backend-Server OR and Backend-Server AND.\n");
	printf("The edge server has successfully finished sending all computation results to the client.\n");
}

void connect_backend()
{
    sockfd = socket(PF_INET, SOCK_DGRAM, 0);

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(or_port);
    servaddr.sin_addr.s_addr = inet_addr(addr);

    sockfd2 = socket(PF_INET, SOCK_DGRAM, 0);

    bzero(&servaddr2, sizeof(servaddr2));
    servaddr2.sin_family = AF_INET;
    servaddr2.sin_port = htons(and_port);
    servaddr2.sin_addr.s_addr = inet_addr(addr);

    //char sendline[100];
    //sprintf(sendline, "Hello, world!");

    //sendto(sockfd, sendline, strlen(sendline), 0, (struct sockaddr *)&servaddr, sizeof(servaddr));

}

void deal(char * buf){
    int opera;
    char *p;
    char *p2;
    char number1[LEN+1];
    char number2[LEN+1];	
    char sendline[BUFSIZ];
    char recvline[BUFSIZ];

    if(buf[0]=='a')
        opera = 2;  //and
    else
        opera = 1;  //or

    p = strchr(buf,',');
    p2 = strchr(p+1,',');
    int length = p2-p-1;
    memcpy(number1,p+1,length);
    number1[length]='\0';
	strcpy(sendline,number1);
	sendline[length]=' ';
    
    int length2 = 0;
    p2++;
    while(*p2=='0'||*p2=='1'){
        number2[length2++]=*p2;
        p2++;
    } 
    number2[length2]='\0';
	strcpy(sendline+length+1,number2);
	int ulen = length+1+length2;
    sendline[ulen]=' ';
    sendline[ulen+1]='\0';

	if(opera==1){

		sendto(sockfd, sendline, strlen(sendline), 0, (struct sockaddr *)&servaddr, sizeof(servaddr));
		recvfrom(sockfd, recvline, LEN+1, 0, NULL, NULL);
	or_num++;
	}
	else{

		sendto(sockfd2, sendline, strlen(sendline), 0, (struct sockaddr *)&servaddr2, sizeof(servaddr2));
		recvfrom(sockfd2, recvline, LEN+1, 0, NULL, NULL);
and_num++;

	}
	strcpy(buf,number1);
	length = strlen(number1);
	buf[length++]=' ';
	if(opera==1){
		buf[length++]='o';
		buf[length++]='r';
	}
	else{
	
		buf[length++]='a';
		buf[length++]='n';
		buf[length++]='d';
	}
	buf[length++]=' ';
	strcpy(buf+length,number2);
    length+=strlen(number2);     	
	buf[length++]=' ';
	buf[length++]='=';
	buf[length++]=' ';

	length2 = 0;
	while(recvline[length2]=='0') length2++;
	if(length2==strlen(recvline))
	{
		buf[length++]='0';
		buf[length]='\0';
	}
	else	
		strcpy(buf+length,recvline+length2);
	
	if(opera==1){
		sendto(sockfd, buf, strlen(buf), 0, (struct sockaddr *)&servaddr, sizeof(servaddr));
		recvfrom(sockfd, recvline, BUFSIZ, 0, NULL, NULL);
	}
	else{
		sendto(sockfd2, buf, strlen(buf), 0, (struct sockaddr *)&servaddr2, sizeof(servaddr2));
		recvfrom(sockfd2, recvline, BUFSIZ, 0, NULL, NULL);


	}
	strcpy(output[num],recvline);	
	//output[num++][LEN] = '\0';
num++  ;

p = strchr(recvline,'=');

strcpy(buf,p+2);

}

void connect_client()
{
    int server_sockfd;
    int client_sockfd; 
    int len;  
    struct sockaddr_in my_addr;    
    struct sockaddr_in remote_addr; 
    int sin_size;  
    char buf[BUFSIZ];  
    memset(&my_addr,0,sizeof(my_addr));  
    my_addr.sin_family=AF_INET; 
    my_addr.sin_addr.s_addr=INADDR_ANY;
    my_addr.sin_port=htons(tcp_port);         
        
    if((server_sockfd=socket(PF_INET,SOCK_STREAM,0))<0)  
    {    
        perror("socket error");  
		exit(1);
    }  
      
    if(bind(server_sockfd,(struct sockaddr *)&my_addr,sizeof(struct sockaddr))<0)  
    {  
        perror("bind error");  
		exit(1);
    }  
          
    if(listen(server_sockfd,5)<0)  
    {  
        perror("listen error");  
		exit(1);
    };  
          
    sin_size=sizeof(struct sockaddr_in);  

	printf("The edge server is up and running.\n");

    if((client_sockfd=accept(server_sockfd,(struct sockaddr *)&remote_addr,&sin_size))<0)  
    {  
        perror("accept error");  
		exit(1);
    }  

    //printf("accept client %s\n",inet_ntoa(remote_addr.sin_addr));  
    //len=send(client_sockfd,"Welcome to my server\n",22,0);
          
    while((len=recv(client_sockfd,buf,BUFSIZ,0))>0)  
    {  

        buf[len]='\0'; 

        //printf("buf= %s\n",buf);

		if(strlen(buf)>0){
			deal(buf);

			

			if(send(client_sockfd,buf,len,0)<0)  
			{  
				perror("write error");  
				exit(1);
			}  

		}
    }  
	
	show_output();
      
    close(client_sockfd);  
    close(server_sockfd);  
}

int main(int argc, char **argv)
{
    connect_backend();
    connect_client();
    
    
    close(sockfd);
    close(sockfd2);

    return 0;
}
