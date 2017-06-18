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
#include <limits.h>
#define NETWORK_SIZE 6
#define INFINITY -1
#define DATA_RELOAD_TIME 5
int AJ[NETWORK_SIZE][NETWORK_SIZE];
int Temp[NETWORK_SIZE][NETWORK_SIZE];
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
	int i;
while(1)
	{
		if(dataReady)
		{
			printf("\n\n");
			printAJ();
			printf("\n\n");
			memcpy(Temp, AJ, sizeof(int) * NETWORK_SIZE * NETWORK_SIZE);
			dataReady = 0;
			
			for (i = 0; i < NETWORK_SIZE; i++)
				Temp[i][i]=0;
			calculateAndPrintSDM(Temp);
			calculateAndPrintMST(Temp);
		}	
	}	}
	
void *Sender(void *vargp)
{
    while(1)
    {
		sleep(1);
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
	memset(AJ, INFINITY, sizeof(int) * NETWORK_SIZE * NETWORK_SIZE);
    int connfd = 0;
    int nodenum;
	
	int AJTemp[NETWORK_SIZE][NETWORK_SIZE];


	time_t endTime = time(NULL) + DATA_RELOAD_TIME;
    while(time(NULL) < endTime)
    {
		memset(AJTemp, INFINITY, sizeof(int) * NETWORK_SIZE * NETWORK_SIZE);
		//printf("Receiver waiting\n");
        connfd = accept(listenfd, (struct sockaddr*)NULL, NULL);
        //if(connfd ==0)
			//printf("Oh, something went wrong with connect()! %s\n", strerror(errno));
        //printf("Receiver connected\n");
        read(connfd, (char*)&nodenum, sizeof(nodenum));
        //printf("%d\n",nodenum);
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
	//printf("sender trying to send to:%d\n", ports[i]);
    if(connect(sendfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == 0)
    {
	//printf("sender connected\n");
    write(sendfd, (char*)&myNo, sizeof(myNo));
    write(sendfd, (char*)AJ,sizeof(int) * NETWORK_SIZE * NETWORK_SIZE);
    //printf("sender wrote\n");
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

void calculateAndPrintSDM(int mat[NETWORK_SIZE][NETWORK_SIZE])
{
    int dist[NETWORK_SIZE][NETWORK_SIZE], i, j, k;
    for (i = 0; i < NETWORK_SIZE; i++)
        for (j = 0; j < NETWORK_SIZE; j++)
        {
            dist[i][j] = mat[i][j];
            if(dist[i][j] == -1)
				dist[i][j] = 9999;
		}
    for (k = 0; k < NETWORK_SIZE; k++)
    {
        for (i = 0; i < NETWORK_SIZE; i++)
        {
            for (j = 0; j < NETWORK_SIZE; j++)
            {
                if ((dist[i][k] + dist[k][j] < dist[i][j] )&& (dist[i][k]!=-1) && (dist[k][j]!=-1))
                    dist[i][j] = dist[i][k] + dist[k][j];
            }
        }
        }
        printf("The shortest path b/w nodes ia given by\n");
        printMat(dist);
        printf("\n\n");
        }
void printMat(int graph[NETWORK_SIZE][NETWORK_SIZE])
{
	int i,j;
	for(i=0;i<NETWORK_SIZE;++i)
	{
	for(j=0;j<NETWORK_SIZE;++j)
	{
		printf("%d\t",graph[i][j]);
	}
	printf("\n");
	}
}

void calculateAndPrintMST(int mat[NETWORK_SIZE][NETWORK_SIZE])
{
	int i,j;
	int parent[NETWORK_SIZE];
	memset(parent, INFINITY, sizeof(int) * NETWORK_SIZE );
     int key[NETWORK_SIZE];
     int mstSet[NETWORK_SIZE]; 
     for (i = 0; i < NETWORK_SIZE; i++)
        for (j = 0; j < NETWORK_SIZE; j++)
        {
            if(mat[i][j] == -1)
				mat[i][j] = 0;
		}
     for (i = 0; i < NETWORK_SIZE; i++)
        key[i] = INT_MAX, mstSet[i] = 0;
     key[myNo] = 0; 
     parent[myNo] = -2;
	//printf("Before count\n");
     for (int count = 0; count < NETWORK_SIZE-1; count++)
     {
        int u = minKey(key, mstSet);
        mstSet[u] = 1;
        for (i = 0; i < NETWORK_SIZE; i++)
          if (mat[u][i] && mstSet[i] == 0 && mat[u][i] <  key[i])
             parent[i]  = u, key[i] = mat[u][i];
     }
     //printf("Print MST\n");
     printMST(parent, NETWORK_SIZE, mat);
}

int printMST(int parent[], int n, int graph[NETWORK_SIZE][NETWORK_SIZE])
{
   //printf("Edge   Weight\n");
   for (int i = 0; i < NETWORK_SIZE; i++)
   {
	   if(i==myNo)
		continue;
	   if(parent[i] == -1)
		continue;
      printf("%d - %d    %d \n", parent[i], i, graph[i][parent[i]]);
  }
}

int minKey(int key[], int mstSet[])
{
   int min = INT_MAX, min_index;
 
   for (int v = 0; v < NETWORK_SIZE; v++)
     if (mstSet[v] == 0 && key[v] < min)
         min = key[v], min_index = v;
 
   return min_index;
}
