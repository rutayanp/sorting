#include <stdio.h>
#include <shmem.h>

int main(){
	start_pes(0);
	int me = shmem_my_pe();		
	int npes = shmem_n_pes();	
	
	printf("I am %d of %d.\n",me,npes);
	shmem_finalize();
}
