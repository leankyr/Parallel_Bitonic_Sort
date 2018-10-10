/*
 bitonic.c 
 This file contains two different implementations of the bitonic sort
        recursive  version :  rec
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
    modified by Antotsiou Dafni and Sourgkounis Theodosis
*/


#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <pthread.h>
#include <math.h>
#include <string.h>


struct timeval startwtime, endwtime;
double seq_time;


int N;//P;          // data array size
int *a;         // data array to be sorted

int threadlayers;

const int ASCENDING  = 1;
const int DESCENDING = 0;


void init(void);
void print(void);
void test(void);
inline void exchange(int i, int j);
void compare(int i, int j, int dir);

void recBitonicSort( int lo, int cnt, int dir );
void bitonicMerge( int lo, int cnt, int dir );

void Psort( void );
void * PrecBitonicSort( void * arg );
void * PbitonicMerge( void * arg );

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

/** the main program **/ 
int main( int argc, char **argv ) {

    if (argc != 2) {
        printf( "Usage: %s q t\n  where n=2^q is problem size (power of two)\n", argv[ 0 ] );
        exit( 1 );
    }


    char *str1;
    str1 = (char *) malloc(25);
    strcpy(str1, "pthreads_data_rec_");
    strcat (str1, argv[1]);
    strcat (str1, ".txt");
    FILE *f1 = fopen(str1,"w");

    N = 1 << atoi( argv[ 1 ] );
    a = (int *) malloc( N * sizeof( int ) );

    for (int i=1;i<=50;i++)
    { 	
      for (int j=1;j<=8;j++){

     threadlayers =j;
      if( threadlayers != 0 && threadlayers != 1 ) {
	--threadlayers;
      }


      init();
      gettimeofday( &startwtime, NULL );
      Psort();
      gettimeofday( &endwtime, NULL );
      seq_time = (double)( ( endwtime.tv_usec - startwtime.tv_usec ) / 1.0e6 + endwtime.tv_sec - startwtime.tv_sec );
      //printf( "Bitonic parallel recursive with qsort and %d threads wall clock time = %f\n", 1 << j, seq_time );
      //test();
      fprintf(f1,"%f ", seq_time);
  
    }
    fprintf(f1,"\n");  
  }
  fclose(f1);

}

/** -------------- SUB-PROCEDURES  ----------------- **/ 




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
    printf("%d\n", a[i]);
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
   the parameter cbt is the number of elements to be sorted. 
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

typedef struct{
    int lo, cnt, dir, layer;
} sarg;

/** Procedure bitonicMerge
 *  Same as serial, but uses pthreads.
 **/
void * PbitonicMerge( void * arg ){
    int lo = ( ( sarg * ) arg ) -> lo;
    int cnt = ( ( sarg * ) arg ) -> cnt;
    int dir = ( ( sarg * ) arg ) -> dir;
    int layer = ( ( sarg * ) arg ) -> layer;
    if( cnt > 1 ){
        int k = cnt / 2;
        int i;
        for( i = lo; i < lo + k; ++i ){
            compare( i, i + k, dir );
        }
        if( layer <= 0 ){
            bitonicMerge( lo, k, dir );
            bitonicMerge( lo + k, k, dir );
            return 0;
        }
        sarg arg1, arg2;
        pthread_t thread1, thread2;
        arg1.lo = lo;
        arg1.cnt = k;
        arg1.dir = dir;
        arg1.layer = layer - 1;
        arg2.lo = lo + k;
        arg2.cnt = k;
        arg2.dir = dir;
        arg2.layer = layer - 1;
        pthread_create( &thread1, NULL, PbitonicMerge, &arg1 );
        pthread_create( &thread2, NULL, PbitonicMerge, &arg2 );
        
        pthread_join( thread1, NULL );
        pthread_join( thread2, NULL );
    }
    return 0;
}

/** function PrecBitonicSort() 
    first produces a bitonic sequence by recursively sorting 
    its two halves in opposite sorting orders, and then
    calls bitonicMerge to make them in the same order 
    Uses pthreads
 **/

void * PrecBitonicSort( void * arg ){
    int lo = ( ( sarg * ) arg ) -> lo;
    int cnt = ( ( sarg * ) arg ) -> cnt;
    int dir = ( ( sarg * ) arg ) -> dir;
    int layer = ( ( sarg * ) arg ) -> layer;
    if ( cnt > 1 ) {
        int k = cnt / 2;
        if( layer >= threadlayers ) {
            qsort( a + lo, k, sizeof( int ), asc );
            qsort( a + ( lo + k ) , k, sizeof( int ), desc );
        }
        else{
            sarg arg1;
            pthread_t thread1;
            arg1.lo = lo;
            arg1.cnt = k;
            arg1.dir = ASCENDING;
            arg1.layer = layer + 1;
            pthread_create( &thread1, NULL, PrecBitonicSort, &arg1 );
            
            sarg arg2;
            pthread_t thread2;
            arg2.lo = lo + k;
            arg2.cnt = k;
            arg2.dir = DESCENDING;
            arg2.layer = layer + 1;
            pthread_create( &thread2, NULL, PrecBitonicSort, &arg2 );
            
            
            pthread_join( thread1, NULL );
            pthread_join( thread2, NULL );
        }
        sarg arg3;
        arg3.lo = lo;
        arg3.cnt = cnt;
        arg3.dir = dir;
        arg3.layer = threadlayers - layer;
        PbitonicMerge( &arg3 );
    }
    return 0;
}

/** function Psort() 
   Caller of recBitonicSort for sorting the entire array of length N 
   in ASCENDING order
 **/
void Psort() {
    sarg arg;
    arg.lo = 0;
    arg.cnt = N;
    arg.dir = ASCENDING;
    arg.layer = 0;
    
    PrecBitonicSort( &arg );
}




