//
//  qsort.c
//  
//
//  Created by Rutayan Patro on 4/27/13.
//
//
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <omp.h>

double timerval()
{
	struct timeval st;
	gettimeofday(&st,NULL);
	return st.tv_sec + st.tv_usec * 1e-6;
}
void srand48();
double drand48();

static int CmpInt(const void *a, const void *b)
{
	return ( *(int*)a - *(int*)b );
}

/* Merge sorted lists A and B into list A.  A must have dim >= m+n */
void merge(int A[], int B[], int m, int n)
{
	int i=0, j=0, k=0, p=0;
	int size = m+n;
	int *C = (int *)malloc(size*sizeof(int));
	while (i < m && j < n) {
        if (A[i] <= B[j]) C[k] = A[i++];
        else C[k] = B[j++];
        k++;
	}
	if (i < m) for ( p = i; p < m; p++,k++) C[k] = A[p];
	else for ( p = j; p < n; p++,k++) C[k] = B[p];
	for( i=0; i<size; i++ ) A[i] = C[i];
	free(C);
}

/* Merges N sorted sub-sections of array a into final, fully sorted array a */
void arraymerge( int *a, int size, int *index, int N)
{
	int i;
	while ( N>1 ) {
	    for( i=0; i<N; i++ ) index[i]=i*size/N; index[N]=size;
#pragma omp parallel for private(i)
	    for( i=0; i<N; i+=2 ) {
            merge(a+index[i],a+index[i+1],index[i+1]-index[i],index[i+2]-index[i+1]);
	    }
	    N /= 2;
	}
}

int quicksort( int *A, int p, int q){
	int r;
	if (p < q){
		r = partition(A, p, q);
		quicksort(A,p,r-1);
		quicksort(A,r+1,q);
    }
	return r;
	
}

int partition( int *A, int p, int q){
	int x,i,j,temp;
	
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


int main(int argc,char **argv)
{
	int i;
	/*if( argc != 2 && argc != 3 ) {
		fprintf(stderr,"usage: quicksort_omp Nelements [Nthreads]\n");
		return -1;
	}*/
    // set up array to be sorted
	int size = atoi(argv[1]);
	int *a = (int *)malloc(size*sizeof(int));
	srand48(8675309);
	for(i=0; i<size; i++) { a[i] = rand()%100; printf("a[%d] %d\n",i,a[i]);}
    // set up threads
	int threads = omp_get_max_threads();
	int *index = (int *)malloc((threads+1)*sizeof(int));
	for(i=0; i<threads; i++) index[i]=i*size/threads; index[threads]=size;
    
    /* Main parallel sort loop */
	double start = timerval();
#pragma omp parallel for private(i)
	for(i=0; i<threads; i++) quicksort(&(a[index[i]]),0,(index[i+1]-index[i]-1));
	double middle = timerval();
    /* Merge sorted array pieces */
	if( threads>1 ) arraymerge(a,size,index,threads);
	double end = timerval();
    	printf("\nTime taken for %d inputs: %f",size,end-start);
	int lm=0;
    /* Check the sort -- output should never show "BAD: ..." */
//	for(lm=0; lm<size; lm++) printf("a[%d] : %d\n",lm,a[lm]);
	free(a);
	return 0;
}
