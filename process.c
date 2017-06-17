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
#define NETWORK_SIZE 6
#define INFINITY -1
#define DATA_RELOAD_TIME 5
int AJ[NETWORK_SIZE][NETWORK_SIZE];
int ports[NETWORK_SIZE],portnum=0;
int neighbour[NETWORK_SIZE];
int myNo;
int dataReady = 0;
int listenfd;
int myPort;
void printAJ();
void updateAJ(int [NETWORK_SIZE][NETWORK_SIZE]);
void loaddata();
void sendData();
void listenAndPrint();
void *Listener(void *vargp)
{
	listenfd = socket(AF_INET, SOCK_STREAM, 0);
			struct sockaddr_in serv_addr; 
			memset(&serv_addr, '0', sizeof(serv_addr));
			serv_addr.sin_family = AF_INET;
			serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
			serv_addr.sin_port = htons(myPort);
			bind(listenfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr));  
			listen(listenfd, 1000); 
	while(1)
	{
		    
			listenAndPrint();
	}
    return NULL;
}
void *Intellect(void *vargp)
{
while(1)
	{
		if(dataReady)
		{
			printAJ();
			dataReady = 0;
		}	
	}	}
	
void *Sender(void *vargp)
{
    while(1)
    {
		sleep(2);
		sendData();
	}
    //printf("In sender\n");
    return NULL;
}

int main(int argc, char *argv[])
{
    pthread_t sid, lid, aid;
    myPort = atoi(argv[1]);
    myNo = atoi(argv[2]);
    //printf("Before Thread\n");
    loaddata(argv[1]);
    pthread_create(&sid, NULL, Sender, NULL);
	pthread_create(&lid, NULL, Listener, NULL);
	pthread_create(&aid, NULL, Intellect, NULL);
    pthread_join(sid, NULL);
	pthread_join(lid, NULL);
	pthread_join(aid, NULL);
    //printf("After Thread\n");
    exit(0);
}


void listenAndPrint()
{
	if(dataReady)
		return;
	//printf("listenAndPrint%d\n", dataReady);
	memset(AJ, INFINITY, sizeof(AJ[0][0]) * NETWORK_SIZE * NETWORK_SIZE);
    int connfd = 0;
    int nodenum;
	
	int AJTemp[NETWORK_SIZE][NETWORK_SIZE];



	time_t endTime = time(NULL) + DATA_RELOAD_TIME;
    while(time(NULL) < endTime)
    {
		memset(AJTemp, INFINITY, sizeof(AJTemp[0][0]) * NETWORK_SIZE * NETWORK_SIZE);
		//printf("Receiver waiting\n");
        connfd = accept(listenfd, (struct sockaddr*)NULL, NULL);
        //if(connfd ==0)
			//printf("Oh, something went wrong with connect()! %s\n", strerror(errno));
        //printf("Receiver connected\n");
        read(connfd, (char*)&nodenum, sizeof(nodenum));
        printf("%d\n",nodenum);
        AJ[nodenum][myNo] = AJ[myNo][nodenum] = neighbour[nodenum];
        read(connfd,(char*)AJTemp,sizeof(int) * NETWORK_SIZE * NETWORK_SIZE);
        close(connfd);
        //printf("BEfore updating\n");
        updateAJ(AJTemp);
     }
     dataReady = 1;
}
void sendData()
{
	int i=0;
	
	for(;i<portnum;++i)
	{
	int sendfd = 0, connfd = 0;
    int length = 5;
    struct sockaddr_in serv_addr; 
    sendfd = socket(AF_INET, SOCK_STREAM, 0);
    memset(&serv_addr, '0', sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    serv_addr.sin_port = htons(ports[i]); 
	sleep(2);
	//printf("sender trying to send to:%d\n", ports[i]);
    if(connect(sendfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == 0)
    {
	//printf("sender connected\n");
    write(sendfd, (char*)&myNo, sizeof(myNo));
    write(sendfd, (char*)AJ,sizeof(int) * NETWORK_SIZE * NETWORK_SIZE);
    //printf("sender wrote\n");
	}
	else
	{
		  //printf("Oh, something went wrong with connect()! %s\n", strerror(errno));
	}
	
    close(sendfd);
    }
}

void loaddata(char **fileName)
{
	FILE *file = fopen(fileName, "rb");
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
void updateAJ(int mat[NETWORK_SIZE][NETWORK_SIZE])
{
	//printf("In update aj\n");
	int i,j;
	for(i=0;i<NETWORK_SIZE;++i)
	{
		//printf("In i loop%d\n",i);
		if(i==myNo)
		continue;
	for(j=0;j<NETWORK_SIZE;++j)
	{
		//printf("In j loop%d\n",j);
		if(j==myNo)
			continue;
			if(AJ[i][j]== -1)
			{
				//printf("Trying to assign from mat");
			AJ[i][j] = mat[i][j];
			//printf("Assigned from mat");
		}
	}
	}
}
void printAJ()
{
	int i,j;
	for(i=0;i<NETWORK_SIZE;++i)
	{
	for(j=0;j<NETWORK_SIZE;++j)
	{
		printf("%d\t",AJ[i][j]);
	}
	printf("\n");
	}
}
