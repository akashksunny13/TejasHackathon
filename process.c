#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#define NETWORK_SIZE 100
int AJ[NETWORK_SIZE][NETWORK_SIZE];
int ports[NETWORK_SIZE],portnum=0;
int neighbour[NETWORK_SIZE];
int myNo;
// A normal C function that is executed as a thread when its name
// is specified in pthread_create()
void *Listener(void *vargp)
{
    sleep(1);
    printf("Listening and printing\n");
    listenAndPrint();
    return NULL;
}

void *Sender(void *vargp)
{
    sleep(1);
    sendData();
    printf("In sender\n");
    return NULL;
}

int main()
{
    pthread_t sid, lid;
    printf("Before Thread\n");
    loaddata();
    pthread_create(&sid, NULL, Sender, NULL);
	pthread_create(&lid, NULL, Listener, NULL);
    pthread_join(sid, NULL);
	pthread_join(lid, NULL);
    printf("After Thread\n");
    exit(0);
}


void listenAndPrint()
{
    int listenfd = 0, connfd = 0;
    int length;
    struct sockaddr_in serv_addr; 
	
    char received[1025];

    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    memset(&serv_addr, '0', sizeof(serv_addr));
    memset(received, '0', sizeof(received)); 

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(5001); 

    bind(listenfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)); 

    listen(listenfd, 10); 

    while(1)
    {
		bzero(received, 1025);
		printf("Receiver waiting\n");
        connfd = accept(listenfd, (struct sockaddr*)NULL, NULL);
        printf("Receiver connected\n");
        read(connfd, (char*)&length, sizeof(length));
        printf("%ld\n",length);
        read(connfd,received,length);
        printf("%s\n", received);
        close(connfd);
     }
}
void sendData()
{
	int i=0;
	
	for(;1;++i)
	{
	if(i==portnum)
		i=0;
	int sendfd = 0, connfd = 0;
    int length = 5;
    struct sockaddr_in serv_addr; 
	
    char sendBuff[1025] = "12345";

    sendfd = socket(AF_INET, SOCK_STREAM, 0);
    memset(&serv_addr, '0', sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(ports[i]); 
	sleep(2);
	printf("sender trying\n");
    if(connect(sendfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == 0)
    {
	printf("sender connected\n");
    write(sendfd, (char*)&length, sizeof(length));
    write(sendfd, sendBuff,length);
    close(sendfd);
	}
    }
}

void loaddata()
{
	FILE *file = fopen("nodes", "rb");
	int node,port,distance;
    if (file != NULL)
    {
		int i =0;
        while(fscanf(file,"%d%d%d",&node, &distance, &port) && !feof(file))
        {
			printf("%d %d %d\n", node, distance, port);
			neighbour[node] = distance;
			ports[i++]= port;
			portnum = i;
			}
				
    }
	fclose(file);
	}
