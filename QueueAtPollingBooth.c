#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<time.h>
#include<sys/shm.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<sys/ipc.h>
#include<pthread.h>
#include<semaphore.h>

#define maxV 1000
#define maxE 1000

int n,waitvar=0;
int glEvmCnt=0,glVoterCnt=0 ;

typedef struct booth {
	int boothid;
	int evmCnt;
	int evmSlot;
	int voterCnt;
	pthread_mutex_t lockevm ;
	pthread_mutex_t lockstruct ;
	pthread_mutex_t lockcast ;
	int waiting ;
	int casted;
	int ready;
	int tmp;
	int alloted;
	int waitevm;
}booth ;

void polling_ready_evm(booth *b, int count, int id)
{
	//pthread_mutex_lock(&b->lockstruct);
	//pthread_mutex_lock(&b->lockprint);
	printf("Evm no %d at booth no.%d ready with slots %d\n",id,b->boothid,count);
	b->ready=1;
	//pthread_mutex_unlock(&b->lockprint);
	while(1)
	{
		while(b->waitevm==1);
		b->waitevm=1;
		while(b->waitevm==1);
		if((b->casted)+(b->waiting)==(b->voterCnt) || (b->waiting)==count)
			break;
	}
	//pthread_mutex_lock(&b->lockprint);
	b->ready=0;
	//printf("No more ready\n");
	//printf("After filling evm the no. of voter waiting/ready to be alloted are : %d\n",b->waiting);
	//pthread_mutex_unlock(&b->lockprint);
	//pthread_mutex_lock(&b->lockprint);
	if((b->casted)+(b->waiting)==(b->voterCnt))
	{
		b->alloted=b->waiting;
		b->waiting=0;
		//b->casted+=waiting ;
	}
	else
	{
		b->waiting-=count;
		b->alloted = count ;
		//b->casted+=count;
	}
	//printf("The no. of voters waiting is %d and alloted is %d\n",b->waiting,b->alloted);
	while(b->alloted>0);
	//pthread_mutex_unlock(&b->lockprint);
	//pthread_mutex_unlock(&b->lockstruct);
	return ;
}

void voter_wait_for_evm(booth *b)
{
	int count=0 ;
	//printf("Voter waiting\n");
	//pthread_mutex_lock(&b->lockprint);
	while(b->ready!=1)
		count++;
	//printf("Voter waited for %d\n",count);
	//pthread_mutex_unlock(&b->lockprint);
	return;
}

void voter_in_slot(booth *b)
{
	pthread_mutex_lock(&b->lockstruct);
	while(b->waitevm==0);
	(b->waiting)++;
	//printf("No. of voters waiting increased to : %d\n",b->waiting);
	b->waitevm=0;
	pthread_mutex_unlock(&b->lockstruct);
	return;
}

void evm_cast_vote(booth *b)
{
	//pthread_mutex_lock(&b->lockprint);
	(b->casted)++;
	(b->alloted)--;
//	printf("No. of votes casted increased to : %d\n",b->casted);
	//pthread_mutex_unlock(&b->lockprint);
	return ;
}

void vote(booth *b)
{
	voter_wait_for_evm(b);
	voter_in_slot(b);
	printf("Voter is waiting at booth no. %d\n",b->boothid);
	pthread_mutex_lock(&b->lockcast);
	while(b->alloted==0);
	//pthread_mutex_lock(&b->lockstruct);
	evm_cast_vote(b);
	//pthread_mutex_unlock(&b->lockstruct);
	pthread_mutex_unlock(&b->lockcast);
	glVoterCnt--;
	printf("Voter casted his vote at booth %d and now voters left is %d\n",b->boothid,b->voterCnt-b->casted);
	return ;
}

void control(booth *b)
{
	int evm_id = b->tmp ;
	waitvar=1;
	while(1)
	{
		pthread_mutex_lock(&b->lockevm);
		if(b->casted==b->voterCnt)
		{
			pthread_mutex_unlock(&b->lockevm);
			break;
		}
		polling_ready_evm(b,(rand()%(b->evmSlot))+1,evm_id);
		//evm_cast_vote();
		pthread_mutex_unlock(&b->lockevm);
	}
	glEvmCnt--;
	//printf("An evm is returning and glVoterCnt is %d and glEvmcnt is %d\n",glVoterCnt,glEvmCnt);
	return ;
}

int main()
{
	booth b[10];
	pthread_t voter[maxV];
	pthread_t evm[maxE];
	int i,j;
	time_t t;
	srand((unsigned) time(&t));
	printf("Enter the number of booth : ");
	scanf("%d",&n);
	for(i=0;i<n;i++)
	{
		printf("Allot the number of voters to booth%d : ",i);
		scanf("%d",&b[i].voterCnt);
		printf("Allot the number of evm to booth%d : ",i);
		scanf("%d",&b[i].evmCnt);
		printf("Allot the number of max Slots to this booth : ");
		scanf("%d",&b[i].evmSlot);
		b[i].boothid=i;
		b[i].waiting = 0;
		b[i].casted = 0;
		b[i].ready = 0;
		b[i].alloted = 0;
		b[i].waitevm=0;
		pthread_mutex_init(&b[i].lockevm,NULL);
		pthread_mutex_init(&b[i].lockstruct,NULL);
		pthread_mutex_init(&b[i].lockcast,NULL);
	}
	glEvmCnt=0;glVoterCnt=0;
	for(i=0;i<n;i++)
	{
		for(j=0;j<b[i].evmCnt;j++)
		{
			b[i].tmp = j;
			pthread_create(&evm[glEvmCnt],NULL,(void *)control,&b[i]);
			glEvmCnt++;
			while(waitvar==0);
			waitvar=0;
		}
		for(j=0;j<b[i].voterCnt;j++)
		{
			pthread_create(&voter[glVoterCnt],NULL,(void *)vote,&b[i]);
			glVoterCnt++;
		}
	}
	/*for(i = 0; i < glEvmCnt; i++)
		pthread_join(evm[i], NULL);
	for(i = 0; i < glVoterCnt; i++)
		pthread_join(voter[i], NULL);
	*/
	while(glEvmCnt>0 || glVoterCnt>0 );
	return 0;
}
