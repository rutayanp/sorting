#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int quicksort(int *A, int, int);
int partition(int *A, int, int);

int main(int argc, char **argv){
	int *A,i,j,n;
	srand (time(NULL));
	
	n= atoi(argv[1]);
	
	printf("The value of n : %d\n",n);
	A = (int *)malloc(n*sizeof(int));
	
	if (A == 0){
		printf("ERROR: Out of memory\n");
		return 1;
		}
	i=0;
	while(i<n){
		*(A+i) = rand()%(60000-0);
		i++;
		}
	
/*	i=0;
	printf("The elements are: \n");
	while(i<n){
		printf("%d, ",A[i]);
		i++;
		}*/
	i=0;
	int pivot=quicksort(A, 0, n-1);
	printf("The pivot:%d",pivot);
	printf("\nThe sorted list is: ");
	while(i<n){
		printf("%d \n ",A[i]);
		i++;
		}
	printf("\n"); 	
	return 0;
	} 
			
int quicksort(int *A, int p, int q){
	int r;
	if (p < q){
		r = partition(A, p, q);
		quicksort(A,p,r-1);
		quicksort(A,r+1,q);
		}
	return r;
	
	}
	
int partition(int *A, int p, int q){
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
