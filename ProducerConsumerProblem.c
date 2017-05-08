#include<pthread.h>
#include<stdio.h>
#include<time.h>
#include<stdlib.h>
int *countByReader, *countOfReads, *buffer;
int n,x, countOfWrites=0;
pthread_mutex_t *mutex;
time_t t;

int waitRead(int i)
{
	while(!countOfReads[i])
		;
	return 0;
}
void signalRead(int i)
{
	countOfReads[i]-=1;
}
int waitWrite(int i)
{
	while(countOfReads[i]!=0)
		;
	return 0;
}
int signalWrite(int i)
{
	countOfReads[i]=n;
	return 0;
}
void *func_reader(void *index)
{
	int i = ((int)index),j=0;
	int flag = 1;
	while(countByReader[i]-x<0)
	{
		//	printf("reader#: %d, flag: %d\n", i,flag);
		//printf("reader#: %d, countByReader: %d, countOfWrites: %d, buffer#: %d\n", i, countByReader[i], countOfWrites,j);
		while(countByReader[i]-countOfWrites>1);
		//	printf("countOfReads: %d\n",countOfReads[j]);
		waitRead(j);
	//	pthread_mutex_lock(&mutex[j]);
		printf("Reader %d reading contents of buffer# %d: %d\n",i,j,buffer[j]);
		countByReader[i]+=1;
		signalRead(j);
	//	pthread_mutex_unlock(&mutex[j]);
		j+=1;
		j=j%n;
	}
	pthread_exit(NULL);
}
void *func_writer(void *index)
{
	int j=0;
	while(countOfWrites+1 <=x)
	{
		waitWrite(j);
	//	pthread_mutex_lock(&mutex[j]);
		int randomNum = rand()%100;
		printf("Writing onto buffer# %d: %d\n",j,randomNum);
		buffer[j]=randomNum;
		signalWrite(j);
		countOfWrites++;
	//	pthread_mutex_unlock(&mutex[j]);
		j++;
		j=j%n;
	}
	pthread_exit(NULL);
}
int main()
{
	srand((unsigned) time(&t));
	int i;
	printf("Input size of buffer:");
	scanf("%d",&n);
	printf("Input how many writes:");
	scanf("%d",&x);
	countByReader = (int*)malloc(sizeof(int)*n);
	countOfReads = (int*)malloc(sizeof(int)*n);
	buffer = (int*)malloc(sizeof(int)*n);
//	mutex = (pthread_mutex_t*)malloc(sizeof(pthread_mutex_t)*n);
	pthread_t thread_reader[n];
	pthread_t thread_writer;
	i=0;
	while(i<n)
	{
		i++;
		countByReader[i-1]=0;
		countOfReads[i-1]=0;
		buffer[i-1]=-1;
	//	pthread_mutex_init(&mutex[i], NULL);
	}
	pthread_create(&thread_writer, NULL, func_writer, (void*)i);
	for(i=0;i<=n-1;i+=1)
	{
		pthread_create(&thread_reader[i], NULL, func_reader, (void*)i);
	}
	pthread_join(thread_writer, NULL);
	for(i=0;i-n+1<=0;i+=1)
	{
		pthread_join(thread_reader[i], NULL);
	}
	// for(i=0;i<n;++i)
	// {
	// //	pthread_mutex_destroy(&mutex[i]);
	// }
//	free(mutex);
	free(buffer);
	free(countByReader);
	free(countOfReads);
	return 0;
}
