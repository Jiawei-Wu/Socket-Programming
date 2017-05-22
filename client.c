#include <stdio.h>  
#include <sys/types.h>  
#include <sys/socket.h>  
#include <netinet/in.h>  
#include <arpa/inet.h>  
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

#define MAXLINES 100
#define LEN 10

char ip[20]="127.0.0.1";
int edge_port = 23575;
char output[MAXLINES][LEN+1];
int num;

void show_output(){
	int i;

	printf("The client has successfully finished sending %d lines to the edge server.\n",num);
	printf("The client has successfully finished receiving all computation results from the edge server.\n");

	printf("The final computation results are:\n");

	for(i=0;i<num;i++)
	{
		printf("%s\n",output[i]);
	}
}

void connect_edge(FILE * fp)
{

    int client_sockfd;  
    int len;  
    struct sockaddr_in remote_addr; 
    char buf[BUFSIZ];  
    memset(&remote_addr,0,sizeof(remote_addr)); 
    remote_addr.sin_family=AF_INET; 
    remote_addr.sin_addr.s_addr=inet_addr(ip);
    remote_addr.sin_port=htons(edge_port);   
    
    if((client_sockfd=socket(PF_INET,SOCK_STREAM,0))<0)  
    {  
        perror("socket error");  
        exit(1);  
    }  

    if(connect(client_sockfd,(struct sockaddr *)&remote_addr,sizeof(struct sockaddr))<0)  
    {  
        perror("connect error");  
        exit(1);
    } 

    printf("The client is up and running.\n");  
    //len=recv(client_sockfd,buf,BUFSIZ,0);  
    //buf[len]='\0';  
    //printf("%s",buf);

	num = 0;
    while(fgets(buf,sizeof(buf),fp))  
    {  
		//printf("bf = %s",buf);
        if(!strcmp(buf,"quit"))  
            break;  

        len=send(client_sockfd,buf,strlen(buf),0);  
		buf[0]='\0';

        len=recv(client_sockfd,buf,BUFSIZ,0);  
        buf[len]='\0';  
        //printf("received:%s\n",buf);  
		strcpy(output[num],buf);
		output[num++][LEN] = '\0';

    }  
    
    close(client_sockfd); 

	show_output();

}

int main(int argc, char *argv[])  
{  
	if(argc!=2){
		
		printf("Error:<usage>./client <input_file_name>\n");
		exit(1);
	}


	FILE * fp = fopen(argv[1],"r");	

	if(fp==NULL)
		exit(1);

    connect_edge(fp); 
  
	fclose(fp);

    return 0;  
}  
