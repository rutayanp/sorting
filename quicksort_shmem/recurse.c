#include <shmem.h>
#include <stdio.h>
int me,npes,i=0;
int recurse(int, int);
long pSync[_SHMEM_BCAST_SYNC_SIZE];
int me_import=0;
int main ()
{
	int i;
	for (i=0; i < SHMEM_BCAST_SYNC_SIZE; i++) {
 		pSync[i] = _SHMEM_SYNC_VALUE;
		}
	
	start_pes (0);
//	int * me = (int*) shmalloc(sizeof(int));
//	int * me_import = (int*) shmalloc(sizeof(int));
	me = shmem_my_pe ();
	int npes = shmem_n_pes ();
	/*if(me[0]>3)  //	this if condition is the work around...on removing this the broadcast doesnt work properly.
		shmem_broadcast32(me_import,me,sizeof(int),0,4,0,4,pSync);
        shmem_barrier_all();
        printf("(%d) me_import %d (i): %d\n", me[0],me_import[0],i);*/
	recurse(0,npes); //let this function be commented for now. I am using something similar for my hyperquicksort.
	shmem_finalize ();
}

int recurse(int b, int npes){
	int npe;
//	int me_import = 0;
	if(npes > 1){
		shmem_broadcast64(&me_import,&me,1,0,b,0,npes,pSync);
		//shmem_barrier_all();
		if(me == b) me_import = me;
		printf("(%d) me_import %d \n", me,me_import);
		npe = npes/2;
		if(me >= b && me < b+npe) recurse(b,npe);
		else if(me >= npe+b && me < b+npes) recurse(b+npe,npe);
	}
	else
	return 1;

}
