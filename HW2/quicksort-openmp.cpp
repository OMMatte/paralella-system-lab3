/* matrix summation using OpenMP
 
 usage with gcc (version 4.2 or higher required):
 gcc -O -fopenmp -o matrixSum-openmp matrixSum-openmp.c
 ./matrixSum-openmp size numWorkers
 
 */

#include <omp.h>
#include <vector>
#include <stdlib.h>
#include <time.h>
#include <stdio.h>

#define SIZE 100  /* standard array size */
#define MAXWORKERS 8   /* maximum number of workers */

double start_time, end_time;

long partition(long*, long, long);
void quicksort(long*, long, long);

int main(int argc, char *argv[]) {
    long numWorkers, size, maxValue, i;
    numWorkers = (argc > 2)? atoi(argv[1]) : MAXWORKERS;
    size = (argc > 2)? atoi(argv[2]) : SIZE;
    // if the maxValue parameter isn't given, we set maxValue to the same as size
    maxValue = (argc > 3)? atoi(argv[3]) : size;
    if (numWorkers > MAXWORKERS) numWorkers = MAXWORKERS;
    omp_set_num_threads(numWorkers);
    
    // create the unsorted array
    long * array = new long[size];
    for(long i = 0; i < maxValue; i++) {
        array[i] = rand() % maxValue;
    }
    
    start_time = omp_get_wtime();
    
#pragma omp parallel
    {
#pragma omp single nowait
        {
    quicksort(array, 0, size - 1);
        }
    }
    
    end_time = omp_get_wtime();
    
    delete[] array;
    
//    for(i = 0;i < size; i++){
//        printf("%d\t", array[i]);
//    }
//    printf("\n");
    
    printf("it took %g seconds\n", end_time - start_time);
    return 0;
}

long partition(long * a, long first, long last) {
    long * leftList = new long[last-first];
    long * rightList = new long[last-first];
    long i;
    long j;
    long pivot = a[last];
    long leftListCounter = 0;
    long rightListCounter = 0;
    
    for(i = first; i < last; i++){
        if(a[i] < pivot){
            leftList[leftListCounter++] = a[i];
        }else{
            rightList[rightListCounter++] = a[i];
        }
    }
    
    
    for(i = 0; i < leftListCounter; i++){
        a[first + i] = leftList[i];
    }
    
    a[first + leftListCounter] = pivot;
    
    for(j = 0; j < rightListCounter; j++){
        a[first + leftListCounter + j + 1] = rightList[j];
    }
    
    delete[] leftList;
    delete[] rightList;
    
    return first + leftListCounter;
}

void quicksort(long * a, long first, long last) {
    long splitter;
    
    if(first < last){
        //we only enter if we have more than 1 value to sort, sorting one element does not do much...
        //be begin by partioning and returning the position of the pivot that we partioned around
        splitter = partition(a, first, last);
        
//        #pragma omp parallel sections
//        {
            //here we do the partial work, section by section until we are done
             #pragma omp task
            quicksort(a, first, splitter - 1);
             #pragma omp task
            quicksort(a, splitter + 1, last);
//        }
    }
}