#include <shmem.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int *me,N,total_npes;
int *next_pivot;
int me_import=0;
int *nelems_import,*nelems ;
int *temp_arr;
long pSync[_SHMEM_BCAST_SYNC_SIZE];


int uplowPartition(int *A, int *new_pivot, int);
int exchange(int start, int npes, int *A, int size, int nelems);
int recurse(int, int, int *A, int);
int partition(int *A, int p, int q);
int quicksort(int *A, int p, int q);

int main (int argc, char *argv[])
{
	int i;
	for (i=0; i < SHMEM_BCAST_SYNC_SIZE; i++) {
 		pSync[i] = _SHMEM_SYNC_VALUE;
		}
	

	start_pes (0);
	
	me = (int *)shmalloc(sizeof(int));
	me[0] = shmem_my_pe ();
	int npes = shmem_n_pes ();
	
	srand (me[0]+time(NULL));
	N = atoi(argv[1]);
	total_npes = npes;
 //       printf("%d: Size = %d with np=%d\n",me[0],N,npes);
	//TODO check out of memory exception for all memory allocations
        int *A = (int *)shmalloc(2*(N/npes)*sizeof(int));

	next_pivot = (int *)shmalloc(sizeof(int));
        
	if(A==NULL){
                printf("\nOut of memory");
                return 1;
        }
        //n= N/npes;
        i=0;
        while(i<N/npes){
                A[i] = rand()%(100000-0);
                i++;
        }
        /*printf("\nprocess %d elements:",me[0]);
        for(i=0;i<(N/npes);i++){
                printf("%d, ", A[i]);
                }
	printf("\n");

	shmem_barrier_all();*/
	quicksort(A, 0, N/npes-1);
	recurse(0,npes,A,N/npes); //let this function be commented for now. I am using something similar for my hyperquicksort.
	
	
	shfree(A);
	shfree(next_pivot);	
	shmem_finalize ();
}

int uplowPartition(int *A, int *next_pivot, int size){
	int temp,j,i= -1;
	for(j=0;j<size;j++){
		if(A[j] <next_pivot[0]) {
			i=i+1;
			temp = A[i];
			A[i] = A[j];
			A[j] = temp;		
			}
		}
	//printf("%d\n",npes);
	return i+1;	
}

int recurse(int start, int npes, int *A, int size){
	
	int npe,check,j, next_size;
        temp_arr = (int *)shmalloc(2*(N/npes)*sizeof(int));
	
	if(npes > 1){
		next_pivot[0] = A[size/2];
		//shmem_barrier(start,0,npes,pSync);
		shmem_broadcast32(next_pivot,next_pivot,1,0,start,0,npes,pSync);
		shmem_barrier(start,0,npes,pSync);
		if(me[0] == start) next_pivot[0] = A[size/2];
		
	//printf("DEBUG(%d) next_pivot: %d \n", me[0],next_pivot[0]);
		npe = npes/2;
	// here the first part of the pseudo code finishes--------------------------------------------------------------------

		check = uplowPartition(A,next_pivot, size);	
		shmem_barrier_all();	
	/*	printf("DEBUG (%d)up low array: ",me[0]);
		for(j=0;j<size;j++){
			printf("%d ,",A[j]);
		}
	 	
		printf("(%d) check : %d\n",me[0],check);
		shmem_barrier_all();*/
		//printf("DEBUG_recurse (%d) , \n",me[0]);
		next_size = exchange(start, npes, A, size, check); //function call
		shmem_barrier_all();
		quicksort(temp_arr,0,next_size-1);
	
/*	printf(" DEBUG(%d) next_size = %d :: temp_arr[] = ", me[0],next_size);
		
		for( j=0 ; j < next_size; j++){
			printf("%d ,",temp_arr[j]);
		}
		shmem_barrier_all();	//check the need here...mainly for debugging
		printf("\nDEBUG(%d) transfer back to A[] ",*me);*/

//this loop is to store back the temp_arr into A
		for( j=0; j <next_size;j++){
			A[j] = temp_arr[j];
		}
		
//		printf("\n"); //DEBUG
		shmem_barrier_all();//DEBUG
		shfree(temp_arr);	
			
		if(me[0]>= start && me[0] < (npe+start)) 
			recurse(start,npe, A,next_size);
		else if(me[0] >= (npe+start) && me[0] < npes+start)
			recurse((start+npe),npe, A,next_size);
            
	}
	else{
		//display the sorted list
		for(int i=0;i<total_npes;i++){
			if(i == *me){
				for(j=0;j<size;j++){
					printf("(%d) Sorted list: %d \n",*me,A[j]);
				}
			}
		shmem_barrier_all();
		}
		printf("\n");
		return 0;
	}
}

int exchange(int start, int npes, int *A, int size, int check){
	
	int pe, i, j=0, next_size;

	nelems_import = (int*)shmalloc(sizeof(int));
	nelems = (int*) shmalloc(sizeof(int));	
	shmem_barrier_all();
	//printf("DEBUG (%d), start : %d, #PE: %d \n",me[0],start, npes);
	
	if(me[0] >= start && me[0] < (npes/2+start)){
		pe = me[0] + npes/2;
		*nelems = size - check;
	//printf( " (%d) TO (%d) no. of elements to be exported: %d  \n ", me[0],pe, *nelems);
		shmem_int_p(nelems_import, *nelems, pe);
		shmem_quiet();
		shmem_int_put(temp_arr, &A[check],*nelems,pe);
		
	}
	
	if(me[0] >= (npes/2+start) && me[0] < npes+start){
		pe = me[0] -npes/2;
		*nelems = check;
	//printf( " (%d) TO (%d) no. of elements to be exported: %d  \n ", me[0],pe, *nelems);
		shmem_int_p(nelems_import, *nelems,pe);
		shmem_quiet();
		shmem_int_put(temp_arr,A, *nelems, pe);
	}
	
	shmem_barrier_all();
//printf(" (%d) nelems_import = %d \n", me[0],*nelems_import);
//	shmem_barrier_all();		
/*	printf("DEBUG (%d)temp_arr : ",me[0]);
	for(i = 0; i < *nelems_import;i++){
		printf("%d ,",temp_arr[i]);
	}
	printf("\n");

	shmem_barrier_all();*/	
	//Merging of the imported data and the existing data
	if(me[0] >= start && me[0] < (npes/2+start)){
		j=0;
		for(i= *nelems_import;i< (*nelems_import + check) ; i++){
			temp_arr[i] = A[j];
			j++; 
		}
		next_size = *nelems_import + check;
	}
	
	
	if(me[0] >= (npes/2+start) && me[0] < npes+start){
		j=check;
		for(i = *nelems_import; i< (*nelems_import + size - check); i++){
			temp_arr[i] = A[j];
			j++; 
		}	
		next_size = *nelems_import + size - check;
	}
	
	return next_size;	
}
	
int quicksort(int *A, int p, int q){
        int r;
//      printf("hello from quicksort\n");
        if (p < q){
                r = partition(A, p, q);
                quicksort(A,p,r-1);
                quicksort(A,r+1,q);
                }
        return r;

        }

int partition(int *A, int p, int q){
        int x,i,j,temp;
//      printf("hello from partition\n");
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
	
	

