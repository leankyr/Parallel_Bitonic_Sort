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
#include <omp.h>
//#include <math.h>
#include <string.h>

struct timeval startwtime, endwtime;
double seq_time;




int N,n,t,BASE ;          // data array size
int *a;          // data array to be sorted
     int parallel;
const int ASCENDING  = 1;
const int DESCENDING = 0;
int P;
int count_threads = 1;


void init(void);
void print(void);
void sort(void);
void test(void);
inline void exchange(int i, int j);
void compare(int i, int j, int dir);
void bitonicMerge(int lo, int cnt, int dir);
void recBitonicSort(int lo, int cnt, int dir);
void impBitonicSort(void);

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
int main(int argc, char **argv) {

  omp_set_dynamic(0);

  N = 1<<atoi(argv[1]);
  a = (int *) malloc(N * sizeof(int));
  
 char *str1;
  str1 = (char *) malloc(25);
  strcpy(str1, "openmp_data_imp_");
  strcat (str1, argv[1]);
  strcat (str1, ".txt");

  char *str2;
  str2 = (char *) malloc(25);
  strcpy(str2, "openmp_data_rec_");
  strcat (str2, argv[1]);
  strcat (str2, ".txt");

  FILE *f1 = fopen(str1,"w");
  FILE *f2 = fopen(str2, "w");
   int i,j;  

   for ( i=1;i<=50;i++)
   { 	
	  for ( j=1;j<=8;j++){
	  	
	  	P = 1<<j;
		//First Implementation
                              
  		init(); //Initialises array with random data

 		gettimeofday (&startwtime, NULL);
  		impBitonicSort();
  		gettimeofday (&endwtime, NULL);
  		seq_time = (double)((endwtime.tv_usec - startwtime.tv_usec)/1.0e6
		      + endwtime.tv_sec - startwtime.tv_sec);
  		fprintf(f1, "%f ", seq_time);
  		
		//Second Implementation

 		init();

  		gettimeofday (&startwtime, NULL);
  		sort();
  		gettimeofday (&endwtime, NULL);
  		seq_time = (double)((endwtime.tv_usec - startwtime.tv_usec)/1.0e6
		      + endwtime.tv_sec - startwtime.tv_sec);
  		fprintf(f2,"%f ", seq_time);
  
  	}
  	fprintf(f1,"\n");
  	fprintf(f2,"\n");
  
  }
  fclose(f1);
  fclose(f2);


}
/** -------------- SUB-PROCEDURES  ----------------- **/ 

/** procedure test() : verify sort results **/
void test() {
  int pass = 1;
  int i;
  for (i = 1; i < N; i++) {
    pass &= (a[i-1] <= a[i]); //bitwise and assignement between pass (=1 initially)  and a[i-1] a[i]  
  }

  printf(" TEST %s\n",(pass) ? "PASSed" : "FAILed");
}


/** procedure init() : initialize array "a" with data **/
void init() {
  int i;
  for (i = 0; i < N; i++) {
    a[i] = rand() % N; // (N - i);
 //   printf("%d\n" ,a[i]  );
  }
}

/** procedure  print() : print array elements **/
void print() {
  int i;
  for (i = 0; i < N; i++) {
  //  printf("%d\n", a[i]);
  }
  printf("\n");
}


/** INLINE procedure excihange() : pair swap **/
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
    for (i=lo; i<lo + k; i++ ){
    compare(i, i+k, dir);
 }
  #pragma omp task
    bitonicMerge(lo, k, dir);
 // #pragma omp task
    bitonicMerge(lo+k, k, dir);
  #pragma omp taskwait
   }

 }
  
/** function recBitonicSort() 
    first produces a bitonic sequence by recursively sorting 
    its two halves in opposite sorting orders, and then
    calls bitonicMerge to make them in the same order 
 **/

void recBitonicSort(int lo, int cnt, int dir) {  //dir = direction (ascending or decending)
 if (cnt>1) {
     if (cnt > BASE) {                                 //Initially  lo = 0, cnt == N
    int k=cnt/2;


      #pragma omp task    
        recBitonicSort(lo, k, ASCENDING);
        recBitonicSort(lo+k, k, DESCENDING);
      #pragma omp taskwait    
bitonicMerge(lo, cnt, dir);
}
else {
   if (dir == ASCENDING) {
qsort( a + lo, cnt, sizeof( int ), asc );
    }
   else{
qsort( a + lo ,cnt, sizeof( int ), desc );
}
  }

  }
}
/** function sort() 
   Caller of recBitonicSort for sorting the entire array of length N 
   in ASCENDING order
 **/

void sort() {
BASE = N/P;
#pragma omp parallel
#pragma omp single nowait
  recBitonicSort(0, N, ASCENDING);   // N = Size of Sequence
}          
//  imperative version of bitonic sort



void impBitonicSort() {
//omp_set_num_threads(2);
  int i,j,k;
  for (k=2; k<=N; k=2*k){
    for (j=k>>1; j>0; j=j>>1) {

#pragma omp parallel for schedule(auto)  
     for (i=0; i<N; i++) {
	int ij=i^j; //xor ij 
	if ((ij)>i) {
//           if( count_threads > omp_get_num_threads()) {
  //            count_threads += 1;
	  if ((i&k)==0 && a[i] > a[ij]) //bitwise and i , k
	      exchange(i,ij);
	  if ((i&k)!=0 && a[i] < a[ij])
	      exchange(i,ij);
	}
        // count_threads -= 1;    
//} 
//else {

//qsort( a + j, k, sizeof( int ), asc );
//qsort( a + ( j + k ) , k, sizeof( int ), desc );

// }
      }
    }     // end of for with j 
  }      // end of for with k
}      // end of impBitonicSort()

