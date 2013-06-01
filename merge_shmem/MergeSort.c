#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int mergesort(int, int);
int merge(int, int, int);

int *A,*Aux;

int main(int argc, char *argv[]){

	int i,j,low,high;	
	time_t start_t, end_t;
	double seconds;	
	srand(time(NULL));
	
	high = atoi(argv[1]) -1;
	
//	printf("high : %d\n",high); //DEBUG
	A = (int *)malloc((high+1)*sizeof(int));
	Aux = (int *)malloc((high+1)*sizeof(int));
	
	if(A == NULL) {
		printf("Out of Memory");
		return 1;
	}

	printf("\nUnsort List : \n");	
	for(i =0 ;i <= high; i++){
		A[i] = rand()%(10000-0);
		printf("A(%d) : %d \n",i,A[i]);
	}
	
	low =0;
	

	start_t = time (NULL);
	mergesort(low, high);
	end_t = time(NULL);
	seconds = difftime(start_t, end_t);
	printf("Time taken : %f\n",seconds);	
	printf("\n\nSorted List: \n");
	for(i=0; i<= high; i++){
		printf("A[%d]: %d\n",i,A[i]);
	}
}


int mergesort(int low, int high){
	
	int middle;
//	static int c=0;//DEBUG
	
	if(low < high){
		middle = low + (high-low)/2;
//		printf("call(%d) : %d\n", c,middle); //DEBUG
		mergesort(low,middle);
		mergesort(middle+1,high);
//		c++; //DEBUG  
		
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
