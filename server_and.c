#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#define LEN 10
int num = 0;
int or_port = 21575;
int and_port = 22575;


void deal_and(char * recv,char* send){
	int i;
    char number1[LEN+1];	
    char number2[LEN+1];	
    char *p = recv;

    number1[LEN]='\0';
    number2[LEN]='\0';

    for(i=0;i<LEN;i++)
    {
        number1[i]='0';
        number2[i]='0';
    }

    char*p1,*p2;
    p1 = strchr(recv,' ');
    int length = p1-recv;
    memcpy(number1+LEN-length,recv,length);
    //length = strlen(p1+1);
    p2 = strchr(p1+1,' ');
	length = p2-p1-1;
	
    memcpy(number2+LEN-length,p1+1,length);

	//printf("num1 %s\n",number1);
	//printf("num2 %s\n",number2);


    for(i=0;i<LEN;i++)
    {
        if(number1[i]=='1'&&number2[i]=='1')
            number1[i]='1';
        else
            number1[i]='0';
    }
    strcpy(send,number1);
}

void connect_edge()
{
   
    int sockfd;
    struct sockaddr_in servaddr,remoteaddr;

    sockfd = socket(PF_INET, SOCK_DGRAM, 0);

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(and_port);

	socklen_t remoteaddrLength = sizeof(remoteaddr);

    bind(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr));

    int n;
    char recvline[200];
    char sendline[11];
printf("The Server AND is up and running using UDP on port 22575\n");
    printf("The Server AND start receiving lines from the edge server for AND computation.The computation results are:\n");

    while(1){
        recvfrom(sockfd, recvline, 200, 0, (struct sockaddr*)&remoteaddr,&remoteaddrLength);

        deal_and(recvline,sendline);    

        sendto(sockfd, sendline, strlen(sendline)+1, 0, (struct sockaddr *)&remoteaddr, sizeof(remoteaddr));

        recvfrom(sockfd, recvline, 200, 0, (struct sockaddr*)&remoteaddr,&remoteaddrLength);

		sendline[0]='2';
		sendline[1]='\0';
        sendto(sockfd, recvline, strlen(recvline)+1, 0, (struct sockaddr *)&remoteaddr, sizeof(remoteaddr));

        printf("%s\n", recvline);
		num++;
    }

    close(sockfd);
    printf("The Server AND has successfully received %d lines from the edge server and finished all AND computations.\n",num);
    printf("The Server AND has successfully finished sending all computation results to the edge server.\n");	
}

int main(int argc, char **argv)
{
    connect_edge();

    return 1;
}
