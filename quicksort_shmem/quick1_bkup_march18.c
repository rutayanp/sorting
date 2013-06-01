#include <stdio.h>
#include <shmem.h>
#include <stdlib.h>
#include <time.h>

int quicksort(int *x, int, int);
int partition(int *x, int, int);

int main(int argc, char *argv[]){

	start_pes(0);
	int me = shmem_my_pe();
	int npes = shmem_n_pes();
	int i,N,n,pivot;	
	
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
	
	shmem_barrier_all();
		printf("\nThe sorted list is of process %d: ",me);
		for(i=0;i<n;i++){
			printf("%d,  ",A[i]);
			}
		
printf("\n"); 
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
