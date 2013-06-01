#include <stdio.h>
#include <stdlib.h>
#include <shmem.h>
#include <time.h>

int me, npes;
int *A, *Aux;
long pSync[_SHMEM_BCAST_SYNC_SIZE];

int recurse (int, int, int, int, int);
int mergesort(int, int);
int merge(int, int, int);

int main(int argc, char * argv[]){
	time_t start_t, end_t;
	int i;
	double seconds;
	for (i = 0; i < SHMEM_BCAST_SYNC_SIZE; i += 1) { 
		pSync[i] = _SHMEM_SYNC_VALUE;
	}

	start_pes(0);
	me = shmem_my_pe();
	npes = shmem_n_pes();
	srand(me*time(NULL));

	int N;
	N = atoi (argv[1]);
	//printf(" N = %d\n",N);
	//based on this N generate N/npes length sequence in each processor. Merge sort each sequence in the different processes. Then recurse is called upon to perform the interprocessor merge operation. 
	A = (int *)shmalloc(N*sizeof(int));
	Aux = (int *)shmalloc(N*sizeof(int));
	if(A == NULL){
		printf("Out of Memory\n");
		return 1;
	}
	
	if(Aux == NULL){
		printf("Out of Memory\n");
		return 1;
	}

	for(i =0 ; i< N/npes; i++){
		A[i] = rand()%(100000-0);		
	} 	
	
	shmem_barrier_all();
/*	printf("DEBUG(%d)the unsorted elements: ",me);
	for(i=0;i< N/npes; i++){
		printf("%d ,",A[i] );
	}*/
	//recurse (1,2, npes,1);
	printf("\n");
	start_t = time(NULL);
	mergesort(0,N/npes-1);
	
	shmem_barrier_all();
/*	printf("DEBUG(%d)the sorted elements: ",me);
	for(i=0;i< N/npes; i++){
		printf("%d ,",A[i] );
	}*/
	
	recurse (1,2, npes,1, N/npes);
	end_t = time(NULL);
	seconds = difftime(end_t, start_t);
	if(me == 0){
		printf("\nTime taken : %f\n",seconds);
		printf("\n(%d)The sorted list:\n",me);
		for(i=0 ; i< N; i++){
			printf("A[%d] = %d \n", i, A[i]);
		}	
	}
	
	shfree(Aux);
	shfree(A);
	shmem_finalize();

}
			
int mergesort(int low, int high){

        int middle;
//      static int c=0;//DEBUG

        if(low < high){
                middle = low + (high-low)/2;
//              printf("call(%d) : %d\n", c,middle); //DEBUG
                mergesort(low,middle);
                mergesort(middle+1,high);
//              c++; //DEBUG  

                merge(low,middle,high);

        }
}

int merge(int low, int middle, int high){

        int i,j,k;

        //This is to create the auxillary arrays to merge 
        for(i=low ; i<= high; i++){
                Aux[i] = A[i];
        }

        i= low;
        j= middle+1;
        k= low;

        while(i <= middle && j <= high){
                if (Aux[i] <= Aux[j]) A[k++] = Aux[i++];
                else if(Aux[i] > Aux[j]) A[k++] = Aux[j++];
        }

        while(j <= high){
                A[k++] = Aux[j++];
        }

        while(i <= middle){
                A[k++] = Aux[i++];
        }
}

int recurse(int i, int ilog, int pes, int j, int nelems){
	int next_i,next_ilog,next_pes,k;
	if(ilog <= npes){
		if(me % ilog ==0){

			if(me == 4) printf("\n");	
	//		printf("\nDEBUG%d --> %d", me+i, me);
			shmem_int_get(&A[nelems], A, nelems, me+i);			
			
		/*	printf("\nDEBUG(%d)imported list: ",me);
			for( k = 0; k < nelems; k++){
				printf("%d ,",Aux[k]); 
			}
			printf("\n");			
			
			for(k=0; k < nelems; k++){
				A[nelems+k] = Aux[k];
			}	*/		

			merge (0,nelems-1, nelems*2-1);
	
			nelems = nelems *2;
				
		/*	printf("\nDEBUG(%d)new A: ",me);
                        for( k = 0; k < nelems; k++){
                                printf("A[%d]= %d \n",k,A[k]);
                        }
                        printf("\n");*/
			
			
			
			next_pes = pes/2;
			next_i = 2*i;
			next_ilog = 2*ilog;
			shmem_barrier(0,j , next_pes, pSync);
			j = j+1;
			recurse(next_i, next_ilog, next_pes, j, nelems);
		}
	}
}
