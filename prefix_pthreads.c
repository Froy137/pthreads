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
static int*arrayC;
static int*lastPrefix;
static int size;

//1000000000
int main(int argc, char* argv[])
{
                              printf("Prefix Sum v1.0 [pthreads]\n");

                              // check command line
                              if (argc != 3) {fprintf(stderr, "usage: %s size\n", argv[0]); exit(-1);}
                              size = atoi(argv[1]);
                              if (size < 1) {fprintf(stderr, "size is too small: %d\n", size); exit(-1);}

                                thread_count = strtol(argv[2],NULL,10);

                              
                               printf("configuration: %d elements, %llu threads.\n", size,thread_count);	

								int fPref=(int)thread_count;
                            // allocate arrays
                               arrayA = (int*)malloc(sizeof(int) * size);  if (arrayA == NULL) {fprintf(stderr, "cannot allocate arrayA\n");  exit(-1);}
                               arrayC = (int*)malloc(sizeof(int) * size);  if (arrayC == NULL) {fprintf(stderr, "cannot allocate arrayC\n");  exit(-1);}
                               lastPrefix= (int*)malloc(sizeof(int) * fPref);  if (lastPrefix == NULL) {fprintf(stderr, "cannot allocate partialPrefix\n");exit(-1);}

                               


                            // initialize
                              for (int i = 0; i < size; i++) 
                              {
                                arrayA[i]= f(i);
                              }

                               
                              // time the prefix sum computation
                              struct timeval start, end;
                              gettimeofday(&start, NULL);
                              //prefixSumB(arrayB, size);

                                long thread;
                                pthread_t* thread_handles;

                               thread_handles = malloc ((thread_count-1.0)*sizeof(pthread_t));
                               


                                for(int v=0;v<size;v++){
								if(v > size-10)//print last chunk
                                    printf("Array A %d\n",arrayA[v]);   
                                }
									long templ=thread_count-1.0;
                                //generating -1 thread so that master thread can work too.
                              for (thread=0;thread<templ;thread++){
                                  pthread_create(&thread_handles[thread],NULL,pthreadCalc,(void*) thread);
                              }
							  //master doing work too.      
                                pthreadCalc((void*)templ);
								
                                //join threads
                                for (thread=0;thread<templ;thread++){
                                  pthread_join(thread_handles[thread],NULL);
                              }
                               
                                


			printf("JOINED THREADS 1\n");
			printf("threadcount:%llu\n",thread_count);
                               for(int v=0;v<size;v++){
								if(v > size-10)//print last chunk
                                  printf("Array C %d\n",arrayC[v]);   
                                }
    
    
    
    
    
                                //printf("in PRE bad area?");
                                //master copy carry values to new array .
                                //int chunk_size = (size / thread_count)-1.0;


                             //loading the carries to new array
                                //for(int chk=chunk_size;chk<size;chk+chunk_size){
                                 
                                for(int u=0;u<thread_count;u++){
								//	printf("in bad area?");
                                    long r=(u+1)*size/thread_count;
									r=r-1.0;;
                                    lastPrefix[u]=arrayC[r];   
                                }

                               for(int v=0;v<thread_count;v++){
										//if(v > size-10)//print last chunk
											printf("lastPrefix %d\n",lastPrefix[v]);   
                                }
    
                                //doing prefix sum on the carries
                                prefixSumA(lastPrefix,(int)thread_count);

                                 for(int v=0;v<thread_count;v++){
									// if(v > size-10)//print last chunk
                                printf("lastPrefix AFTER Prefix + %d\n",lastPrefix[v]);   
                                }
    
    
                                //create threads again
                                for (thread=0;thread<thread_count-1.0;thread++){
                                  pthread_create(&thread_handles[thread],NULL,pthreadCalc2,(void*) thread);
                              }
                                
                               // printf("Array C %d\n"); 
                               //master doing work too.      
                                pthreadCalc2((void*)templ);


                                    //join threads
                                for (thread=0;thread<thread_count-1.0;thread++){
                                  pthread_join(thread_handles[thread],NULL);
                              }

    
                               for(int v=0;v<size;v++){
								   if(v > size-10)//print last chunk
                                   printf("Array C after Prefix+ %d\n",arrayC[v]);   
                                }
                               
                              gettimeofday(&end, NULL);

                              // print performance info
                              double runtime = end.tv_sec + end.tv_usec / 1000000.0 - start.tv_sec - start.tv_usec / 1000000.0;
                              printf("compute time: %.4f s\n", runtime);
                              printf("mega_elements/sec: %.3f\n", size * 0.000001 / runtime);

                              // compare results
                              prefixSumA(arrayA, size);
    
                                      // for(int v=0;v<size;v++){
									//	   if(v > size-10)//print last chunk
									//	printf("Array A after Prefix+ %d\n",arrayA[v]);   
                                //}
    
                              for (int i = 0; i < size; i++) {
                                if (arrayA[i] != arrayC[i]) 
                                {
                                    fprintf(stderr, "result mismatch at position %d\n", i);  

                                    printf("current A:%d current C:%d\n",arrayA[i],arrayC[i]);
                                    printf("Next A:%d Next C:%d\n",arrayA[i+1],arrayC[i+1]);
                                    exit(-1);

                                }
                              }
                                free(thread_handles);
                              free(arrayA);  free(arrayC);


                             return 0;
}

static void* pthreadCalc(void* rank){
	
	
	long long my_rank = (long long)rank;
	long my_start = my_rank * size / thread_count;
	long my_end = (my_rank + 1.0) * size / thread_count;
	printf("\nrank in %llu:\n",my_rank);
	//long myS=size/thread_count;
	long myS=my_end-my_start;
	//if(my_rank == thread_count-1){
	//	myS=my_end-my_start;
	//my_end++;	
	//}
	

	printf("\nstart in %llu:    %llu:\n",my_rank,my_start);
	printf("\nend in %llu:      %llu:\n",my_rank,my_end);
	printf("\nmysVAL in %llu:   %llu:\n",my_rank,myS);
	
    printf("\nrank in mys%llu:\n",my_rank);
    //creating a temp array for local prefix solution
	//int tempArr[myS];
	int *tempArr= (int*)malloc(sizeof(int) * myS);  if (tempArr == NULL) {fprintf(stderr, "cannot allocate partialPrefix\n");exit(-1);}
	int c=0;
    printf("\nrank in tempArr%llu:\n",my_rank);
	//getting partial array.
	    for(int p=my_start;p<my_end;p++){
        //loading the corresponding part of the array into temp
        tempArr[c]=arrayA[p];
        c++;
    }
    printf("\nrank in loading%llu:\n",my_rank);
	
	prefixSumA(tempArr,myS);
	/*
	if(my_rank != thread_count)
	prefixSumA(tempArr,myS);
	else
		prefixSumA(tempArr,myS+1);
	*/
	for(int t=0;t<myS;t++){
		
		//printf("tempPREXI CONT: %d\n",tempArr[t]);
	}
	
	
	printf("done with prefix %llu\n",my_rank);
    c=0;
    for(int y=my_start;y<my_end;y++){
        arrayC[y]=tempArr[c];
        c++;
    }
    printf("\nrank in assign%llu:\n",my_rank);
	//free(tempArr);
	return NULL;
}




static void* pthreadCalc2(void* rank){
	long long my_rank = (long long)rank;
	long my_start = my_rank * size / thread_count;
	long my_end = (my_rank + 1.0) * size / thread_count;
	
    
    //get your index from lastPrefix and subtract last value from arrayC
    long tempo=my_end-1.0;
	
    int additionValue=lastPrefix[my_rank]-arrayC[tempo];
    
   for(int y=my_start;y<my_end;y++){
        arrayC[y]+=additionValue;
    }
    
    
	return NULL;
}
