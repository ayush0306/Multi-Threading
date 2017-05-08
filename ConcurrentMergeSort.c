#include<stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>

int * input;

void merge(int l1,int r1, int l2, int r2)
{
	int *tmp;
	tmp = (int *)malloc(sizeof(int)*(r2-l1+2));
	int i,tmp1,tmp2;
	tmp1=l1;
	tmp2=l2;
	int t3=l1;

	while(tmp1<=r1 && tmp2<=r2)
	{
		if ( input[tmp1] >  input[tmp2])
		{
			tmp[t3] = input[tmp2] ;
			tmp2++ ;
			t3++ ;
		}

		else
		{
			tmp[t3] = input[tmp1] ;
			tmp1++ ;
			t3++ ;
		}

	}   

	if (tmp1==r1+1)
	{   
		while(tmp2<=r2)
		{
			tmp[t3] = input[tmp2] ;
			tmp2++ ;
			t3++ ;
		}

	}   


	if (tmp2==r2+1)
	{   
		while(tmp1<=r1)
		{
			tmp[t3] = input[tmp1] ;
			tmp1++ ;
			t3++ ;
		}
	}

	for (i=l1 ; i<=r2 ; i++)
		input[i] = tmp[i] ;
	//free(tmp);
	return ;
}


void sort(int l, int r)
{
	if(l==r)
		return ;
	int mid = l+(r-l)/2;
	pid_t left, right ;
	int status;
	int i;

	left = fork();

	if(left<0)
	{
		perror("Could not fork left child");
		exit(1);
	}
	if(left==0)		//child process
	{
		sort(left,mid);
		exit(0);
	}
	else			//continuing for parent process
	{
		right = fork();
		if(right<0)
		{
			perror("Could not fork right child");
			exit(1);
		}
		if(right==0)		//child process
		{
			sort(mid+1,r);
			exit(0);
		}
	}
	waitpid(left, &status, 0);
	waitpid(right, &status, 0);
	merge(l,mid,mid+1,r);
	return ;		
}

int main()
{
	int n;
	scanf("%d",&n);
	int i;
	input = (int *)malloc(sizeof(int)*(n+1));

	/*Taking input*/

	/*Creating a shared memory segment */

	int shm_id;
	key_t key ;
	size_t shm_size = n * sizeof(int);
	if ((shm_id = shmget(key, shm_size, IPC_CREAT | 0666)) == -1) {
		perror("Unable to shmget");
		exit(1);
	}

	/*Attaching the shared memory to inp */

	if((input=shmat(shm_id,NULL,0))==(int*)-1)
	{
		perror("Unable to attach the shared memory");
		exit(1);
	}

	for(i=0;i<n;i++)
		scanf("%d",&input[i]);

	sort(0,n-1);

	for(i=0;i<n;i++)
		printf("%d ",input[i]);
	printf("\n");

	if (shmdt(input) == -1) {
		perror("shmdt");
		exit(1);
	}

	/* Delete the shared memory segment. */
	if (shmctl(shm_id, IPC_RMID, NULL) == -1) {
		perror("shmctl");
		exit(1);
	}

	return 0;
}
