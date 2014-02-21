/* matrix summation using OpenMP
 
 usage with gcc (version 4.2 or higher required):
 gcc -O -fopenmp -o matrixSum-openmp matrixSum-openmp.c
 ./matrixSum-openmp size numWorkers
 
 */

#include <omp.h>
#include <vector>
#include <stdlib.h>
#include <time.h>

double start_time, end_time;

#include <stdio.h>
#define MAXSIZE 100000  /* maximum matrix size */
#define MAXWORKERS 8   /* maximum number of workers */

int numWorkers;
int size;
int matrix[MAXSIZE][MAXSIZE];

/* read command line, initialize, and create threads */
int main(int argc, char *argv[]) {
    int i, j, total=0;
    /* read command line args if any */
    size = (argc > 1)? atoi(argv[1]) : MAXSIZE;
    numWorkers = (argc > 2)? atoi(argv[2]) : MAXWORKERS;
    if (size > MAXSIZE) size = MAXSIZE;
    if (numWorkers > MAXWORKERS) numWorkers = MAXWORKERS;
    
    omp_set_num_threads(numWorkers);
    
    /* initialize the matrix */
    for (i = 0; i < size; i++) {
//          printf("[ ");
        for (j = 0; j < size; j++) {
            matrix[i][j] = rand()%99;
//            	  printf(" %d", matrix[i][j]);
        }
//        	  printf(" ]\n");
    }
    
    // initial min and max values to the first val in the matrix
    int minVal[3], maxVal[3] = {matrix[0][0], 0, 0};
    int minValPri[3];
    int maxValPri[3];
    start_time = omp_get_wtime();
#pragma omp parallel shared(minVal, maxVal) private(minValPri, maxValPri)
    {
        // we initial the each threads best min and max to the first value in the matrix aswell
        minValPri[0] = maxValPri[0] = matrix[0][0];
        minValPri[1] = minValPri[2] = maxValPri[1] = maxValPri[2];
        
    #pragma omp for reduction (+:total) private(j)
        for (i = 0; i < size; i++)
            for (j = 0; j < size; j++){
                total += matrix[i][j];
                //we update the private min and max values
                if(matrix[i][j] < minValPri[0]) {
                    minValPri[0] = matrix[i][j];
                    minValPri[1] = i;
                    minValPri[2] = j;
                }
                if(matrix[i][j] > maxValPri[0]) {
                    maxValPri[0] = matrix[i][j];
                    maxValPri[1] = i;
                    maxValPri[2] = j;
                }
            }
    
        
    #pragma omp critical
        {
            //lastly we enter a critical section where we update the shared min and max values
            if(minValPri[0] < minVal[0]) {
                minVal[0] = minValPri[0];
                minVal[1] = minValPri[1];
                minVal[2] = minValPri[2];
            }
            if(maxValPri[0] > maxVal[0]) {
                maxVal[0] = maxValPri[0];
                maxVal[1] = maxValPri[1];
                maxVal[2] = maxValPri[2];
            }
        }

    // implicit barrier
    }

    end_time = omp_get_wtime();
    
    // print the info
    printf("the total is %d\n", total);
    printf("Maximum element value is %d at row/col position %d/%d\n", maxVal[0], maxVal[1], maxVal[2]);
    printf("Minimum element value is %d at row/col position %d/%d\n", minVal[0], minVal[1], minVal[2]);
    printf("it took %g seconds\n", end_time - start_time);
}
