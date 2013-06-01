#include<stdio.h>
#include<omp.h>
#include<stdlib.h>
#include<time.h>

double timerval()
{
	struct timeval st;
	gettimeofday(&st,NULL);
	return st.tv_sec + st.tv_usec * 1e-6;
}
int *a;
void qsort_serial(int l,int r){
	if(r>l){
		int pivot=a[r],tmp;
		int less=l-1,more;
		for(more=l;more<=r;more++){
			if(a[more]<=pivot){
				less++;
				 tmp=a[less];
				a[less]=a[more];
				a[more]=tmp;
			}
		}
		qsort_serial(l,less-1);
		qsort_serial(less+1,r);
	}
}
void qsort_parallel(int l,int r){
	if(r>l){

		int pivot=a[r],tmp;
		int less=l-1,more;
		for(more=l;more<=r;more++){
			if(a[more]<=pivot){
				less++;
				tmp=a[less];
				a[less]=a[more];
				a[more]=tmp;
			}
		}
		if((r-l)<1000){
			qsort_serial(l,less-1);
			qsort_serial(less+1,r);
		}
		else{
			#pragma omp task
			qsort_parallel(l,less-1);
			#pragma omp task
			qsort_parallel(less+1,r);
			#pragma omp taskwait
		}
	}
}
int main(int argc, char *argv[]){
	long int n,i;
	double seconds;
	double start_t, end_t;
	n=atoi(argv[1]); //increased the value of n
	a =(int*)malloc(n*sizeof(int)); 
	int range=100000;
	srand(time(NULL));
	/*printf("Unsorted list: \n");
	for( i=0;i<n;i++){
		a[i]=rand()%range;
		printf("%d \n",a[i]);
	}*/
	#pragma omp parallel	
	#pragma omp single
	start_t = timerval();
	qsort_parallel(0,n-1);
	end_t = timerval();
	seconds = end_t-start_t;
	printf("Time Taken : %f\n",seconds);
	/*printf("\n Sorted List : \n");
	for(i=0;i<n;i++ ){
		printf("%d \n",a[i]);
	}*/
	free(a);	
	return 0;
}
