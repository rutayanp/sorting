#include <shmem.h>
#include <stdio.h>
int me,npes;
int hello();
int main ()
{
	start_pes (0);
	me = shmem_my_pe ();
	npes = shmem_n_pes ();

	printf ("I am %d of %d.\n", me, npes);
	hello();
	shmem_finalize ();
}

int hello(){
	shmem_barrier_all();
	printf("(%d) Hello \n",me);
}
