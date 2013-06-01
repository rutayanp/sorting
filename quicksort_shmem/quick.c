#include <stdio.h>
#include <shmem.h>
#include <stdlib.h>
#include <time.h>

int quicksort(int *x, int, int);
int partition(int *x, int, int);
int uplowPartition(int);
int N;
int nelems_import[1],nelems[1];
int pe; 
int *A,*temp_arr;
int me,npes;

int main(int argc, char *argv[]){
	
		
	int i,n,next_pivot, pivot;	
	long pSync[_SHMEM_BCAST_SYNC_SIZE];
	
	for (i=0; i < SHMEM_BCAST_SYNC_SIZE; i++) {
 		pSync[i] = _SHMEM_SYNC_VALUE;
		}	
	
	start_pes(0);
	me = shmem_my_pe();
	npes = shmem_n_pes();
	shmem_barrier_all();
	srand (me+time(NULL));

	N = atoi(argv[1]);
	
	//int *nelems = (int*) shmalloc(sizeof(int));

	//int *nelems_import= (int*) shmalloc(sizeof(int));;
	printf("%d: Size = %d with np=%d\n",me,N,npes);
	A = (int *)shmalloc((N/npes)*sizeof(int));
	temp_arr = (int *)shmalloc((N/npes)*sizeof(int));
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
	
	next_pivot = A[0];
	
	//the step two of algo.....broadcast the new pivot
	shmem_broadcast32(&next_pivot,A,1,0,0,0,npes,pSync);	
	shmem_barrier_all();	
	pivot = quicksort(A, 0, n-1);
	printf("Process %d the pivot:%d",me, pivot);
	
	shmem_barrier_all(); //just for the sake of clear display...can be removed in the end
	printf("\nThe sorted list is of process %d: ",me);
	for(i=0;i<n;i++){
		printf("%d,  ",A[i]);
		}
	printf("\n");
	printf("the new pivot of process %d: %d\n",me,next_pivot); // to check the broadcast of new pivots
	
	int check,j; //to check the division of the sorted arrays according to the new pivot.
	shmem_barrier_all();
	check = uplowPartition(next_pivot);
	shmem_barrier_all();	
	printf("(%d)",me);	
	for(int j=0;j<N/npes;j++){
		printf("%d, ",A[j]);
		}
	printf("new partition: %d",check);
	shmem_barrier_all();
	if(me < npes/2)
		{
		i=0;
	//	printf("Hello from %d", me);
		printf("\n");
		for(j=check;j<N/npes;j++){
			temp_arr[i] = A[j];
			i++;
			}
		i=0;
		printf("(%d)",me);
		for(j=check;j<N/npes;j++){
                        printf("%d, ",temp_arr[i]) ;
			i++;
                	}
//	printf("\n");
	}
	
	shmem_barrier_all();
	if(me >= npes/2)
		{
		
	//	printf("Hello from %d", me);
		printf("\n");
		for(j=0;j<check;j++){
			temp_arr[j] = A[j];
			}
		
		printf("(%d)",me);
		for(j=0;j<check;j++){
                        printf("%d, ",temp_arr[j]) ;
			
                	}
//	printf("\n");
	}
	shmem_barrier_all();	
	printf("\n");
	
	if(me < npes/2){
		printf("\n");
		pe = me +npes/2;
		nelems[0] = N/npes - check;
		printf (" process %d pe : %d nelems : %d\n",me,pe,nelems[0]);//to test the value

		printf("(%d) addr = %d , value = %d , pe = %d\n ",me, &nelems_import[0],nelems[0],pe);//to test the value

		shmem_int_p(nelems_import,nelems[0],pe);
		shmem_quiet();
		shmem_int_put(temp_arr,&A[check],nelems[0],pe);
	}

	shmem_barrier_all();//check if the entire barrier is needed
	if(me >= npes/ 2){
		
		pe = me-npes/2;//check if it is synced
		nelems[0]= check;
		printf (" process %d pe : %d nelems : %d\n",me,pe,nelems[0]);//to test the value
		shmem_int_p(nelems_import,nelems[0],pe);
		shmem_quiet();
		shmem_int_put(temp_arr,A,nelems[0],pe);
	}
	
	shmem_barrier_all();//again sync is required...check it with profiling
//this snippet is to check if the processors have got the high and low lists respectively	-------------------
		printf("(%d) nelems_import = %d\n",me,nelems_import[0]);//to test the value
                printf("(%d) new elements = ",me);
                for(i=0;i<nelems_import[0];i++){
                        printf("%d, ",temp_arr[i]);
                }
                printf("\n");
//------------------------------------here this checking snippet ends----

//----------------------------------merging of arrays begin-------------------------
	if(me < npes/2){
		i=0;
		for(j=nelems_import[0];j<(nelems_import[0]+check);j++){
		
			temp_arr[j] = A[i];
			i++;
		}

	}
	

	if(me >= npes/2){
		i=check;
		for(j=nelems_import[0];j<(nelems_import[0]+N/npes-check);j++){
		
			temp_arr[j] = A[i];
			i++;
		}

	}

	shmem_barrier_all(); //to test if the arrays are merged properly
	int size;
	if(me < npes/2){	
		size = (nelems_import[0]+check);
		printf("(%d) merged array:",me);
		for(j=0;j<size;j++){
			printf("%d, ",temp_arr[j]);
		}
		printf("\n");
	}
		
	if(me >= npes/2){
		size = (nelems_import[0]+N/npes-check);
		printf("(%d) merged array:",me);
		for(j=0;j<size;j++){
			printf("%d, ",temp_arr[j]);
		}
	printf("\n");
	}
			//-----------------------check of merging finishes--------
//--------------------------------------------------merging finishes------------------------------

//-----------------------sort again-----------------------------------------------	
	if(me < npes/2){
		quicksort(temp_arr,0,(nelems_import[0]+check-1));	
	}
	
	if(me >= npes/2){
		quicksort(temp_arr,0,(nelems_import[0]+N/npes-check-1));
	}
	//sorting routine checked...once program is done we can remove this part-------------
	shmem_barrier_all();//test purpose only
	if(me < npes/2){
		printf("(%d) sorted list: ",me);
		for(i=0;i<size;i++){
			printf("%d, ",temp_arr[i]);
		}
		printf("\n");
	}
	
	
	if(me >= npes/2){
		printf("(%d) sorted list: ",me);
		for(i=0;i<size;i++){
			printf("%d, ",temp_arr[i]);
		}
		printf("\n");
	}
	//-------------------------------------------------------------
//---------------------------------------------------------------------------------
	
shfree(temp_arr);
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

int uplowPartition( int next_pivot){
	int temp,j,i= -1;
//	int *temp_arr;
	for(j=0;j<N/npes;j++){
		if(A[j] <next_pivot) {
			i=i+1;
			temp = A[i];
			A[i] = A[j];
			A[j] = temp;		
			}
		}
	//printf("%d\n",npes);
	return i+1;

}
