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

    // Master process generates the random matrix
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

    // Scatter the matrix among processes
    MPI_Scatter(originalMatrix, MATRIX_SIZE * MATRIX_SIZE / size, MPI_INT, receivedMatrix,
                MATRIX_SIZE * MATRIX_SIZE / size, MPI_INT, 0, MPI_COMM_WORLD);

    // Do computations here using receivedMatrix

    // Gather the results back to the master process
    MPI_Gather(receivedMatrix, MATRIX_SIZE * MATRIX_SIZE / size, MPI_INT, originalMatrix,
               MATRIX_SIZE * MATRIX_SIZE / size, MPI_INT, 0, MPI_COMM_WORLD);

    // Master process verifies the gathered matrix
    if (rank == 0) {
        printf("\nGathered Matrix:\n");
        for (int i = 0; i < MATRIX_SIZE; ++i) {
            for (int j = 0; j < MATRIX_SIZE; ++j) {
                printf("%d ", originalMatrix[i][j]);
            }
            printf("\n");
        }
        // Compare originalMatrix with the expected original matrix to verify
        // If they are the same, your MPI code is correct.
    }

    MPI_Finalize();
    return 0;
}
