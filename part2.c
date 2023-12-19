#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

// Function to generate a random matrix
void generateRandomMatrix(int rows, int cols, int *matrix) {
    for (int i = 0; i < rows * cols; ++i) {
        matrix[i] = rand() % 10; // Adjust the range as needed
    }
}

// Function to print a matrix
void printMatrix(int rows, int cols, int *matrix) {
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            printf("%d ", matrix[i * cols + j]);
        }
        printf("\n");
    }
}

int main(int argc, char *argv[]) {
    int rank, size;

    if(argc != 3){
        printf("Too Few Arguments! Enter Matrix Row and Column\n");
        printf("./<programName> <rows> <columns>");
        return (-1);
    }

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int n = atoi(argv[1]); // Default dimensions for rows
    int m = atoi(argv[2]); // Default dimensions for columns

    // Allocate memory for matrices A, B, and the result matrix
    int *A = (int *)malloc(n * m * sizeof(int));
    int *B = (int *)malloc(n * m * sizeof(int));
    int *result = (int *)malloc(n * m * sizeof(int));

    // Generate random matrices A and B in the master process
    if (rank == 0) {
        generateRandomMatrix(n, m, A);
        generateRandomMatrix(n, m, B);
        printf("\nMatrix A:\n");
        printMatrix(n, m, A);
        printf("\nMatrix B:\n");
        printMatrix(n, m, B);
    }

    // Scatter rows of matrices A and B to different processes
    MPI_Scatter(A, n * m / size, MPI_INT, A, n * m / size, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Scatter(B, n * m / size, MPI_INT, B, n * m / size, MPI_INT, 0, MPI_COMM_WORLD);

    // Perform matrix addition for each process
    for (int i = 0; i < n * m / size; ++i) {
        result[i] = A[i] + B[i];
    }

    // Gather results from all processes to the master process
    MPI_Gather(result, n * m / size, MPI_INT, result, n * m / size, MPI_INT, 0, MPI_COMM_WORLD);

    // Verify the gathered matrix is the sum of A and B in the master process
    if (rank == 0) {
        printf("\nGathered Result Matrix:\n");
        printMatrix(n, m, result);

        int flag = 1;
        for (int i = 0; i < n * m; ++i) {
            if(result[i] != A[i] + B[i]){
                printf("\nMatrices Are Not Equal!\n");
                flag = 0;
                break;
            }
        }
        if(flag == 1){
            printf("\nMatrices are Equal!\n");
        }
    }

    // Free allocated memory
    free(A);
    free(B);
    free(result);

    MPI_Finalize();
    return 0;
}
