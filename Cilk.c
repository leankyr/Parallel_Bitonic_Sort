/*
 bitonic.c 

 This file contains two different implementations of the bitonic sort
        recursive  version :  recBitonicSort()
        imperative version :  impBitonicSort() 
 

 The bitonic sort is also known as Batcher Sort. 
 For a reference of the algorithm, see the article titled 
 Sorting networks and their applications by K. E. Batcher in 1968 


 The following codes take references to the codes avaiable at 

 http://www.cag.lcs.mit.edu/streamit/results/bitonic/code/c/bitonic.c

 http://www.tools-of-computing.com/tc/CS/Sorts/bitonic_sort.htm

 http://www.iti.fh-flensburg.de/lang/algorithmen/sortieren/bitonic/bitonicen.htm 
 */

/* 
------- ---------------------- 
   Nikos Pitsianis, Duke CS 
-----------------------------
*/


#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <cilk/cilk.h>
#include <cilk/cilk_api.h>
//#include <math.h>
#include <string.h>

struct timeval startwtime, endwtime;
double seq_time;


int N;          // data array size
int *a;         // data array to be sorted
int P,p;
int count_threads=1;

const int ASCENDING  = 1;
const int DESCENDING = 0;

void init(void);
void print(void);
void sort(void);
void test(void);
void exchange(int i, int j);
void compare(int i, int j, int dir);
void bitonicMerge(int lo, int cnt, int dir);
void recBitonicSort(int lo, int cnt, int dir);
void impBitonicSort(void);


/** the main program **/ 
int main(int argc, char **argv) {



   if (argc != 2 && argc!= 3 || (argc == 3 && atoi(argv[2]) > 8)) {
   printf("Usage: %s q\n  where N=2^q is problem size (power of two) and P=2^p is the number of active workers\n",argv[0]);
    exit(1);
  }

  N = 1<<atoi(argv[1]);
  a = (int *) malloc(N * sizeof(int)); 

	  if (argc==2) P=__cilkrts_get_nworkers();
	  else {p=atoi(argv[2]); P = 1<<p;}
	  char str_threads[30];
	  sprintf(str_threads,"%d",P);
	  __cilkrts_set_param("nworkers",str_threads);
  	  init();

	  gettimeofday (&startwtime, NULL);
	  //print();
	  impBitonicSort();
	  //print();
	  gettimeofday (&endwtime, NULL);
	  seq_time = (double)((endwtime.tv_usec - startwtime.tv_usec)/1.0e6
			      + endwtime.tv_sec - startwtime.tv_sec);
	  printf("Imperative wall clock time = %f\n", seq_time);
	  test();

	
	  init();

	  gettimeofday (&startwtime, NULL);
	  //print();
	  sort();
	  //print();
	  gettimeofday (&endwtime, NULL);
	  seq_time = (double)((endwtime.tv_usec - startwtime.tv_usec)/1.0e6
			      + endwtime.tv_sec - startwtime.tv_sec);
	  printf("Recursive wall clock time = %f\n", seq_time);
	  test();
}


/** -------------- SUB-PROCEDURES  ----------------- **/ 

/** compare for qsort **/
int desc( const void *a, const void *b ){
    int* arg1 = (int *)a;
    int* arg2 = (int *)b;
    if( *arg1 > *arg2 ) return -1;
    else if( *arg1 == *arg2 ) return 0;
    return 1;
}
int asc( const void *a, const void *b ){
    int* arg1 = (int *)a;
    int* arg2 = (int *)b;
    if( *arg1 < *arg2 ) return -1;
    else if( *arg1 == *arg2 ) return 0;
    return 1;
}


/** procedure test() : verify sort results **/
void test() {
  int pass = 1;
  int i;
  for (i = 1; i < N; i++) {
    pass &= (a[i-1] <= a[i]);
  }

  printf(" TEST %s\n",(pass) ? "PASSed" : "FAILed");
}


/** procedure init() : initialize array "a" with data **/
void init() {
  int i;
  for (i = 0; i < N; i++) {
    a[i] = rand() % N; // (N - i);
  }
}

/** procedure  print() : print array elements **/
void print() {
  int i;
  for (i = 0; i < N; i++) {
    printf("%d ", a[i]);
  }
  printf("\n");
}


/** INLINE procedure exchange() : pair swap **/
inline void exchange(int i, int j) {
  int t;
  t = a[i];
  a[i] = a[j];
  a[j] = t;
}



/** procedure compare() 
   The parameter dir indicates the sorting direction, ASCENDING 
   or DESCENDING; if (a[i] > a[j]) agrees with the direction, 
   then a[i] and a[j] are interchanged.
**/
inline void compare(int i, int j, int dir) {
  if (dir==(a[i]>a[j])) 
    exchange(i,j);
}


/** Procedure bitonicMerge() 
   It recursively sorts a bitonic sequence in ascending order, 
   if dir = ASCENDING, and in descending order otherwise. 
   The sequence to be sorted starts at index position lo,
   the parameter cnt is the number of elements to be sorted. 
 **/
void bitonicMerge(int lo, int cnt, int dir) {
  if (cnt>1) {
    int k=cnt/2;
    int i;
    for (i=lo; i<lo+k; i++)
      compare(i, i+k, dir);

    bitonicMerge(lo, k, dir);
    bitonicMerge(lo+k, k, dir);
  }
}

/** function recBitonicSort() 
    first produces a bitonic sequence by recursively sorting 
    its two halves in opposite sorting orders, and then
    calls bitonicMerge to make them in the same order 
 **/
void recBitonicSort(int lo, int cnt, int dir){//, int* count_threads) {
   //printf("************************************************s\n");
  
    if (cnt>1) {
    int k=cnt/2;
    if (count_threads<P){
		
	 //printf("Parallel->count_threads=%d\n",count_threads);		
	 count_threads+=1;
	 cilk_spawn recBitonicSort(lo, k, ASCENDING);
    	 recBitonicSort(lo+k, k, DESCENDING);
    	 cilk_sync;
	 count_threads-=1;
    }
    else {	
	  //printf("QSort->count_threads=%d\n",count_threads);
	  qsort( a + lo, k, sizeof( int ), asc);
	  qsort(a + ( lo + k ) , k, sizeof( int ), desc);
    }    
    bitonicMerge(lo, cnt, dir);
  }
    //printf("************************************************e\n");
}


/** function sort() 
   Caller of recBitonicSort for sorting the entire array of length N 
   in ASCENDING order
 **/
void sort() {
  recBitonicSort(0, N, ASCENDING);
}



/*
  imperative version of bitonic sort
*/
void impBitonicSort() {

  int i,j,k;
  
  for (k=2; k<=N; k=2*k) {
    for (j=k>>1; j>0; j=j>>1) {

   	//if (count_threads<__e) {	
     		cilk_for (i=0; i<N; i++) {
		int ij=i^j;
		if ((ij)>i) {
	 	if ((i&k)==0 && a[i] > a[ij]) 
	      	exchange(i,ij);
	  	if ((i&k)!=0 && a[i] < a[ij])
	      	exchange(i,ij);
	//}
	//else {  
	//	qsort( a, N, sizeof( int ), asc);
	//}

	}
      }
    }
  }
}
