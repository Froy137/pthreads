//Author: Froylan Morales

#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>
#include <pthread.h>

static void* pthreadCalc(void* rank);

static void* pthreadCalc2(void* rank);

static int f(int val)
{
  return (val - 17) * 19;
}

static void prefixSumA(int data[], const int size)
{
  for (int i = 1; i < size; i++) {
    data[i] += data[i - 1];
  }
}

/*
static void prefixSumB(int data[], const int size)
{
  for (int dist = 1; dist < size; dist *= 2) {
    for (int i = size - 1; i >= dist; i--) {
      data[i] += data[i - dist];
    }
  }
}
*/

//global vars
static long long thread_count;
static int*arrayA;
static int*arrayB;
static int*arrayC;
static int*lastPrefix;
static int size;


int main(int argc, char* argv[])
{
  printf("Prefix Sum v1.0 [pthreads]\n");

  // check command line
  if (argc != 3) {fprintf(stderr, "usage: %s size\n", argv[0]); exit(-1);}
  size = atoi(argv[1]);
  if (size < 1) {fprintf(stderr, "size is too small: %d\n", size); exit(-1);}
  
    thread_count = strtol(argv[2],NULL,10);

   printf("stage1\n"); 
   printf("configuration: %d elements, %llu threads.\n", size,thread_count);	
   printf("stage1"); 
  


// allocate arrays
   arrayA = (int*)malloc(sizeof(int) * size);  if (arrayA == NULL) {fprintf(stderr, "cannot allocate arrayA\n");  exit(-1);}
   
   printf("stage1"); 
arrayB = (int*)malloc(sizeof(int) * size);  if (arrayB == NULL) {fprintf(stderr, "cannot allocate arrayB\n");  exit(-1);}
   
   printf("stage1"); 
arrayC = (int*)malloc(sizeof(int) * size);  if (arrayC == NULL) {fprintf(stderr, "cannot allocate arrayC\n");  exit(-1);}
   lastPrefix= (int*)malloc(sizeof(int) * thread_count);  if (lastPrefix == NULL) {fprintf(stderr, "cannot allocate partialPrefix\n");  exit(-1);}
    
   printf("stage1"); 
  

// initialize
  for (int i = 0; i < size; i++) 
  {
    arrayA[i] = arrayB[i] = f(i);
  }

   printf("stage1"); 
  // time the prefix sum computation
  struct timeval start, end;
  gettimeofday(&start, NULL);
  //prefixSumB(arrayB, size);
  
    long thread;
    pthread_t* thread_handles;
  
   thread_handles = malloc ((thread_count-1.0)*sizeof(pthread_t));
   printf("stage0");

 
    //generating -1 thread so that master thread can work too.
  for (thread=0;thread<thread_count-1.0;thread++){
	  pthread_create(&thread_handles[thread],NULL,pthreadCalc,(void*) thread);
  }
   
   printf("stage2"); 
    //master doing work too.      
    pthreadCalc((void*)thread_count-1);
  
    //join threads
    for (thread=0;thread<thread_count-1.0;thread++){
	  pthread_join(thread_handles[thread],NULL);
  }
  
   printf("stage3"); 
    //master copy carry values to new array .
  	int chunk_size = (size / thread_count)-1.0;
   

 //loading the carries to new array
    //for(int chk=chunk_size;chk<size;chk+chunk_size){
     for(int u=0;u<thread_count;u++){
	int r=(u+1)*size/thread_count;
	lastPrefix[u]=arrayC[r];   
    }
  
    //doing prefix sum on the carries
    prefixSumA(lastPrefix,(int)thread_count);
    
    
    //create threads again
    for (thread=0;thread<thread_count-1.0;thread++){
	  pthread_create(&thread_handles[thread],NULL,pthreadCalc2,(void*) thread);
  }
   
      //master doing work too.      
    pthreadCalc2((void*)thread_count-1);
    
    
        //join threads
    for (thread=0;thread<thread_count-1.0;thread++){
	  pthread_join(thread_handles[thread],NULL);
  }
    
  gettimeofday(&end, NULL);

  // print performance info
  double runtime = end.tv_sec + end.tv_usec / 1000000.0 - start.tv_sec - start.tv_usec / 1000000.0;
  printf("compute time: %.4f s\n", runtime);
  printf("mega_elements/sec: %.3f\n", size * 0.000001 / runtime);

  // compare results
  prefixSumA(arrayA, size);
  for (int i = 0; i < size; i++) {
    if (arrayA[i] != arrayC[i]) {fprintf(stderr, "result mismatch at position %d\n", i);  exit(-1);}
  }

  free(arrayA);  free(arrayB);
 

 return 0;
}

static void* pthreadCalc(void* rank){
	long long my_rank = (long long)rank;
	int my_start = my_rank * size / thread_count;
	int my_end = (my_rank + 1) * size / thread_count;
	
	int myS=my_end-my_start;
    
    //creating a temp array for local prefix solution
	int tempArr[myS];
	int c=0;
    //getting partial array.
    for(int p=my_start;p<my_end;p++,c++){
        //loading the corresponding part of the array into temp
        tempArr[c]=arrayA[p];
    }
    
	prefixSumA(tempArr,myS);
	
    c=0;
    for(int y=my_start;y<my_end;y++,c++){
        arrayC[y]=tempArr[c];
    }
    
	return NULL;
}

static void* pthreadCalc2(void* rank){
	long long my_rank = (long long)rank;
	int my_start = my_rank * size / thread_count;
	int my_end = (my_rank + 1) * size / thread_count;
	
    
    //get your index from lastPrefix and subtract last value from arrayC
    int additionValue=lastPrefix[my_rank]-arrayC[my_end];
    
   for(int y=my_start;y<my_end;y++){
        arrayC[y]+=additionValue;
    }
    
    
	return NULL;
}
