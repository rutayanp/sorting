//edited in march 19th ..status : broadcasting of pivot done...rank 0 not done
#include <stdio.h>
#include <shmem.h>
#include <stdlib.h>
#include <time.h>

int quicksort(int *x, int, int);
int partition(int *x, int, int);

int main(int argc, char *argv[]){
	
		
	int i,N,n,pivot;	
	long pSync[_SHMEM_BCAST_SYNC_SIZE];
	
	for (i=0; i < SHMEM_BCAST_SYNC_SIZE; i++) {
 		pSync[i] = _SHMEM_SYNC_VALUE;
		}	
	
	start_pes(0);
	int me = shmem_my_pe();
	int npes = shmem_n_pes();
	shmem_barrier_all();
	srand (me+time(NULL));

	N = atoi(argv[1]);
	printf("%d: Size = %d with np=%d\n",me,N,npes);
	int *A = (int *)shmalloc((N/npes)*sizeof(int));
	if(A==NULL){
		printf("\nOut of memory");
		return 1;
	}
	n= N/npes;
	i=0;
	while(i<N/npes){
		A[i] = rand()%(10000-0);
		i++;
	}
	printf("\nprocess %d elements:",me);
	for(i=0;i<(N/npes);i++){
                printf("%d, ", A[i]);
       		}

	pivot=quicksort(A, 0, n-1);
	printf("Process %d the pivot:%d",me, pivot);
	
	shmem_barrier_all(); //just for the sake of clear display...can be removed in the end
	printf("\nThe sorted list is of process %d: ",me);
	for(i=0;i<n;i++){
		printf("%d,  ",A[i]);
		}
	printf("\n");
	
	//the step two of algo.....broadcast the new pivot
	shmem_broadcast32(&pivot,A,1,0,0,0,npes,pSync);	
	printf("the new pivot of process %d: %d\n",me,pivot); // to check the broadcast of new pivots

	if(me == 0){
		pivot = A[pivot];
		printf("the new pivot of process %d : %d\n",me,pivot);
	}

shfree(A);
shmem_finalize();
}

int quicksort(int *A, int p, int q){
	int r;
//	printf("hello from quicksort\n");
	if (p < q){
		r = partition(A, p, q);
		quicksort(A,p,r-1);
		quicksort(A,r+1,q);
		}
	return r;
	
	}
	
int partition(int *A, int p, int q){
	int x,i,j,temp;
//	printf("hello from partition\n");
	x= A[q];
	i=p-1;
	for(j=p;j<q;j++){
		if(A[j] <= x){
			i=i+1;
			temp = A[i];
			A[i] = A[j];
			A[j]= temp;
			}
		}
	temp = A[i+1];
	A[i+1] = A[q];
	A[q]=temp;
	return i+1;
} 
