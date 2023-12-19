#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <mpi.h>

#define MATRIX_SIZE 3

int main(int argc, char *argv[]) {
    int rank, size;
    int originalMatrix[MATRIX_SIZE][MATRIX_SIZE];
    int receivedMatrix[MATRIX_SIZE][MATRIX_SIZE];

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // generate a random matrix in the master process
    if (rank == 0) {
        srand(time(NULL));
        printf("Original Matrix:\n");
        for (int i = 0; i < MATRIX_SIZE; ++i) {
            for (int j = 0; j < MATRIX_SIZE; ++j) {
                originalMatrix[i][j] = rand() % 10; // Generating random numbers between 0 and 9
                printf("%d ", originalMatrix[i][j]);
            }
            printf("\n");
        }
    }

    // scatter the matrix among the different processes
    MPI_Scatter(originalMatrix, MATRIX_SIZE * MATRIX_SIZE / size, MPI_INT, receivedMatrix, MATRIX_SIZE * MATRIX_SIZE / size, MPI_INT, 0, MPI_COMM_WORLD);

    // print the processes and how many elements each process got
    printf("\nProcess %d, Partitioned Matrix:\n", rank);
    for (int i = 0; i < MATRIX_SIZE; ++i) {
        for (int j = 0; j < MATRIX_SIZE; ++j) {
            printf("%d ", receivedMatrix[i][j]);
        }
        printf("\n");
    }

    // gather the matrix back
    MPI_Gather(receivedMatrix, MATRIX_SIZE * MATRIX_SIZE / size, MPI_INT, originalMatrix, MATRIX_SIZE * MATRIX_SIZE / size, MPI_INT, 0, MPI_COMM_WORLD);

    // verify if the matrices were equal
    if (rank == 0) {
        printf("\nGathered Matrix:\n");
        for (int i = 0; i < MATRIX_SIZE; ++i) {
            for (int j = 0; j < MATRIX_SIZE; ++j) {
                printf("%d ", receivedMatrix[i][j]);
            }
            printf("\n");
        }

        int flag = 1;
        for(int i = 0; i < MATRIX_SIZE; i++){
            for(int j = 0; j < MATRIX_SIZE; j++){
                if(originalMatrix[i][j] != receivedMatrix[i][j]){
                    printf("\nThe Matrices Are Not Equal!!\n");
                    flag = 0;
                    break;
                }
            }
            if(flag == 0){
                break;
            }
        }
    }

    MPI_Finalize();
    return 0;
}